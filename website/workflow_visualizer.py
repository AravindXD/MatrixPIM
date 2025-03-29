import os
import json
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image, ImageDraw, ImageFont
import streamlit as st

class WorkflowVisualizer:
    """
    Helper class for visualizing the pPIM compiler workflow
    """
    
    @staticmethod
    def ensure_directory_exists(path):
        """
        Ensure the directory exists, and create it if it doesn't
        """
        directory = os.path.dirname(path)
        if not os.path.exists(directory):
            os.makedirs(directory, exist_ok=True)
    
    @staticmethod
    def create_matrix_diagram(matrix_a, matrix_b, output_path="website/temp_diagram.png"):
        """
        Create a visualization of matrix multiplication with actual result
        """
        # Calculate the result matrix
        matrix_c = np.matmul(matrix_a, matrix_b)
        
        # Create a new image with white background
        width, height = 900, 450
        image = Image.new('RGB', (width, height), color='white')
        draw = ImageDraw.Draw(image)
        
        # Try to use a nice font, fall back to default if not available
        try:
            font = ImageFont.truetype("DejaVuSans.ttf", 16)
            title_font = ImageFont.truetype("DejaVuSans-Bold.ttf", 20)
        except IOError:
            # Use default font if the requested font is not available
            font = ImageFont.load_default()
            title_font = ImageFont.load_default()
        
        # Draw title
        draw.text((width//2 - 150, 20), "Matrix Multiplication Process", fill='black', font=title_font)
        
        # Calculate matrix sizes and positions
        m1_rows, m1_cols = matrix_a.shape
        m2_rows, m2_cols = matrix_b.shape
        m3_rows, m3_cols = matrix_c.shape
        
        # Scale matrices for visualization
        scale_factor = min(120 / max(m1_rows, m1_cols, m2_rows, m2_cols, m3_rows, m3_cols), 30)
        cell_size = max(min(scale_factor, 40), 20)
        
        # Draw Matrix A
        m1_x, m1_y = 100, 100
        draw.text((m1_x, m1_y - 30), "Matrix A", fill='blue', font=font)
        
        for i in range(m1_rows):
            for j in range(m1_cols):
                x = m1_x + j * cell_size
                y = m1_y + i * cell_size
                draw.rectangle([x, y, x + cell_size, y + cell_size], outline='black')
                value_text = str(matrix_a[i, j])
                # Position text in center of cell
                text_width = len(value_text) * 7  # Approximate width based on font size
                draw.text((x + cell_size//2 - text_width//2, y + cell_size//2 - 8), 
                         value_text, fill='black', font=font)
        
        # Draw multiplication symbol
        draw.text((m1_x + m1_cols * cell_size + 30, m1_y + (m1_rows * cell_size) // 2 - 10), 
                 "×", fill='black', font=title_font)
        
        # Draw Matrix B
        m2_x = m1_x + m1_cols * cell_size + 80
        m2_y = 100
        draw.text((m2_x, m2_y - 30), "Matrix B", fill='green', font=font)
        
        for i in range(m2_rows):
            for j in range(m2_cols):
                x = m2_x + j * cell_size
                y = m2_y + i * cell_size
                draw.rectangle([x, y, x + cell_size, y + cell_size], outline='black')
                value_text = str(matrix_b[i, j])
                # Position text in center of cell
                text_width = len(value_text) * 7  # Approximate width based on font size
                draw.text((x + cell_size//2 - text_width//2, y + cell_size//2 - 8), 
                         value_text, fill='black', font=font)
        
        # Draw equals symbol
        draw.text((m2_x + m2_cols * cell_size + 30, m1_y + (m1_rows * cell_size) // 2 - 10), 
                 "=", fill='black', font=title_font)
        
        # Draw result Matrix C
        m3_x = m2_x + m2_cols * cell_size + 80
        m3_y = 100
        draw.text((m3_x, m3_y - 30), "Matrix C (Result)", fill='red', font=font)
        
        for i in range(m3_rows):
            for j in range(m3_cols):
                x = m3_x + j * cell_size
                y = m3_y + i * cell_size
                draw.rectangle([x, y, x + cell_size, y + cell_size], outline='black', fill='#ffeeee')
                
                # Add the actual value from the calculated result
                value_text = str(matrix_c[i, j])
                # Position text in center of cell
                text_width = len(value_text) * 7  # Approximate width based on font size
                draw.text((x + cell_size//2 - text_width//2, y + cell_size//2 - 8), 
                         value_text, fill='black', font=font)
        
        # Add explanation of the matrix multiplication process
        explanation_y = m1_y + max(m1_rows, m2_rows, m3_rows) * cell_size + 40
        draw.text((width//2 - 250, explanation_y), 
                 "Matrix Multiplication: C[i,j] = ∑ A[i,k] × B[k,j] (for all k)", 
                 fill='black', font=font)
        
        # Add a simple visual example for one element calculation
        if m1_rows > 0 and m2_cols > 0:
            example_y = explanation_y + 40
            example_text = f"Example: C[0,0] = "
            
            # Create the equation for the first element
            terms = []
            for k in range(m1_cols):
                terms.append(f"A[0,{k}]×B[{k},0]")
            
            example_text += " + ".join(terms)
            example_text += f" = {matrix_c[0,0]}"
            
            draw.text((width//2 - 200, example_y), example_text, fill='black', font=font)
        
        # Save the image
        WorkflowVisualizer.ensure_directory_exists(output_path)
        image.save(output_path)
        return output_path
    
    @staticmethod
    def create_compilation_flow_diagram(output_path="website/temp_flow.png"):
        """
        Create a visual diagram of the compilation flow
        """
        # Create a figure for the flowchart
        fig, ax = plt.subplots(figsize=(10, 6))
        
        # Disable axis
        ax.axis('off')
        
        # Data for the flowchart
        stages = [
            "C++ Matrix Code",
            "Parser",
            "Matrix Operations",
            "Optimizer",
            "Code Generator",
            "pPIM Assembly"
        ]
        
        # Position the boxes
        box_positions = np.linspace(0, 1, len(stages))
        box_width = 0.15
        box_height = 0.1
        
        # Colors for the boxes
        colors = ['#FFC0CB', '#FFD700', '#98FB98', '#ADD8E6', '#D8BFD8', '#FFDAB9']
        
        # Draw boxes and arrows
        for i, (stage, color) in enumerate(zip(stages, colors)):
            # Draw box
            rect = plt.Rectangle(
                (box_positions[i] - box_width/2, 0.45), 
                box_width, box_height, 
                facecolor=color, 
                edgecolor='black',
                alpha=0.8
            )
            ax.add_patch(rect)
            
            # Add text
            ax.text(
                box_positions[i], 0.5, 
                stage, 
                ha='center', va='center', 
                fontsize=10, fontweight='bold'
            )
            
            # Draw arrow
            if i < len(stages) - 1:
                ax.annotate(
                    '', 
                    xy=(box_positions[i+1] - box_width/2, 0.5),
                    xytext=(box_positions[i] + box_width/2, 0.5),
                    arrowprops=dict(arrowstyle='->', lw=1.5, color='black')
                )
        
        # Add title
        ax.text(0.5, 0.9, 'pPIM Compiler Workflow', 
                ha='center', fontsize=14, fontweight='bold')
        
        # Save the figure
        WorkflowVisualizer.ensure_directory_exists(output_path)
        plt.tight_layout()
        plt.savefig(output_path, dpi=100, bbox_inches='tight')
        plt.close()
        
        return output_path
    
    @staticmethod
    def create_asm_visualization(asm_code, output_path="website/temp_asm.png"):
        """
        Create a visual representation of the assembly code structure using a bar chart
        """
        # Parse the assembly code
        lines = asm_code.strip().split('\n')
        
        # Count instruction types
        prog_count = sum(1 for line in lines if line.startswith('PROG '))
        read_count = sum(1 for line in lines if 'EXE Read' in line)
        compute_count = sum(1 for line in lines if 'EXE CorePtr' in line)
        write_count = sum(1 for line in lines if 'EXE Write' in line)
        
        # Create a bar chart
        labels = ['PROG\n(LUT Config)', 'Read\nOperations', 'Compute\nOperations', 'Write\nOperations']
        counts = [prog_count, read_count, compute_count, write_count]
        colors = ['#ff9999', '#66b3ff', '#99ff99', '#ffcc99']
        
        fig, ax = plt.subplots(figsize=(10, 6))
        bars = ax.bar(labels, counts, color=colors)
        
        # Add count labels on top of bars
        for bar in bars:
            height = bar.get_height()
            ax.annotate(
                f'{height}',
                xy=(bar.get_x() + bar.get_width() / 2, height),
                xytext=(0, 3),  # 3 points vertical offset
                textcoords="offset points",
                ha='center', va='bottom',
                fontsize=12
            )
        
        # Customize chart
        ax.set_ylabel('Instruction Count', fontsize=12)
        ax.set_title('Distribution of Assembly Instructions', fontsize=14, fontweight='bold')
        ax.spines['top'].set_visible(False)
        ax.spines['right'].set_visible(False)
        ax.tick_params(axis='y', which='major', labelsize=10)
        ax.tick_params(axis='x', which='major', labelsize=10)
        
        # Add total instruction count as text
        total_instructions = sum(counts)
        ax.text(
            0.5, 0.9, 
            f'Total Instructions: {total_instructions}',
            transform=ax.transAxes,
            ha='center',
            fontsize=12
        )
        
        # Save the figure
        WorkflowVisualizer.ensure_directory_exists(output_path)
        plt.tight_layout()
        plt.savefig(output_path, dpi=100, bbox_inches='tight')
        plt.close()
        
        return output_path
    
    @staticmethod
    def performance_comparison_chart(cpu_time, pim_time, output_path="website/temp_perf.png"):
        """
        Create an enhanced performance comparison chart
        """
        # Data for the performance comparison
        categories = ['Memory Access', 'Computation', 'Overhead']
        
        # These values are estimates based on typical performance profiles
        cpu_breakdown = [70, 20, 10]  # 70% memory access, 20% computation, 10% overhead
        pim_breakdown = [10, 80, 10]  # 10% memory access, 80% computation, 10% overhead
        
        # Calculate absolute times for each category
        cpu_times = [cpu_time * p / 100 for p in cpu_breakdown]
        pim_times = [pim_time * p / 100 for p in pim_breakdown]
        
        # Create a figure with two subplots
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
        
        # First subplot - Detailed breakdown
        x = np.arange(len(categories))  # the label locations
        width = 0.35  # the width of the bars
        
        rects1 = ax1.bar(x - width/2, cpu_times, width, label='CPU', color='#ff9999')
        rects2 = ax1.bar(x + width/2, pim_times, width, label='pPIM', color='#99ff99')
        
        # Add some text for labels, title and custom x-axis tick labels, etc.
        ax1.set_ylabel('Time (μs)', fontsize=12)
        ax1.set_title('Performance Breakdown by Category', fontsize=14, fontweight='bold')
        ax1.set_xticks(x)
        ax1.set_xticklabels(categories, fontsize=10)
        ax1.legend(fontsize=10)
        ax1.spines['top'].set_visible(False)
        ax1.spines['right'].set_visible(False)
        
        # Add value labels on top of bars
        def autolabel(rects):
            for rect in rects:
                height = rect.get_height()
                ax1.annotate(f'{height:.1f}',
                            xy=(rect.get_x() + rect.get_width() / 2, height),
                            xytext=(0, 3),  # 3 points vertical offset
                            textcoords="offset points",
                            ha='center', va='bottom', fontsize=9)
        
        autolabel(rects1)
        autolabel(rects2)
        
        # Second subplot - Overall comparison
        ax2.bar(['CPU'], [cpu_time], color='#ff9999', label='CPU')
        ax2.bar(['pPIM'], [pim_time], color='#99ff99', label='pPIM')
        
        # Add a line connecting the tops of the bars to highlight the difference
        if pim_time < cpu_time:  # Only if pPIM is faster
            ax2.plot(['CPU', 'pPIM'], [cpu_time, pim_time], 'k--', alpha=0.5)
            # Add annotation for speedup
            speedup = cpu_time / pim_time
            midpoint_x = 0.5
            midpoint_y = (cpu_time + pim_time) / 2
            ax2.annotate(f'{speedup:.1f}× Speedup',
                        xy=(midpoint_x, midpoint_y),
                        xytext=(0, 20),
                        textcoords="offset points",
                        ha='center', fontsize=12,
                        arrowprops=dict(arrowstyle='->', lw=1.5))
        
        ax2.set_ylabel('Total Time (μs)', fontsize=12)
        ax2.set_title('Overall Performance Comparison', fontsize=14, fontweight='bold')
        ax2.spines['top'].set_visible(False)
        ax2.spines['right'].set_visible(False)
        
        # Add value labels on top of bars
        for i, v in enumerate([cpu_time, pim_time]):
            label = 'CPU' if i == 0 else 'pPIM'
            ax2.text(i, v + 5, f'{v:.1f} μs', ha='center', fontsize=10)
        
        # Adjust y-axis for better visualization (ensuring the difference is clearly visible)
        y_max = max(cpu_time, pim_time) * 1.2
        ax2.set_ylim(0, y_max)
        
        fig.tight_layout()
        WorkflowVisualizer.ensure_directory_exists(output_path)
        plt.savefig(output_path, dpi=100, bbox_inches='tight')
        plt.close()
        
        return output_path 