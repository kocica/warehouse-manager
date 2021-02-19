import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

algs = (r"Bez optimalizace", r"Cesta" + "\n" +"(MMAS, 59.4%) ", r"Rozložení" + "\n" + "(GA, 57.0%)", r"Kombinace" + "\n" + "(MMAS+GA, 36.9%)")

alg_ids = np.arange(len(algs))
performance = [10126, 6017, 5772, 3739]

barlist = plt.bar(alg_ids, performance, align='center')
plt.xticks(alg_ids, algs)

barlist[0].set_color('blue')
barlist[1].set_color('purple')
barlist[2].set_color('black')
barlist[3].set_color('orange')

plt.xlabel('')
plt.ylabel('Čas [s]')

plt.title('')

plt.savefig('plotOptimizersComparison.png')
plt.show()