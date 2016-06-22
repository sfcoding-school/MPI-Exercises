from matplotlib import pyplot as plt
from math import floor, ceil
def plot_speedup(n, serial, times, labels):
    plt.plot(n, n)
    plt.hold(True)
    clrs = [".r-", "oy-", "xg-"]
    for i in xrange(len(times)):
        speedup = [serial[i] / j for j in times[i]]
        plt.plot(n, speedup, clrs[i])
    plt.xlim([4.95, 16.05])
    plt.ylabel("SpeedUp")
    plt.xlabel("N. di processi")
    plt.legend(["Ideale"] + labels)
    plt.show()
def plot_efficiency(n, serial, times, labels):
    plt.plot(n, [1 for i in n])
    plt.hold(True)
    clrs = [".r-", "oy-", "xg-"]
    for i in xrange(len(times)):
        efficiecy = [(serial[i] / j) / a[i] for j in times[i]]
        plt.plot(n, efficiecy, clrs[i])
    plt.xlim([4.95, 16.05])
    plt.ylim([0.0, 1.2])
    plt.ylabel("Efficienza")
    plt.xlabel("N. di processi")
    plt.legend(["Ideale"] + labels)
    plt.show()
a = [5, 7, 9, 10, 12, 14, 16]
b = [3.0, 10.0, 27.0]
c = [[17.206264, 13.442121, 12.813312, 10.149984, 11.568818, 9.912965, 12.587979],
     [192.578522, 193.387209, 191.758068, 87.567464, 95.065402, 151.326846, 109.712253],
     [361.432620, 361.946067, 355.118384, 214.338215, 204.481319, 174.402304, 270.071495]]
d = ["1000x601", "2000x901", "3000x1201"]
plot_speedup(a, b, c, d)
plot_efficiency(a, b, c, d)
