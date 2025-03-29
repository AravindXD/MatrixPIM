# pPIM Compiler Visualization Frontend

This directory contains a comprehensive visualization frontend for the pPIM compiler, built using Streamlit. The frontend allows users to visually explore the entire workflow of the pPIM compiler, from matrix input to assembly code generation and performance analysis.

## Features

- **Matrix Input**: Input matrices for multiplication either manually or generate random ones
- **Compilation Visualization**: View the step-by-step compilation process from parsing to code generation
- **LUT Visualization**: Explore how Look-Up Tables are programmed for in-memory computing
- **Performance Analysis**: Compare execution times between CPU and pPIM implementations

## Setup and Installation

1. Ensure you have Python 3.8+ installed
2. Install the required dependencies:
   ```
   pip install streamlit numpy pandas matplotlib
   ```
3. Run the Streamlit app:
   ```
   cd website
   streamlit run app.py
   ```

## Usage

1. **Input Matrices**: Start by entering matrix dimensions and values on the Matrix Input page
2. **Generate Assembly**: Click "Compile and Generate Assembly" to process the matrices
3. **Explore**: Navigate through the tabs to visualize the compilation process, LUT programming, and performance comparison

## Directory Structure

- `app.py`: Main Streamlit application
- `lut_state_*.html`: HTML files containing LUT programming visualizations
- `index.html`: Index page for LUT visualizations

## Integration with pPIM Compiler

The frontend integrates with the pPIM compiler by:
1. Taking matrix input and formatting it for the compiler
2. Displaying the generated assembly code
3. Visualizing the LUT programming
4. Comparing performance metrics

## Extending the Visualization

To add new visualization components:
1. Add new functions to `app.py`
2. Create new pages in the navigation sidebar
3. Use Streamlit's layout and visualization tools to display data 