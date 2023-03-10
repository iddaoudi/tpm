import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os.path
import csv

plot = 0

def average(a):
    summ = []
    for i in range(len(a[0])):
        tmp = 0
        for j in range(len(a)):
            tmp += a[j][i]
        summ.append(tmp)
    for i in range(len(summ)):
        summ[i] = summ[i]/len(a)
    return summ

def compare(a):
    comp = []
    for i in range(len(a) - 1):
        tmp = 100 - (a[i]*100)/a[len(a)-1]
        comp.append(tmp)
    return comp

machines   = ["cascadelake", "skylake", "haswell"]
algorithms = ["cholesky", "qr", "lu", "sparselu"]
matrices   = ["16384", "32768"]
tiles      = ["1024", "2048"]
threads    = ["24", "48"] #FIXME
iterations = ["1", "2", "3", "4", "5"]

labels = ["c1","c2","c3","c4","c5","c6","c7","c8","c9","c10","c11","c12","c13","c14","c15","c16"]
labelslu = ["c1","c2","c3","c4","c5","c6","c7","c8"]

delimiter = "_"
bars = 0
width = 0.2

for machine in machines:
    for algo in algorithms:
        if algo == "lu":
            lab = labelslu
        else:
            lab = labels
        bars = len(lab)
        ind  = np.arange(bars)
        ind2 = ind[:-1]
        for matrix in matrices:
            for tile in tiles:
                for thread in threads:
                    dram0 = []
                    dram1 = []
                    pkg0 = []
                    pkg1 = []
                    for iteration in iterations:
                        file = machine + delimiter + algo + delimiter +
                        matrix + delimiter + tile + delimiter + thread +
                        delimiter + "min" + delimiter + iteration
                        if os.path.exists(file):
                            fdram0 = file + "/dram_0.dat"
                            fdram1 = file + "/dram_1.dat"
                            fpkg0 = file + "/pkg_0.dat"
                            fpkg1 = file + "/pkg_1.dat"
                            fd0 = open(fdram0, encoding="utf8").read()
                            fd1 = open(fdram1, encoding="utf8").read()
                            fp0 = open(fpkg0, encoding="utf8").read()
                            fp1 = open(fpkg1, encoding="utf8").read()
                            tmp = []
                            for word in enumerate(fd0.split(" ")):
                                if word[1] != '':
                                    tmp.append(float(word[1]))
                            dram0.append(tmp)
                            tmp = []
                            for word in enumerate(fd1.split(" ")):
                                if word[1] != '':
                                    tmp.append(float(word[1]))
                            dram1.append(tmp)
                            tmp = []
                            for word in enumerate(fp0.split(" ")):
                                if word[1] != '':
                                    tmp.append(float(word[1]))
                            pkg0.append(tmp)
                            tmp = []
                            for word in enumerate(fp1.split(" ")):
                                if word[1] != '':
                                    tmp.append(float(word[1]))
                            pkg1.append(tmp)
                        if os.path.exists(file):
                            dram0 = average(dram0)
                            dram1 = average(dram1)
                            pkg0 = average(pkg0)
                            pkg1 = average(pkg1)
                            dram0 = compare(dram0)
                            dram1 = compare(dram1)
                            pkg0  = compare(pkg0)
                            pkg1  = compare(pkg1)
                            X_axis = np.arange(len(lab) - 1)
                            plt.xlabel('Combinations')
                            plt.ylabel('Gain (%)')
                            plt.plot(dram0, label="Dram 0")
                            plt.plot(dram1, label="Dram 1")
                            plt.plot(pkg0, label="Pkg 0")
                            plt.plot(pkg1, label="Pkg 0")
                            plt.ylim([-10, 10])
                            plt.xticks(ind2, lab[:-1])
                            plt.legend(loc='upper right', fancybox=True, shadow=True)
                            plt.tight_layout()
                            plt.subplots_adjust(top=0.85)
                            title = "Energy gain\n" + str(int(thread)) + " cores - Matrix = " + matrix + " - Tile = " + tile + " - Frequency = " + freq
                            plt.title(title)
                            plt.grid()
                            plt.show()
