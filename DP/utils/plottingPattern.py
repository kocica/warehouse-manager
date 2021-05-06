#
# Filip Kočica
# Pattern script for plotting results (to be filled with optimization results)
#

import matplotlib.pyplot as plt

plt.style.use('seaborn')

plt.rcParams['font.serif'] = 'Ubuntu'
plt.rcParams['font.monospace'] = 'Ubuntu Mono'
plt.rcParams['axes.labelweight'] = 'bold'
plt.rc('xtick', labelsize=13)
plt.rc('ytick', labelsize=13)
plt.rc('legend', fontsize=15)

fig, ax = plt.subplots()
plt.subplots_adjust(right=1, top=1, wspace=0, hspace=0)

plot_1 = [] # Fill with data
plot_1_r = range(0, ) # Fill with range - dimension
ax.plot(plot_1_r, plot_1, label='') # Fill label

legend = ax.legend(loc='upper right')

plt.xlabel('') # Fill x label
plt.ylabel('') # Fill y label

plt.title('')  # Fill title

plt.savefig('.pdf', format="pdf") # Fill name and possibly change data-format
plt.show()
