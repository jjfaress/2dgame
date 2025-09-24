import json
import seaborn as sns
import matplotlib.pyplot as plt
from pathlib import Path
import pandas as pd
import numpy as np
from datetime import datetime

sns.set_theme()

def load_and_process_benchmark_data(results_path="./Results"):
    """Load and process benchmark JSON files into a time-series ready DataFrame"""
    results = Path(results_path).glob("*.json")
    processed_data = []
    
    for file in results:
        with open(file, 'r') as f:
            data = json.load(f)
            date = pd.to_datetime(data["context"]["date"])
            
            # Process each benchmark in the file
            for benchmark in data["benchmarks"]:
                processed_data.append({
                    "date": date,
                    "benchmark_name": benchmark["name"],
                    "run_name": benchmark["run_name"],
                    "iterations": benchmark["iterations"],
                    "real_time": benchmark["real_time"],
                    "cpu_time": benchmark["cpu_time"],
                    "time_unit": benchmark["time_unit"]
                })
    
    df = pd.DataFrame(processed_data)
    return df

def create_time_series_plots(df):
    """Create comprehensive time series plots for benchmark data"""
    
    # Set up the plotting style
    plt.style.use('default')
    sns.set_palette("husl")
    
    # Create figure with subplots
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    fig.suptitle('Benchmark Performance Over Time', fontsize=16, fontweight='bold')
    
    # Plot 1: Real Time by Benchmark
    ax1 = axes[0, 0]
    for benchmark in df['benchmark_name'].unique():
        benchmark_data = df[df['benchmark_name'] == benchmark]
        ax1.plot(benchmark_data['date'], benchmark_data['real_time'], 
                marker='o', label=benchmark, linewidth=2, markersize=6)
    
    ax1.set_title('Real Time Performance', fontweight='bold')
    ax1.set_xlabel('Date')
    ax1.set_ylabel('Real Time (ns)')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    ax1.tick_params(axis='x', rotation=45)
    
    # Plot 2: CPU Time by Benchmark
    ax2 = axes[0, 1]
    for benchmark in df['benchmark_name'].unique():
        benchmark_data = df[df['benchmark_name'] == benchmark]
        ax2.plot(benchmark_data['date'], benchmark_data['cpu_time'], 
                marker='s', label=benchmark, linewidth=2, markersize=6)
    
    ax2.set_title('CPU Time Performance', fontweight='bold')
    ax2.set_xlabel('Date')
    ax2.set_ylabel('CPU Time (ns)')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    ax2.tick_params(axis='x', rotation=45)
    
    # Plot 3: Iterations by Benchmark
    ax3 = axes[1, 0]
    for benchmark in df['benchmark_name'].unique():
        benchmark_data = df[df['benchmark_name'] == benchmark]
        ax3.plot(benchmark_data['date'], benchmark_data['iterations'], 
                marker='^', label=benchmark, linewidth=2, markersize=6)
    
    ax3.set_title('Iterations Count', fontweight='bold')
    ax3.set_xlabel('Date')
    ax3.set_ylabel('Iterations')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    ax3.tick_params(axis='x', rotation=45)
    
    # Plot 4: Performance Efficiency (iterations per unit time)
    ax4 = axes[1, 1]
    for benchmark in df['benchmark_name'].unique():
        benchmark_data = df[df['benchmark_name'] == benchmark]
        # Calculate efficiency as iterations per millisecond of real time
        efficiency = benchmark_data['iterations'] / (benchmark_data['real_time'] / 1e6)
        ax4.plot(benchmark_data['date'], efficiency, 
                marker='d', label=benchmark, linewidth=2, markersize=6)
    
    ax4.set_title('Performance Efficiency (Iterations/ms)', fontweight='bold')
    ax4.set_xlabel('Date')
    ax4.set_ylabel('Efficiency (iter/ms)')
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    ax4.tick_params(axis='x', rotation=45)
    
    plt.tight_layout()
    return fig

