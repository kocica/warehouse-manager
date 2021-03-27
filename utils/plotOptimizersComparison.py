import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

plt.style.use('seaborn')

plt.rcParams['font.serif'] = 'Ubuntu'
plt.rcParams['font.monospace'] = 'Ubuntu Mono'
plt.rcParams['axes.labelweight'] = 'bold'

plt.subplots_adjust(right=1, top=1, wspace=0, hspace=0)

algs = (r"Bez optimalizace", r"Optimalizace cesty" + "\n" +"(MMAS, 59.4%) ", r"Optimalizace rozložení" + "\n" + "(GA, 57.0%)", r"Kombinace Optimalizací" + "\n" + "(MMAS+GA, 36.9%)")

alg_ids = np.arange(len(algs))
performance = [10126, 6017, 5772, 3739]

barlist = plt.bar(alg_ids, performance, align='center')
plt.xticks(alg_ids, algs)

barlist[0].set_color('tab:blue')
barlist[1].set_color('purple')
barlist[2].set_color('black')
barlist[3].set_color('orange')

plt.xlabel('')
plt.ylabel('Čas [s]')

plt.title('')

plt.savefig('plotOptimizersComparison.png')
plt.show()