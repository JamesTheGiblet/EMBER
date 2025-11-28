import requests
import csv
import time
import argparse
import random
from datetime import datetime

# --- ANSI Color Codes for Terminal Output ---
class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

# --- Helper Functions for API Interaction ---

def get_bot_data(bot_id, base_host):
    """Fetches and returns the JSON data for a single bot."""
    url = f"http://{base_host}-{bot_id}.local/api/stats"
    try:
        response = requests.get(url, timeout=3)
        response.raise_for_status()
        return response.json()
    except requests.exceptions.RequestException as e:
        print(f"{Colors.YELLOW}WARNING: Could not fetch data for bot {bot_id}. {e}{Colors.RESET}")
        return None

def send_bot_command(bot_id, base_host, command, params=None):
    """Sends a command to a bot's API endpoint (e.g., /reset, /mutate)."""
    url = f"http://{base_host}-{bot_id}.local/{command}"
    try:
        requests.get(url, timeout=3, params=params)
        return True
    except requests.exceptions.RequestException as e:
        print(f"{Colors.YELLOW}WARNING: Could not send '{command}' to bot {bot_id}. {e}{Colors.RESET}")
        return False

# --- Core Evolution Functions ---

def initialize_population(num_bots, base_host):
    """Randomizes the genome of every bot in the population for Generation 0."""
    print(f"{Colors.CYAN}Initializing population for Generation 0 by randomizing all genomes...{Colors.RESET}")
    for i in range(num_bots):
        print(f"  Randomizing bot {i}...")
        send_bot_command(i, base_host, "randomize")
        time.sleep(0.2) # Stagger requests slightly
    print(f"{Colors.GREEN}Population initialized.{Colors.RESET}")

def run_generation(generation_time, num_bots, base_host):
    """Resets all bots and monitors them for the duration of the generation."""
    print(f"{Colors.CYAN}Resetting all bots to 100% energy for the new generation...{Colors.RESET}")
    for i in range(num_bots):
        send_bot_command(i, base_host, "reset")
    
    print(f"{Colors.GREEN}Generation running for {generation_time} seconds. Monitoring...{Colors.RESET}")
    
    # Monitor progress during the generation
    monitoring_interval = 30 # seconds
    start_time = time.time()
    while time.time() - start_time < generation_time:
        remaining_time = generation_time - (time.time() - start_time)
        if remaining_time < 0: break
        
        # Count how many bots are still alive
        alive_count = 0
        for i in range(num_bots):
            data = get_bot_data(i, base_host)
            if data and data.get('alive', False):
                alive_count += 1
        
        print(f"  {int(remaining_time)}s remaining... {alive_count}/{num_bots} bots are still alive.")
        time.sleep(min(monitoring_interval, remaining_time))

def log_generation_data(generation, num_bots, base_host, filename):
    """Logs the final state of all bots to a CSV file."""
    print(f"{Colors.CYAN}Logging fitness and genome data for Generation {generation}...{Colors.RESET}")
    
    is_new_file = not os.path.exists(filename)
    with open(filename, 'a', newline='') as f:
        writer = csv.writer(f)
        if is_new_file:
            writer.writerow(['generation', 'bot_id', 'alive_time', 'alive', 'energy', 
                           'light_threshold', 'efficiency', 'timestamp'])
        
        for i in range(num_bots):
            data = get_bot_data(i, base_host)
            if data:
                writer.writerow([
                    generation, data.get('bot_id'), data.get('alive_time'), data.get('alive'),
                    data.get('energy'), data.get('threshold'), data.get('efficiency'),
                    datetime.now().isoformat()
                ])
    print(f"{Colors.GREEN}Data logged to {filename}.{Colors.RESET}")

