import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

algs = (r"Bez optimalizace", r"Cesta" + "\n" +"(MMAS, 66.11%) ", r"Rozložení" + "\n" + "(GA, 67.26%)", r"Kombinace" + "\n" + "(MMAS+GA, 54.8%)")

alg_ids = np.arange(len(algs))
performance = [15771, 10427, 10609, 8644]

barlist = plt.bar(alg_ids, performance, align='center')
plt.xticks(alg_ids, algs)

barlist[0].set_color('blue')
barlist[1].set_color('purple')
barlist[2].set_color('black')
barlist[3].set_color('orange')

plt.xlabel('')
plt.ylabel('Čas [s]')

plt.title('')

plt.savefig('plotOptimizersComparison7501000.png')
plt.show()