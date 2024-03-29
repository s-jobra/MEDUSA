import sys
import pandas as pd
import matplotlib.pyplot as plt

def graph_plot(xdata, ydata, title, xlabel, ylabel):
    colors = plt.cm.get_cmap('tab10').colors

    plt.figure(figsize=(10, 6))
    legend_handles = []
    legend_labels = []
    for i, col in enumerate(ydata.columns):
        values = ydata[col].replace({'TO': None, 'Error': None, '---': None}).astype(float)
        if not values.isnull().all():  # Check if any non-null values exist - to not show in legend
            line, = plt.plot(xdata, values, marker='o', color=colors[i])
            legend_handles.append(line)
            legend_labels.append(col.rsplit(' ', 1)[0])
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.xticks(rotation=45, ha='right')
    plt.legend(legend_handles, legend_labels, loc='upper left', bbox_to_anchor=(0, 1))
    plt.gca().xaxis.set_major_locator(plt.MultipleLocator(5))  # Show every 5th label
    plt.show()

def plot_benchmark_data(csv_file, benchmark_name):
    df = pd.read_csv(csv_file)

    benchmark_data = df[df['Circuit'].str.startswith(benchmark_name+'/')]
    circuit_names = [test.split('/')[1] for test in benchmark_data['Circuit']]

    runtime_data = benchmark_data.iloc[:, 1::2]  # Select every other column starting from index 1
    memory_usage_data = benchmark_data.iloc[:, 2::2]  # Select every other column starting from index 2

    graph_plot(circuit_names, runtime_data, 'Runtime for ' + benchmark_name, 'Circuit Name', 'Runtime [s]')
    graph_plot(circuit_names, memory_usage_data, 'Memory Usage for ' + benchmark_name, 'Circuit Name', 'Memory Usage [MB]')


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Invalid number of arguments, usage: python plot.py <csv_file> <benchmark_name>")
        sys.exit(1)

    csv_file = sys.argv[1]
    benchmark_name = sys.argv[2]
    plot_benchmark_data(csv_file, benchmark_name)

