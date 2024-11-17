#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}Starting VANET Simulation...${NC}"

# Clean previous results
rm -f results/trace.tr results/nam.tr results/performance_analysis.png

# Run NS2 simulation
echo -e "\n${GREEN}Running NS2 simulation...${NC}"
ns simulation.tcl

# Check if simulation completed successfully
if [ $? -eq 0 ]; then
    echo -e "${GREEN}Simulation completed successfully${NC}"
    
    # Analyze results
    echo -e "\n${GREEN}Analyzing simulation results...${NC}"
    python3 scripts/analyze_trace.py results/trace.tr
    
    # Display NAM visualization
    echo -e "\n${GREEN}Opening NAM visualizer...${NC}"
    nam results/nam.tr &
    
    echo -e "\n${GREEN}Results have been saved to:${NC}"
    echo "- Trace file: results/trace.tr"
    echo "- NAM trace: results/nam.tr"
    echo "- Performance analysis: results/performance_analysis.png"
else
    echo -e "${RED}Simulation failed${NC}"
    exit 1
fi