def select_and_breed(num_bots, base_host, selection_pressure):
    """Selects the fittest bots and breeds the next generation via crossover."""
    print(f"{Colors.CYAN}Selecting winners and breeding next generation...{Colors.RESET}")
    
    # 1. Collect data from all bots
    all_bots_data = [get_bot_data(i, base_host) for i in range(num_bots)]
    all_bots_data = [b for b in all_bots_data if b is not None] # Filter out offline bots

    if not all_bots_data:
        print(f"{Colors.RED}ERROR: Could not retrieve data from any bots. Cannot proceed.{Colors.RESET}")
        return

    # 2. Select winners
    all_bots_data.sort(key=lambda x: x.get('alive_time', 0), reverse=True)
    num_winners = max(1, int(num_bots * selection_pressure))
    winners = all_bots_data[:num_winners]

    if not winners:
        print(f"{Colors.RED}ERROR: No winning bots found. Using all available bots as parents.{Colors.RESET}")
        winners = all_bots_data

    print(f"{Colors.BOLD}--- Top {len(winners)} Winners ---{Colors.RESET}")
    for bot in winners:
        print(f"  Bot {bot['bot_id']}: Fitness={bot['alive_time']}s, "
              f"Genome=(T:{bot['threshold']:.3f}, E:{bot['efficiency']:.3f})")
    
    # 3. Breed next generation using crossover
    print(f"{Colors.CYAN}Breeding new population...{Colors.RESET}")
    for i in range(num_bots):
        # Pick two random parents from the winners pool
        parent1 = random.choice(winners)
        parent2 = random.choice(winners)
        
        # Crossover: create a new genome
        new_genome = {
            'threshold': parent1['threshold'],
            'efficiency': parent2['efficiency'],
        }
        
        # Apply the new genome to the bot
        send_bot_command(i, base_host, 'threshold', params={'value': new_genome['threshold']})
        send_bot_command(i, base_host, 'efficiency', params={'value': new_genome['efficiency']})
        send_bot_command(i, base_host, 'save') # Ensure it's saved to flash
        print(f"  Bot {i} received new genome from parents {parent1['bot_id']} and {parent2['bot_id']}.")
        time.sleep(0.2)

def mutate_population(num_bots, base_host):
    """Applies a mutation to every bot in the population."""
    print(f"{Colors.CYAN}Applying mutations to the new generation...{Colors.RESET}")
    for i in range(num_bots):
        send_bot_command(i, base_host, "mutate")
    print(f"{Colors.GREEN}All bots mutated and genomes saved.{Colors.RESET}")

# --- Main Experiment Runner ---

def main():
    parser = argparse.ArgumentParser(
        description="Automate a full EMBER evolution experiment.",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument('--generations', type=int, default=10, help='Number of generations to run.')
    parser.add_argument('--time', type=int, default=1800, help='Time per generation in seconds (e.g., 1800 for 30 mins).')
    parser.add_argument('--count', type=int, default=9, help='Number of bots in the swarm.')
    parser.add_argument('--pressure', type=float, default=0.33, help='Selection pressure: fraction of top bots to select as winners (e.g., 0.33 for top 33%%).')
    parser.add_argument('--host', type=str, default="ember-bot", help='Base hostname for the bots.')
    parser.add_argument('--log-file', type=str, default=f"evolution_log_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv", help='CSV file to log experiment data.')
    
    args = parser.parse_args()

    print(f"{Colors.BOLD}{Colors.MAGENTA}{'='*60}{Colors.RESET}")
    print(f"{Colors.BOLD}{Colors.MAGENTA}EMBER AUTOMATED EVOLUTION EXPERIMENT{Colors.RESET}".center(70))
    print(f"{Colors.BOLD}{Colors.MAGENTA}{'='*60}{Colors.RESET}")
    print(f"  Population Size: {args.count} bots")
    print(f"  Generations:     {args.generations}")
    print(f"  Generation Time: {args.time}s ({args.time/60:.1f} minutes)")
    print(f"  Selection:       Top {args.pressure*100:.0f}%")
    print(f"  Log File:        {args.log_file}")
    print(f"{Colors.BOLD}{Colors.MAGENTA}{'='*60}{Colors.RESET}")

    try:
        # Generation 0: Special case for initialization
        print(f"\n{Colors.BOLD}--- Starting Generation 0 ---{Colors.RESET}")
        initialize_population(args.count, args.host)
        run_generation(args.time, args.count, args.host)
        log_generation_data(0, args.count, args.host, args.log_file)
        
        # Main evolution loop
        for gen in range(1, args.generations):
            print(f"\n{Colors.BOLD}--- Starting Generation {gen} ---{Colors.RESET}")
            select_and_breed(args.count, args.host, args.pressure)
            mutate_population(args.count, args.host)
            run_generation(args.time, args.count, args.host)
            log_generation_data(gen, args.count, args.host, args.log_file)

        print(f"\n{Colors.BOLD}{Colors.GREEN}Evolution experiment complete after {args.generations} generations!{Colors.RESET}")
        print(f"Full experiment data is available in '{args.log_file}'.")
        print(f"Use 'genome_analyzer.py {args.log_file}' to analyze the results.")

    except KeyboardInterrupt:
        print("\n\nExperiment stopped by user. Goodbye!")
    except Exception as e:
        print(f"\n{Colors.RED}An unexpected error occurred: {e}{Colors.RESET}")

if __name__ == "__main__":
    main()
