import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

plt.style.use('seaborn')

plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = 'Ubuntu'
plt.rcParams['font.monospace'] = 'Ubuntu Mono'
plt.rcParams['font.size'] = 10
plt.rcParams['axes.labelsize'] = 10
plt.rcParams['axes.labelweight'] = 'bold'
plt.rcParams['axes.titlesize'] = 10
plt.rcParams['xtick.labelsize'] = 8
plt.rcParams['ytick.labelsize'] = 8
plt.rcParams['legend.fontsize'] = 10
plt.rcParams['figure.titlesize'] = 12

algs = ('GA', 'ABC', 'DE', 'Battista a spol', 'PSO', 'RAND', 'Před optimalizací')

alg_ids = np.arange(len(algs))
performance = [9247, 11060, 11264, 11749, 12076, 16903, 18000]

barlist = plt.bar(alg_ids, performance, align='center')
plt.xticks(alg_ids, algs)

barlist[0].set_color('tab:blue')
barlist[1].set_color('tab:green')
barlist[2].set_color('tab:red')
barlist[3].set_color('purple')
barlist[4].set_color('black')
barlist[5].set_color('orange')
barlist[6].set_color('grey')

plt.ylabel('Čas [s]')

plt.title('')

plt.savefig('plotComparisonTest.png')
plt.show()