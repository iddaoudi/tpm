import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os.path
import csv

labels = ["c1","c2","c3","c4","c5","c6","c7","c8","c9","c10","c11","c12","c13","c14","c15","c16"]

machine = "cl64"
matrix  = ["4096", "8192", "16384"]
tile    = ["512", "1024"]
threads = ["31", "63"]
frequency = ["mid", "min"]

bars = 16
ind  = np.arange(bars)
width = 0.2

for m in matrix:
    for t in tile:
        for th in threads:
            for freq in frequency:
                pkg0  = []
                pkg1  = []
                dram0 = []
                dram1 = []
                folder = "../" + machine + "_" + m + "_" + t + "_" + th + "_" + freq
                if os.path.exists(folder):
                    fpkg0 = folder + "/pkg_0.dat"
                    fpkg1 = folder + "/pkg_1.dat"
                    fdram0 = folder + "/dram_0.dat"
                    fdram1 = folder + "/dram_1.dat"
                    if os.path.exists(fpkg0):
                        fp0 = open(fpkg0, encoding="utf8").read()
                        for word in enumerate(fp0.split(" ")):
                            if word[1] != '':
                                pkg0.append(float(word[1]))
                    if os.path.exists(fpkg1):
                        fp1 = open(fpkg1, encoding="utf8").read()
                        for word in enumerate(fp1.split(" ")):
                            if word[1] != '':
                                pkg1.append(float(word[1]))
                    if os.path.exists(fdram0):
                        fd0 = open(fdram0, encoding="utf8").read()
                        for word in enumerate(fd0.split(" ")):
                            if word[1] != '':
                                dram0.append(float(word[1]))
                    if os.path.exists(fdram1):
                        fd1 = open(fdram1, encoding="utf8").read()
                        for word in enumerate(fd1.split(" ")):
                            if word[1] != '':
                                dram1.append(float(word[1]))
                    X_axis = np.arange(16)
                    plt.bar(ind - width  , pkg0, width, label="Pkg 0")
                    plt.bar(ind          , pkg1, width, label="Pkg 1")
                    plt.bar(ind + width  , dram0, width, label="Dram 0")
                    plt.bar(ind + 2*width, dram1, width, label="Dram 1")
                    plt.xticks(ind + width/2, labels)
                    title = "Energy consumption comparison - " + str(int(th)+1) + " cores\n Matrix = " + m + " - Tile = " + t + "\nFrequency = 2.00GHz"
                    plt.title(title)
                    plt.xlabel('Combinations')
                    plt.ylabel('Energy (uJ)')
                    plt.legend()
                    fig_name = "cl64_mid_" + m + "_" + t + "_" + str(int(th)+1) + ".png"
                    plt.savefig(fig_name)
                    plt.show()




