import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

algs = ('GA', 'ABC', 'DE', 'SLAP', 'PSO', 'RAND', 'Puvodni')

alg_ids = np.arange(len(algs))
performance = [9247, 11060, 11264, 11749, 12076, 16903, 18000]

barlist = plt.bar(alg_ids, performance, align='center')
plt.xticks(alg_ids, algs)

barlist[0].set_color('blue')
barlist[1].set_color('green')
barlist[2].set_color('red')
barlist[3].set_color('purple')
barlist[4].set_color('black')
barlist[5].set_color('orange')
barlist[6].set_color('grey')

plt.xlabel('Algoritmus')
plt.ylabel('Čas [s]')

plt.title('')

plt.savefig('plotComparisonTest.png')
plt.show()