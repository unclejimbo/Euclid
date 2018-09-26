"""Plotting the heat kernel signatures stored in csv file.
"""
import sys
import numpy as np
import matplotlib.pyplot as plt


def plot_hks(csv_file):
    """Plot.

    Args:
        csv_file (str) : The file storing hks.
    """

    hks = np.genfromtxt(csv_file, delimiter=',')
    x = np.linspace(0, 100, num=100, endpoint=False)
    plt.scatter(x, hks[:, 240], label='tip 1')
    plt.scatter(x, hks[:, 276], label='tip 1')
    plt.scatter(x, hks[:, 3], label='cube corner 1')
    plt.scatter(x, hks[:, 39], label='cube corner 1')
    plt.scatter(x, hks[:, 87], label='concave 1')
    plt.scatter(x, hks[:, 785], label='concave 1')
    plt.legend()
    plt.show()


if __name__ == "__main__":
    plot_hks(sys.argv[1])
