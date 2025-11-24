import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

def experiment1_plot():
    df = pd.read_csv("results_ex1.csv")

    # Ensure columns are numeric
    df["i_max"] = pd.to_numeric(df["i_max"], errors='coerce')
    df["time_seconds"] = pd.to_numeric(df["time_seconds"], errors='coerce')

    # Drop any rows with NaN after conversion
    df = df.dropna(subset=["i_max", "time_seconds"])

    # Sort by i_max
    df = df.sort_values(by="i_max")

    plt.figure(figsize=(8, 5))

    # Scatter plot
    plt.scatter(df["i_max"], df["time_seconds"], color='blue', label="Execution Time")

    plt.xlabel("i_max")
    plt.ylabel("time_seconds")
    plt.title("Execution Time vs i_max")

    # Optional: log scale for better visualization if i_max varies a lot
    # plt.xscale('log')
    # plt.yscale('log')

    # Force full numbers on x-axis
    plt.ticklabel_format(style='plain', axis='x')

    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig("./experiment_1.png")
    plt.close()



def experiment2_plot():
    # Read CSV
    df = pd.read_csv("results_ex2.csv")

    plt.figure(figsize=(10,6))

    # Use seaborn color palette
    palette = sns.color_palette("tab10", n_colors=df["t_max"].nunique())

    # Plot each t_max in a different color
    for idx, t_max in enumerate(sorted(df["t_max"].unique())):
        subset = df[df["t_max"] == t_max]
        plt.plot(subset["block_size"], subset["time_seconds"],
                 marker="o", label=f"t_max={t_max}", color=palette[idx])

    plt.xlabel("Number of CUDA Threads (block_size)")
    plt.ylabel("Elapsed Time (seconds)")
    plt.title("Experiment 2: Block Size vs Elapsed Time")
    plt.xscale("log")  # Optional: makes spacing nicer for powers of 2
    plt.grid(True, which="both", linestyle="--", alpha=0.5)
    plt.legend(title="t_max")

    plt.tight_layout()
    plt.savefig("experiment_2.png", dpi=300)
    plt.show()

experiment2_plot()

