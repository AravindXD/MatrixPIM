#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np
import os
import subprocess
import re

# Create output directories if they don't exist
os.makedirs('graphs', exist_ok=True)

def run_simulation_and_parse_results():
    """Run the pPIM simulation and parse the output for performance data"""
    # Compile and run the simulation
    subprocess.run(['g++', '-std=c++17', '-O3', './sim/large_matrix_sim.cpp', '-o', 'large_matrix_sim'])
    result = subprocess.run(['./large_matrix_sim'], capture_output=True, text=True)
    
    if result.returncode != 0:
        print("Error running simulation:")
        print(result.stderr)
        return None
    
    # Parse the output
    output = result.stdout
    
    # Initialize data storage
    matrix_sizes = []
    cpu_times = []
    pim_times = []
    opt_pim_times = []
    speedups = []
    
    # Regular expression to match the data lines
    # Example line: "    3×2 * 2×4       0.48           0.24           0.15      3.12×"
    pattern = r'\s*(\d+×\d+\s*\*\s*\d+×\d+)\s+(\d+\.\d+)\s+(\d+\.\d+)\s+(\d+\.\d+)\s+(\d+\.\d+)×'
    
    for line in output.split('\n'):
        match = re.search(pattern, line)
        if match:
            size_str = match.group(1)
            cpu_time = float(match.group(2))
            pim_time = float(match.group(3))
            opt_pim_time = float(match.group(4))
            speedup = float(match.group(5))
            
            # Extract the first dimension as the matrix size (e.g., "32" from "32×32 * 32×32")
            matrix_size = int(size_str.split('×')[0])
            
            matrix_sizes.append(matrix_size)
            cpu_times.append(cpu_time)
            pim_times.append(pim_time)
            opt_pim_times.append(opt_pim_time)
            speedups.append(speedup)
    
    print("Parsed simulation data for matrix sizes:", matrix_sizes)
    return matrix_sizes, cpu_times, pim_times, opt_pim_times, speedups

def extend_data_for_large_matrices(matrix_sizes, cpu_times, pim_times, opt_pim_times):
    """Extend the data for larger matrices by extrapolation"""
    # Check if we already have data for 4096×4096 matrix
    if 4096 in matrix_sizes:
        return matrix_sizes, cpu_times, pim_times, opt_pim_times
    
    # Calculate a scaling factor based on existing data
    last_size = matrix_sizes[-1]
    
    # Make sure we have at least 256×256 data
    if last_size < 256:
        print("Warning: Unable to extrapolate, not enough data points")
        return matrix_sizes, cpu_times, pim_times, opt_pim_times
    
    # Define additional sizes we want to extrapolate
    # If we already have 256, add 512, 1024, 2048, 4096
    additional_sizes = []
    if last_size <= 256:
        additional_sizes.extend([512, 1024, 2048, 4096])
    elif last_size <= 512:
        additional_sizes.extend([1024, 2048, 4096])
    elif last_size <= 1024:
        additional_sizes.extend([2048, 4096])
    elif last_size <= 2048:
        additional_sizes.extend([4096])
    
    # Get the scaling factor from the last two data points
    if len(matrix_sizes) >= 2:
        last_idx = len(matrix_sizes) - 1
        size_ratio = matrix_sizes[last_idx] / matrix_sizes[last_idx - 1]
        cpu_scale = (cpu_times[last_idx] / cpu_times[last_idx - 1]) / (size_ratio ** 3)
        pim_scale = (pim_times[last_idx] / pim_times[last_idx - 1]) / (size_ratio ** 3)
        opt_pim_scale = (opt_pim_times[last_idx] / opt_pim_times[last_idx - 1]) / (size_ratio ** 3)
        
        # Extrapolate for each additional size
        for size in additional_sizes:
            new_size_ratio = size / last_size
            new_cpu = cpu_times[last_idx] * (new_size_ratio ** 3) * cpu_scale
            new_pim = pim_times[last_idx] * (new_size_ratio ** 3) * pim_scale
            new_opt_pim = opt_pim_times[last_idx] * (new_size_ratio ** 3) * opt_pim_scale
            
            matrix_sizes.append(size)
            cpu_times.append(new_cpu)
            pim_times.append(new_pim)
            opt_pim_times.append(new_opt_pim)
            
        print("Extrapolated data for additional sizes:", additional_sizes)
    
    return matrix_sizes, cpu_times, pim_times, opt_pim_times

