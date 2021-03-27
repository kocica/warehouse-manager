import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

plt.style.use('seaborn')

plt.rcParams['font.serif'] = 'Ubuntu'
plt.rcParams['font.monospace'] = 'Ubuntu Mono'
plt.rcParams['axes.labelweight'] = 'bold'
plt.rc('xtick', labelsize=13)
plt.rc('ytick', labelsize=13)
plt.rc('legend', fontsize=15)

plt.subplots_adjust(right=1, top=1, wspace=0, hspace=0)

algs = ('GA', 'ABC', 'DE', "Battista" + "\n" + "a spol", 'PSO', 'RAND', "Před" + "\n" + "optimalizací")

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

plt.savefig('plotComparisonTest.pdf', format="pdf")
plt.show()