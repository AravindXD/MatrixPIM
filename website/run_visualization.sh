#!/bin/bash

# Get directory paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"

# Run the setup script first
echo "Running setup script..."
"$SCRIPT_DIR/setup_visualization.sh"

# Check if Python is installed
if ! command -v python3 &> /dev/null; then
    echo "Python 3 is not installed. Please install Python 3 to run this visualization."
    exit 1
fi

# Check if Streamlit is installed
if ! python3 -c "import streamlit" &> /dev/null; then
    echo "Streamlit is not installed. Installing required packages..."
    pip install streamlit numpy pandas matplotlib pillow
fi

# Change to the website directory
cd "$SCRIPT_DIR"

# Run the Streamlit app
echo "Starting pPIM Compiler Visualization..."
streamlit run app.py 