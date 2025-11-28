import requests
import time
import os
import argparse
from datetime import datetime

# --- ANSI Color Codes for Terminal Output ---
class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

def clear_screen():
    """Clears the terminal screen."""
    os.system('cls' if os.name == 'nt' else 'clear')

def get_energy_bar(energy):
    """Generates a simple text-based energy bar with color."""
    if energy is None:
        return "---"
    
    bar_char = "█"
    num_bars = int(energy / 10)
    bar = bar_char * num_bars + " " * (10 - num_bars)
    
    color = Colors.RED
    if energy > 50:
        color = Colors.GREEN
    elif energy > 20:
        color = Colors.YELLOW
        
    return f"[{color}{bar}{Colors.RESET}] {energy:5.1f}%"

def main():
    """Main function to run the swarm monitor."""
    parser = argparse.ArgumentParser(
        description="Monitor a swarm of EMBER bots in real-time via their JSON APIs.",
        formatter_class=argparse.RawTextHelpFormatter,
        epilog="""
Example usage:
  # Monitor 9 bots (default, ember-bot-0 to ember-bot-8)
  python swarm_monitor.py

  # Monitor 3 bots (ember-bot-0 to ember-bot-2)
  python swarm_monitor.py --count 3

  # Change the refresh rate to 10 seconds
  python swarm_monitor.py --refresh 10
"""
    )
    parser.add_argument('--count', type=int, default=9, help='Number of bots in the swarm to monitor (default: 9).')
    parser.add_argument('--refresh', type=int, default=3, help='Refresh interval in seconds (default: 3).')
    parser.add_argument('--base-host', type=str, default="ember-bot", help='Base hostname for the bots (default: "ember-bot").')
    
    args = parser.parse_args()

    bot_hosts = [f"{args.base_host}-{i}.local" for i in range(args.count)]

    try:
        while True:
            clear_screen()
            
            # --- Print Header ---
            print(Colors.BOLD + "="*120 + Colors.RESET)
            print(f"{Colors.BOLD}{Colors.BLUE}EMBER SWARM MONITOR{Colors.RESET}".center(130))
            print(Colors.BOLD + "="*120 + Colors.RESET)
            
            header = (
                f"{'BOT':>4} | {'STATUS':<8} | {'ENERGY':<20} | {'ALIVE (s)':>10} | {'GEN':>4} | "
                f"{'LIGHT':>7} | {'THRESHOLD':>9} | {'EFFICIENCY':>10} | {'IP ADDRESS':<15}"
            )
            print(Colors.BOLD + header + Colors.RESET)
            print("-" * 120)

            # --- Poll Each Bot ---
            for i, host in enumerate(bot_hosts):
                try:
                    response = requests.get(f"http://{host}/api/stats", timeout=2)
                    response.raise_for_status()
                    data = response.json()

                    # Format data for display
                    status = f"{Colors.GREEN}ALIVE{Colors.RESET}" if data.get('alive') else f"{Colors.RED}DEAD{Colors.RESET}"
                    energy_bar = get_energy_bar(data.get('energy'))
                    alive_time = data.get('alive_time', '---')
                    generation = data.get('generation', '-')
                    light = f"{data.get('light_level', 0):.3f}"
                    threshold = f"{data.get('threshold', 0):.3f}"
                    efficiency = f"{data.get('efficiency', 0):.3f}"
                    ip = data.get('ip', 'N/A') # Assuming IP is added to API

                    print(
                        f"{i:>4} | {status:<18} | {energy_bar:<30} | {alive_time:>10} | {generation:>4} | "
                        f"{light:>7} | {threshold:>9} | {efficiency:>10} | {ip:<15}"
                    )

                except (requests.exceptions.RequestException, requests.exceptions.HTTPError):
                    status = f"{Colors.YELLOW}OFFLINE{Colors.RESET}"
                    print(
                        f"{i:>4} | {status:<18} | {'---':<20} | {'---':>10} | {'---':>4} | "
                        f"{'---':>7} | {'---':>9} | {'---':>10} | {'---':<15}"
                    )
            
            # --- Print Footer ---
            print("-" * 120)
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            footer = f"Last updated: {timestamp} | Refreshing every {args.refresh}s | Press Ctrl+C to exit"
            print(footer)

            time.sleep(args.refresh)

    except KeyboardInterrupt:
        print("\n\nSwarm monitor stopped. Goodbye!")
    except Exception as e:
        print(f"\nAn unexpected error occurred: {e}")

if __name__ == "__main__":
    main()
