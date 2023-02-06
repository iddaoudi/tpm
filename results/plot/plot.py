import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os.path
import csv

test = 1
plot = 0

def moyenne(a):
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

if test == 1:
    machines = ["skylake"]
    matrix   = ["24576"]
    tile     = ["1024"]
    threads  = ["48"]
    frequency = ["min"]
    algorithms = ["cholesky", "qr", "lu", "sparselu"]
else:
    machines = ["cascadelake", "skylake", "zen"]
    matrix   = ["128", "2048", "4096", "8192", "16384", "24576"]
    tile     = ["64", "256", "512", "1024"]
    threads  = ["32", "64", "24", "48"]
    frequency = ["mid", "min"]
    algorithms = ["cholesky", "qr", "lu", "sparselu"]

labels = ["c1","c2","c3","c4","c5","c6","c7","c8","c9","c10","c11","c12","c13","c14","c15","c16"]
labelslu = ["c1","c2","c3","c4","c5","c6","c7","c8"]

delim = "_"
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
        for msize in matrix:
            for bsize in tile:
                for thread in threads:
                    for freq in frequency:
                        dram0 = []
                        dram1 = []
                        pkg0 = []
                        pkg1 = []
                        for iteration in range(11): #FIXME
                            file = "../" + machine + delim + algo + delim + msize + delim + bsize + delim + thread + delim + freq + delim + str(iteration)
                            if os.path.exists(file):
                                fdram0 = file + "/dram_0.dat"
                                fdram1 = file + "/dram_1.dat"
                                fpkg0 = file + "/pkg_0.dat"
                                fpkg1 = file + "/pkg_1.dat"
                                fd0 = open(fdram0, encoding="utf8").read()
                                fd1 = open(fdram1, encoding="utf8").read()
                                fp0 = open(fpkg0, encoding="utf8").read()
                                fp1 = open(fpkg1, encoding="utf8").read()
                                tpm = []
                                for word in enumerate(fd0.split(" ")):
                                    if word[1] != '':
                                        tpm.append(float(word[1]))
                                dram0.append(tpm)
                                tpm = []
                                for word in enumerate(fd1.split(" ")):
                                    if word[1] != '':
                                        tpm.append(float(word[1]))
                                dram1.append(tpm)
                                tpm = []
                                for word in enumerate(fp0.split(" ")):
                                    if word[1] != '':
                                        tpm.append(float(word[1]))
                                pkg0.append(tpm)
                                tpm = []
                                for word in enumerate(fp1.split(" ")):
                                    if word[1] != '':
                                        tpm.append(float(word[1]))
                                pkg1.append(tpm)
                        if os.path.exists(file):
                            dram0 = moyenne(dram0)
                            dram1 = moyenne(dram1)
                            pkg0 = moyenne(pkg0)
                            pkg1 = moyenne(pkg1)
                            if plot == 1:
                                X_axis = np.arange(len(lab))
                                plt.figure()
                                plt.bar(ind - width  , pkg0, width, label="Pkg 0")
                                plt.bar(ind          , pkg1, width, label="Pkg 1")
                                plt.bar(ind + width  , dram0, width, label="Dram 0")
                                plt.bar(ind + 2*width, dram1, width, label="Dram 1")
                                plt.xticks(ind + width/2, lab)
                                #plt.ylim([0, 2.5e6])
                                title = "Energy consumption comparison - " + str(int(thread)) + " cores\n Matrix = " + msize + " - Tile = " + bsize + "\nFrequency = " + freq
                                plt.title(title)
                                plt.xlabel('Combinations')
                                plt.ylabel('Energy (uJ)')
                                plt.legend(bbox_to_anchor=(1.05, 1.0), loc='upper left', fancybox=True, shadow=True)
                                plt.tight_layout()
                                fig_name = machine + "_" + algo + "_" + msize + "_" + thread + "_" + str(int(thread)+1) + "_" + freq + ".png"
                                plt.show()
                            else:
                                #lab.pop(len(lab) - 1)
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
                                title = "Energy gain compared to the default case\n" + str(int(thread)) + " cores - Matrix = " + msize + " - Tile = " + bsize + " - Frequency = " + freq
                                plt.title(title)
                                plt.grid()
                                plt.show()



