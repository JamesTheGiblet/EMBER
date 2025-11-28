import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import argparse
import sys
from pathlib import Path

def analyze_genome_data(log_file):
    """
    Analyzes a CSV log file from an EMBER evolution experiment to find
    correlations between genome parameters and fitness (survival time).

    This script requires pandas, matplotlib, and seaborn.
    Install them with: pip install pandas matplotlib seaborn
    """
    filepath = Path(log_file)
    if not filepath.is_file():
        print(f"ERROR: File not found at '{log_file}'")
        sys.exit(1)

    print(f"Analyzing log file: {filepath.name}...")

    # --- 1. Load and Prepare Data ---
    try:
        df = pd.read_csv(filepath)
        # Ensure correct data types
        for col in ['light_threshold', 'efficiency', 'alive_time', 'energy']:
            if col in df.columns:
                df[col] = pd.to_numeric(df[col], errors='coerce')
        df.dropna(subset=['light_threshold', 'efficiency', 'alive_time'], inplace=True)
        
        if df.empty:
            print("ERROR: No valid data found in the CSV file after cleaning.")
            return

    except Exception as e:
        print(f"ERROR: Could not read or parse the CSV file. Details: {e}")
        return

    # --- 2. Define Fitness and Identify Winners ---
    # We define "winners" as the top 33% of bots by survival time.
    fitness_threshold = df['alive_time'].quantile(0.67)
    winners = df[df['alive_time'] >= fitness_threshold]
    losers = df[df['alive_time'] < fitness_threshold]

    print(f"\nDefining 'Winners' as bots with survival time >= {fitness_threshold:.0f} seconds.")
    print(f"Found {len(winners)} winners and {len(losers)} losers.")

    if len(winners) == 0:
        print("WARNING: No winners found based on the fitness threshold. Analysis might be limited.")
        # Use all data for plotting if no clear winners
        winners = df

    # --- 3. Print Statistical Summary ---
    print("\n" + "="*60)
    print("GENOME ANALYSIS SUMMARY")
    print("="*60)
    print("\n--- Winning Genomes (Top 33%) ---")
    print(winners[['light_threshold', 'efficiency', 'alive_time']].describe())

    if not losers.empty:
        print("\n--- Losing Genomes (Bottom 67%) ---")
        print(losers[['light_threshold', 'efficiency', 'alive_time']].describe())
    
    print("\n" + "="*60)
    print("INSIGHTS:")
    
    # Compare mean values
    avg_win_thresh = winners['light_threshold'].mean()
    avg_win_eff = winners['efficiency'].mean()
    
    if not losers.empty:
        avg_lose_thresh = losers['light_threshold'].mean()
        avg_lose_eff = losers['efficiency'].mean()
        
        if avg_win_thresh < avg_lose_thresh:
            print(f"- Winning bots tend to have a LOWER light threshold (Avg: {avg_win_thresh:.3f}) than losers (Avg: {avg_lose_thresh:.3f}).")
        else:
            print(f"- Winning bots tend to have a HIGHER light threshold (Avg: {avg_win_thresh:.3f}) than losers (Avg: {avg_lose_thresh:.3f}).")

        if avg_win_eff > avg_lose_eff:
            print(f"- Winning bots tend to have a HIGHER efficiency (Avg: {avg_win_eff:.3f}) than losers (Avg: {avg_lose_eff:.3f}).")
        else:
            print(f"- Winning bots tend to have a LOWER efficiency (Avg: {avg_win_eff:.3f}) than losers (Avg: {avg_lose_eff:.3f}).")
    else:
         print(f"- Average winning threshold: {avg_win_thresh:.3f}")
         print(f"- Average winning efficiency: {avg_win_eff:.3f}")

    print("="*60)


    # --- 4. Generate Visualizations ---
    print("\nGenerating plots...")
    
    # Create a figure for the plots
    fig = plt.figure(figsize=(18, 10))
    fig.suptitle(f'Genome Analysis for {filepath.name}', fontsize=16)

    # Plot 1: Fitness Landscape (Threshold vs. Efficiency)
    ax1 = fig.add_subplot(2, 2, 1)
    sns.scatterplot(
        data=df,
        x='light_threshold',
        y='efficiency',
        hue='alive_time',
        size='alive_time',
        sizes=(20, 200),
        palette='viridis',
        ax=ax1
    )
    ax1.set_title('Fitness Landscape: Genome vs. Survival Time')
    ax1.set_xlabel('Light Threshold')
    ax1.set_ylabel('Efficiency')
    ax1.grid(True, linestyle='--', alpha=0.6)
    ax1.legend(title='Survival Time (s)')

    # Plot 2: Distribution of Winning vs. Losing Thresholds
    ax2 = fig.add_subplot(2, 2, 3)
    sns.histplot(data=losers, x='light_threshold', color="red", alpha=0.6, label='Losers', kde=True, ax=ax2)
    sns.histplot(data=winners, x='light_threshold', color="green", alpha=0.6, label='Winners', kde=True, ax=ax2)
    ax2.set_title('Distribution of Light Thresholds')
    ax2.set_xlabel('Light Threshold')
    ax2.legend()

    # Plot 3: Distribution of Winning vs. Losing Efficiencies
    ax3 = fig.add_subplot(2, 2, 4)
    sns.histplot(data=losers, x='efficiency', color="red", alpha=0.6, label='Losers', kde=True, ax=ax3)
    sns.histplot(data=winners, x='efficiency', color="green", alpha=0.6, label='Winners', kde=True, ax=ax3)
    ax3.set_title('Distribution of Efficiencies')
    ax3.set_xlabel('Efficiency')
    ax3.legend()

    # Plot 4: Evolution over Generations (if applicable)
    if 'generation' in df.columns:
        ax4 = fig.add_subplot(2, 2, 2)
        gen_data = df.groupby('generation').agg(
            avg_thresh=('light_threshold', 'mean'),
            avg_eff=('efficiency', 'mean'),
            avg_fitness=('alive_time', 'mean')
        ).reset_index()

        # Plot average fitness on primary y-axis
        ax4.plot(gen_data['generation'], gen_data['avg_fitness'], color='blue', marker='o', label='Avg. Fitness (s)')
        ax4.set_xlabel('Generation')
        ax4.set_ylabel('Average Survival Time (s)', color='blue')
        ax4.tick_params(axis='y', labelcolor='blue')
        
        # Create a secondary y-axis for genome parameters
        ax4b = ax4.twinx()
        ax4b.plot(gen_data['generation'], gen_data['avg_thresh'], color='green', linestyle='--', marker='x', label='Avg. Threshold')
        ax4b.plot(gen_data['generation'], gen_data['avg_eff'], color='purple', linestyle='--', marker='s', label='Avg. Efficiency')
        ax4b.set_ylabel('Genome Value', color='green')
        ax4b.tick_params(axis='y', labelcolor='green')

        ax4.set_title('Evolution of Population Over Generations')
        fig.legend(loc="upper right", bbox_to_anchor=(0.9,0.9), bbox_transform=ax4.transAxes)
    else:
        # If no generation data, show a simple correlation plot
        ax4 = fig.add_subplot(2, 2, 2)
        sns.regplot(data=df, x='light_threshold', y='alive_time', ax=ax4, scatter_kws={'alpha':0.5})
        ax4.set_title('Correlation: Threshold vs. Survival')
        ax4.set_ylabel('Survival Time (s)')
        ax4.grid(True, linestyle='--', alpha=0.6)


    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    
    # Save the plot
    output_filename = filepath.stem + '_analysis.png'
    plt.savefig(output_filename, dpi=300)
    print(f"\nAnalysis plots saved to: {output_filename}")
    plt.show()


def main():
    """Main function to parse arguments and run the analyzer."""
    parser = argparse.ArgumentParser(
        description="Analyze EMBER genome data from a CSV log file.",
        formatter_class=argparse.RawTextHelpFormatter,
        epilog="""
This script reads a CSV file generated by serial_logger.py or a similar tool.
It identifies which genetic traits (light_threshold, efficiency) correlate with
higher fitness (survival time) and generates a statistical summary and plots.

Example usage:
  python genome_analyzer.py ember_log_20251128_220510.csv
  python genome_analyzer.py evolution_log.csv
"""
    )
    parser.add_argument('logfile', help='Path to the CSV log file to analyze.')
    
    args = parser.parse_args()
    analyze_genome_data(args.logfile)

if __name__ == "__main__":
    main()
