#!/usr/bin/env python3

import sys
import pandas as pd
import matplotlib.pyplot as plt

class TraceAnalyzer:
    def __init__(self, trace_file):
        self.trace_file = trace_file
        self.data = []
        
    def parse_trace(self):
        with open(self.trace_file, 'r') as f:
            for line in f:
                if line.startswith('s') or line.startswith('r'):  # sent or received packets
                    parts = line.split()
                    try:
                        self.data.append({
                            'event': parts[0],
                            'time': float(parts[1]),
                            'node': int(parts[2].strip('_')),  # Strip underscores if present
                            'packet_type': parts[3],
                            'size': int(parts[5])
                        })
                    except ValueError as e:
                        print(f"Skipping malformed line: {line.strip()} - Error: {e}")

    
    def analyze_performance(self):
        df = pd.DataFrame(self.data)
        
        # Calculate packet delivery ratio
        sent = df[df['event'] == 's'].shape[0]
        received = df[df['event'] == 'r'].shape[0]
        pdr = (received / sent) * 100 if sent > 0 else 0
        
        # Calculate throughput over time
        df['throughput'] = df.groupby(df['time'].astype(int))['size'].transform('sum')
        
        # Plot results
        plt.figure(figsize=(12, 6))
        
        plt.subplot(1, 2, 1)
        plt.plot(df['time'].unique(), df.groupby('time')['throughput'].first())
        plt.title('Throughput over Time')
        plt.xlabel('Time (s)')
        plt.ylabel('Throughput (bytes)')
        
        plt.subplot(1, 2, 2)
        plt.bar(['Sent', 'Received'], [sent, received])
        plt.title(f'Packet Delivery Ratio: {pdr:.2f}%')
        
        plt.tight_layout()
        plt.savefig('results/performance_analysis.png')
        
        return {
            'packet_delivery_ratio': pdr,
            'average_throughput': df['throughput'].mean(),
            'total_packets_sent': sent,
            'total_packets_received': received
        }

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python analyze_trace.py <trace_file>")
        sys.exit(1)
        
    analyzer = TraceAnalyzer(sys.argv[1])
    analyzer.parse_trace()
    results = analyzer.analyze_performance()
    
    print("\nSimulation Results:")
    print("-----------------")
    for metric, value in results.items():
        print(f"{metric.replace('_', ' ').title()}: {value:.2f}")