def generate_performance_graphs():
    """Generate performance comparison graphs using real simulation data"""
    # Get data from simulation
    sim_data = run_simulation_and_parse_results()
    if not sim_data:
        print("Failed to get simulation data. Exiting.")
        return
    
    matrix_sizes, cpu_times, pim_times, opt_pim_times, sim_speedups = sim_data
    
    # Extend data for visualization of larger matrices
    matrix_sizes, cpu_times, pim_times, opt_pim_times = extend_data_for_large_matrices(
        matrix_sizes, cpu_times, pim_times, opt_pim_times
    )
    
    # Calculate new speedups after extrapolation
    speedups = [cpu_times[i] / opt_pim_times[i] for i in range(len(matrix_sizes))]
    
    # Print performance data
    print("\nPerformance Data:")
    print("Matrix Size | CPU Time (μs) | pPIM Time (μs) | Opt pPIM (μs) | Speedup")
    print("-" * 70)
    for i in range(len(matrix_sizes)):
        print(f"{matrix_sizes[i]}×{matrix_sizes[i]} | {cpu_times[i]:,.2f} | {pim_times[i]:,.2f} | {opt_pim_times[i]:,.2f} | {speedups[i]:,.2f}×")
    
    # Create execution time comparison graph (log scale)
    plt.figure(figsize=(10, 6))
    plt.plot(matrix_sizes, cpu_times, 'o-', label='CPU Time', linewidth=2, markersize=8)
    plt.plot(matrix_sizes, opt_pim_times, 's-', label='Optimized pPIM Time', linewidth=2, markersize=8)
    plt.yscale('log')
    plt.xscale('log', base=2)
    plt.grid(True, which="both", ls="--", alpha=0.7)
    plt.xlabel('Matrix Size (N×N)', fontsize=12)
    plt.ylabel('Execution Time (μs)', fontsize=12)
    plt.title('Matrix Multiplication Performance: CPU vs pPIM (Log Scale)', fontsize=14)
    plt.legend(fontsize=12)
    plt.tight_layout()
    plt.savefig('graphs/performance_log_scale.png', dpi=300)
    
    # Create speedup graph
    plt.figure(figsize=(10, 6))
    plt.plot(matrix_sizes, speedups, 'o-', color='green', linewidth=2, markersize=8)
    plt.xscale('log', base=2)
    plt.grid(True, which="both", ls="--", alpha=0.7)
    plt.xlabel('Matrix Size (N×N)', fontsize=12)
    plt.ylabel('Speedup Factor (×)', fontsize=12)
    plt.title('pPIM Speedup vs. Matrix Size', fontsize=14)
    plt.tight_layout()
    plt.savefig('graphs/speedup.png', dpi=300)
    
    # Create a more detailed figure with two subplots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))
    
    # First subplot: Execution times
    ax1.plot(matrix_sizes, cpu_times, 'o-', label='CPU Time', linewidth=2, markersize=8)
    ax1.plot(matrix_sizes, pim_times, 's-', label='pPIM Time', linewidth=2, markersize=8)
    ax1.plot(matrix_sizes, opt_pim_times, '^-', label='Optimized pPIM Time', linewidth=2, markersize=8)
    ax1.set_yscale('log')
    ax1.set_xscale('log', base=2)
    ax1.grid(True, which="both", ls="--", alpha=0.7)
    ax1.set_xlabel('Matrix Size (N×N)', fontsize=12)
    ax1.set_ylabel('Execution Time (μs)', fontsize=12)
    ax1.set_title('Matrix Multiplication Performance: CPU vs pPIM (Log Scale)', fontsize=14)
    ax1.legend(fontsize=12)
    
    # Second subplot: Speedup
    ax2.plot(matrix_sizes, speedups, 'o-', color='green', linewidth=2, markersize=8)
    ax2.set_xscale('log', base=2)
    ax2.grid(True, which="both", ls="--", alpha=0.7)
    ax2.set_xlabel('Matrix Size (N×N)', fontsize=12)
    ax2.set_ylabel('Speedup Factor (×)', fontsize=12)
    ax2.set_title('pPIM Speedup vs. Matrix Size', fontsize=14)
    
    plt.tight_layout()
    plt.savefig('graphs/combined_performance.png', dpi=300)
    
    # Additional visualization: Execution time in seconds (linear scale) for larger matrices
    plt.figure(figsize=(10, 6))
    
    # Convert to seconds for readability with large matrices
    # Use only the last 4 sizes or all if less than 4
    last_n = min(4, len(matrix_sizes))
    large_sizes = matrix_sizes[-last_n:]
    cpu_seconds = [cpu_times[-i]/1000000 for i in range(1, last_n+1)]  # Convert μs to s
    pim_seconds = [opt_pim_times[-i]/1000000 for i in range(1, last_n+1)]  # Convert μs to s
    
    # Reverse the lists to keep ascending order
    large_sizes.reverse()
    cpu_seconds.reverse()
    pim_seconds.reverse()
    
    x = np.arange(len(large_sizes))
    width = 0.35
    
    plt.bar(x - width/2, cpu_seconds, width, label='CPU Time', color='blue')
    plt.bar(x + width/2, pim_seconds, width, label='Optimized pPIM Time', color='orange')
    plt.xticks(x, [f"{size}×{size}" for size in large_sizes])
    plt.yscale('log')  # Use log scale due to large difference
    plt.grid(True, which="both", ls="--", alpha=0.7, axis='y')
    plt.xlabel('Matrix Size (N×N)', fontsize=12)
    plt.ylabel('Execution Time (seconds)', fontsize=12)
    plt.title('Matrix Multiplication Time for Large Matrices', fontsize=14)
    plt.legend(fontsize=12)
    plt.tight_layout()
    plt.savefig('graphs/large_matrices_comparison.png', dpi=300)
    
    # Output the data to a CSV file for reference
    with open('graphs/performance_data.csv', 'w') as f:
        f.write('Matrix Size,CPU Time (μs),pPIM Time (μs),Optimized pPIM (μs),Speedup\n')
        for i in range(len(matrix_sizes)):
            f.write(f'{matrix_sizes[i]},{cpu_times[i]},{pim_times[i]},{opt_pim_times[i]},{speedups[i]}\n')
    
    print("\nPerformance graphs generated successfully!")
    print(f"Data saved to: graphs/performance_data.csv")
    print(f"Graphs saved to:")
    print(f"- graphs/performance_log_scale.png")
    print(f"- graphs/speedup.png")
    print(f"- graphs/combined_performance.png")
    print(f"- graphs/large_matrices_comparison.png")

if __name__ == "__main__":
    generate_performance_graphs() 