def create_heatmap_view(df):
    """Create a heatmap showing performance across benchmarks and time"""
    
    # Create a pivot table for heatmap
    pivot_data = df.pivot_table(
        index='benchmark_name', 
        columns='date', 
        values='real_time', 
        aggfunc='mean'
    )
    
    # Create heatmap
    plt.figure(figsize=(12, 6))
    sns.heatmap(pivot_data, annot=True, fmt='.0f', cmap='viridis', 
                cbar_kws={'label': 'Real Time (ns)'})
    plt.title('Benchmark Performance Heatmap Over Time', fontweight='bold', pad=20)
    plt.xlabel('Date')
    plt.ylabel('Benchmark')
    plt.xticks(rotation=45)
    plt.tight_layout()
    return plt.gcf()

def create_comparative_analysis(df):
    """Create comparative analysis plots"""
    
    fig, axes = plt.subplots(1, 2, figsize=(15, 6))
    
    # Box plot comparing benchmarks
    ax1 = axes[0]
    df.boxplot(column='real_time', by='benchmark_name', ax=ax1)
    ax1.set_title('Real Time Distribution by Benchmark')
    ax1.set_xlabel('Benchmark')
    ax1.set_ylabel('Real Time (ns)')
    
    # Scatter plot: Real Time vs CPU Time
    ax2 = axes[1]
    for benchmark in df['benchmark_name'].unique():
        benchmark_data = df[df['benchmark_name'] == benchmark]
        ax2.scatter(benchmark_data['real_time'], benchmark_data['cpu_time'], 
                   label=benchmark, alpha=0.7, s=60)
    
    ax2.set_title('Real Time vs CPU Time')
    ax2.set_xlabel('Real Time (ns)')
    ax2.set_ylabel('CPU Time (ns)')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    return fig

# Main execution
if __name__ == "__main__":
    # Load and process the data
    df = load_and_process_benchmark_data()
    print(f"Loaded {len(df)} benchmark records")
    print(f"Date range: {df['date'].min()} to {df['date'].max()}")
    print(f"Benchmarks: {df['benchmark_name'].unique()}")
    
    # Create visualizations
    print("\nCreating time series plots...")
    fig1 = create_time_series_plots(df)
    plt.show()
    
    print("\nCreating heatmap view...")
    fig2 = create_heatmap_view(df)
    plt.show()
    
    print("\nCreating comparative analysis...")
    fig3 = create_comparative_analysis(df)
    plt.show()
    
    # Optional: Save the processed data
    df.to_csv('processed_benchmark_data.csv', index=False)
    print("\nProcessed data saved to 'processed_benchmark_data.csv'")

# Example of how to filter and analyze specific time periods
def analyze_time_period(df, start_date=None, end_date=None):
    """Analyze benchmarks for a specific time period"""
    if start_date:
        df = df[df['date'] >= pd.to_datetime(start_date)]
    if end_date:
        df = df[df['date'] <= pd.to_datetime(end_date)]
    
    summary = df.groupby('benchmark_name').agg({
        'real_time': ['mean', 'std', 'min', 'max'],
        'cpu_time': ['mean', 'std', 'min', 'max'],
        'iterations': ['mean', 'std', 'min', 'max']
    }).round(2)
    
    return summary

# Example usage for trend analysis
def detect_performance_trends(df):
    """Detect performance trends for each benchmark"""
    trends = {}
    
    for benchmark in df['benchmark_name'].unique():
        benchmark_data = df[df['benchmark_name'] == benchmark].sort_values('date')
        
        if len(benchmark_data) > 1:
            # Calculate trend using linear regression
            x = np.arange(len(benchmark_data))
            real_time_trend = np.polyfit(x, benchmark_data['real_time'], 1)[0]
            cpu_time_trend = np.polyfit(x, benchmark_data['cpu_time'], 1)[0]
            
            trends[benchmark] = {
                'real_time_trend': real_time_trend,
                'cpu_time_trend': cpu_time_trend,
                'trend_direction': 'improving' if real_time_trend < 0 else 'degrading'
            }
    
    return trends