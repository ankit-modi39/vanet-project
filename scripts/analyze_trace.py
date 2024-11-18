import pandas as pd
import re
import matplotlib.pyplot as plt

class TraceAnalyzer:
    def __init__(self, trace_file):
        self.trace_file = trace_file

    def parse_trace(self):
        """Parses the trace file and extracts relevant columns."""
        data = []
        with open(self.trace_file, 'r') as file:
            for line in file:
                # Match trace format: event, time, node, layer, pkt_type, pkt_size, and flow_id
                match = re.match(
                    r"^(?P<event>\S+)\s+(?P<time>\S+)\s+(?P<node>\S+)\s+(?P<layer>\S+)\s+---\s+\S+\s+(?P<pkt_type>\S+)\s+(?P<size>\S+).*\[(?P<flow_id>\S+)\]",
                    line
                )
                if match:
                    data.append(match.groupdict())
        
        # Convert to DataFrame
        df = pd.DataFrame(data)
        df['time'] = pd.to_numeric(df['time'])
        df['size'] = pd.to_numeric(df['size'])
        return df

    def analyze_performance(self):
        """Analyzes performance metrics."""
        df = self.parse_trace()

        # Filter CBR packets
        cbr_packets = df[df['pkt_type'] == 'cbr']

        # Separate sent and received packets
        sent_packets = cbr_packets[cbr_packets['event'] == 's']
        received_packets = cbr_packets[cbr_packets['event'] == 'r']

        # Identify unique packets by flow_id
        unique_sent = sent_packets['flow_id'].nunique()
        unique_received = received_packets['flow_id'].nunique()

        # Compute performance metrics
        pdr = (unique_received / unique_sent) * 100 if unique_sent > 0 else 0
        throughput = received_packets['size'].sum() / df['time'].max() if not received_packets.empty else 0

        return {
            'Packet Delivery Ratio (%)': pdr,
            'Average Throughput (bytes/sec)': throughput,
            'Total Packets Sent': unique_sent,
            'Total Packets Received': unique_received
        }

    def plot_performance(self, metrics):
        """Plots performance metrics."""
        labels = list(metrics.keys())
        values = list(metrics.values())

        plt.figure(figsize=(10, 6))
        plt.bar(labels, values)
        plt.xlabel('Metrics')
        plt.ylabel('Values')
        plt.title('VANET Simulation Performance')
        plt.show()


if __name__ == "__main__":
    trace_file = "results/trace.tr"
    analyzer = TraceAnalyzer(trace_file)

    try:
        results = analyzer.analyze_performance()
        print("\nSimulation Results:")
        print("-----------------")
        for metric, value in results.items():
            print(f"{metric}: {value:.2f}")

        analyzer.plot_performance(results)
    except Exception as e:
        print(f"Error analyzing the trace file: {e}")
