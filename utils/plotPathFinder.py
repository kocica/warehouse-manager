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

pf = [6842, 6820, 6820, 6820, 6402, 6402, 6402, 6402, 6402, 6402, 6402, 6402, 6402, 6166, 6166, 6166, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5788, 5558, 5558, 5558, 5558, 5558, 5558, 5558, 5558, 5558, 5558, 5558, 5558, 5558, 5558, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5536, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5354, 5314, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 5188, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4942, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4938, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886, 4886]
pf_r = range(0, 1000)
ax.plot(pf_r, pf, label='Pathfinder')

legend = ax.legend(loc='upper right')

plt.xlabel('Iterace')
plt.ylabel('Nejkratší cesta [m]')

plt.title('')

plt.savefig('plotPathFinder.pdf', format="pdf")
plt.show()
