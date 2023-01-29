import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os.path
import csv

def moyenne(mlist):
    tmp = []
    for x in zip(mlist[1][0],mlist[2][0],mlist[3][0],mlist[4][0],mlist[5][0]):
        tmp.append(sum(x))
    print(tmp)
    for i in range(len(tmp)):
        tmp[i] = tmp[i]/5
    return tmp

labels = ["c1","c2","c3","c4","c5","c6","c7","c8","c9","c10","c11","c12","c13","c14","c15","c16"]
labelslu = ["c1","c2","c3","c4","c5","c6","c7","c8"]

machine = ["cascadelake", "skylake", "zen"]
matrix  = ["128", "2048", "4096", "8192", "16384"]
tile    = ["64", "256", "512", "1024"]
threads = ["31", "63", "15"]
frequency = ["mid", "min"]
algorithms = ["cholesky", "qr", "lu", "sparselu"]

bars = 0
width = 0.2

for mach in machine:
    for a in algorithms:
        for m in matrix:
            for t in tile:
                for th in threads:
                    for freq in frequency:
                        pkg0  = []
                        pkg1  = []
                        dram0 = []
                        dram1 = []
                        pkg0t  = []
                        pkg1t  = []
                        dram0t = []
                        dram1t = []
                        for it in range(1,6):
                            folder = "../" + mach + "_" + a + "_" + m + "_" + t + "_" + th + "_" + freq + "_" + str(it)
                            if a == "lu":
                                lab = labelslu
                            else:
                                lab = labels
                            bars = len(lab)
                            ind  = np.arange(bars)
                            if os.path.exists(folder):
                                pkg0t.append([])
                                pkg1t.append([])
                                dram0t.append([])
                                dram1t.append([])
                                fpkg0 = folder + "/pkg_0.dat"
                                fpkg1 = folder + "/pkg_1.dat"
                                fdram0 = folder + "/dram_0.dat"
                                fdram1 = folder + "/dram_1.dat"
                                if os.path.exists(fpkg0):
                                    fp0 = open(fpkg0, encoding="utf8").read()
                                    for word in enumerate(fp0.split(" ")):
                                        if word[1] != '':
                                            pkg0t[i].append(float(word[1]))
                                if os.path.exists(fpkg1):
                                    fp1 = open(fpkg1, encoding="utf8").read()
                                    for word in enumerate(fp1.split(" ")):
                                        if word[1] != '':
                                            pkg1t[i].append(float(word[1]))
                                if os.path.exists(fdram0):
                                    fd0 = open(fdram0, encoding="utf8").read()
                                    for word in enumerate(fd0.split(" ")):
                                        if word[1] != '':
                                            dram0t[i].append(float(word[1]))
                                if os.path.exists(fdram1):
                                    fd1 = open(fdram1, encoding="utf8").read()
                                    for word in enumerate(fd1.split(" ")):
                                        if word[1] != '':
                                            dram1t[i].append(float(word[1]))
                            pkg0.append(moyenne(pkg0t))
                            pkg1.append(moyenne(pkg1t))
                            dram0.append(moyenne(dram0t))
                            dram1.append(moyenne(dram1t))
                            X_axis = np.arange(len(lab))
                            plt.figure()
                            plt.bar(ind - width  , pkg0, width, label="Pkg 0")
                            plt.bar(ind          , pkg1, width, label="Pkg 1")
                            plt.bar(ind + width  , dram0, width, label="Dram 0")
                            plt.bar(ind + 2*width, dram1, width, label="Dram 1")
                            plt.xticks(ind + width/2, lab)
                            plt.ylim([0, 2.5e6])
                            title = "Energy consumption comparison - " + str(int(th)+1) + " cores\n Matrix = " + m + " - Tile = " + t + "\nFrequency = " + freq
                            plt.title(title)
                            plt.xlabel('Combinations')
                            plt.ylabel('Energy (uJ)')
                            legend = plt.legend(ncols=2, loc='best', fancybox=True, shadow=True)
                            fig_name = mach + "_" + a + "_" + m + "_" + t + "_" + str(int(th)+1) + "_" + freq + ".png"
                            plt.savefig(fig_name, bbox_inches='tight')
                            #plt.show()


