import serial
import csv
import argparse
from datetime import datetime
import time
import re
import sys

def parse_ember_stats(line):
    """
    Parses a line of EMBER serial output using regular expressions.
    Example: "Light: 0.512 | Energy: 73.2 | Alive: 342s | Status: ALIVE | IP: 192.168.1.50"
    Returns a dictionary of the parsed values.
    """
    stats = {}
    patterns = {
        'light': r'Light:\s*([0-9.]+)',
        'energy': r'Energy:\s*([0-9.]+)',
        'alive_time': r'Alive:\s*([0-9]+)s',
        'status': r'Status:\s*(\w+)',
        'ip': r'IP:\s*([0-9.]+)'
    }

    for key, pattern in patterns.items():
        match = re.search(pattern, line)
        if match:
            stats[key] = match.group(1)
        else:
            stats[key] = None # Field not found in this line

    # Only return a dict if the essential 'light' field was found
    if 'light' in stats and stats['light'] is not None:
        return stats
    return None

def main():
    """Main function to run the serial logger."""
    parser = argparse.ArgumentParser(
        description="Log EMBER bot fitness data from a serial port to a CSV file.",
        formatter_class=argparse.RawTextHelpFormatter,
        epilog="""
Example usage:
  - On Windows: python serial_logger.py COM3
  - On macOS:   python serial_logger.py /dev/tty.usbserial-1410
  - On Linux:   python serial_logger.py /dev/ttyUSB0

The script will create a CSV file named 'ember_log_YYYYMMDD_HHMMSS.csv'.
Press Ctrl+C to stop logging.
"""
    )
    parser.add_argument('port', help='The serial port the EMBER bot is connected to (e.g., COM3 or /dev/ttyUSB0).')
    parser.add_argument('--baud', type=int, default=115200, help='Baud rate for the serial connection (default: 115200).')
    
    args = parser.parse_args()

    # --- Setup CSV File ---
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"ember_log_{timestamp}.csv"
    csv_header = ['timestamp', 'light', 'energy', 'alive_time', 'status', 'ip']

    try:
        # --- Setup Serial Connection ---
        print(f"Attempting to connect to {args.port} at {args.baud} baud...")
        ser = serial.Serial(args.port, args.baud, timeout=1)
        print(f"Successfully connected to {args.port}. Listening for data...")
        print(f"Logging data to: {filename}")
        print("Press Ctrl+C to stop.")
        
        with open(filename, 'w', newline='') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=csv_header)
            writer.writeheader()

            while True:
                try:
                    if ser.in_waiting > 0:
                        line = ser.readline().decode('utf-8', errors='ignore').strip()
                        
                        if not line:
                            continue

                        # Check if the line contains the expected stats format
                        if "Light:" in line and "Energy:" in line:
                            stats = parse_ember_stats(line)
                            if stats:
                                stats['timestamp'] = datetime.now().isoformat()
                                writer.writerow(stats)
                                # Flush the writer buffer to ensure data is written to disk
                                csvfile.flush() 
                                print(f"Logged: {stats}")
                        else:
                            # Print other non-stats lines for context (e.g., boot messages)
                            print(f"INFO: {line}")

                except serial.SerialException as e:
                    print(f"\nERROR: Serial device disconnected or read error: {e}")
                    break
                except KeyboardInterrupt:
                    print("\nStopping logger...")
                    break
                except Exception as e:
                    print(f"\nAn unexpected error occurred: {e}")
                    break

    except serial.SerialException as e:
        print(f"ERROR: Could not open serial port {args.port}. Please check the port name and permissions.")
        print(f"Details: {e}")
        sys.exit(1)
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serial port closed.")
        print("Logging complete.")

if __name__ == "__main__":
    main()
