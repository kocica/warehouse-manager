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

fig, ax = plt.subplots()

tournam = [109494, 109081, 108986, 108986, 108280, 108280, 108083, 108083, 107859, 107547, 107536, 107228, 107228, 107228, 106855, 106855, 106855, 106855, 106289, 106289, 106289, 106046, 106046, 105978, 105978, 105978, 105978, 105384, 105384, 104794, 104794, 104794, 104794, 104794, 104794, 104538, 104538, 104374, 104374, 104333, 104333, 104235, 104223, 104223, 104223, 103806, 103806, 103806, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103080, 102404, 102404, 102404, 102404, 102404, 102404, 102404, 102404, 102404, 102169, 102169, 101907, 101907, 101907, 101907, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101114, 101114, 100945, 100945, 100819, 100670, 100670, 100037, 100037, 100037, 100037, 100037, 100037, 99815, 99815, 99814, 99767, 99609, 99609, 99609, 99609, 99566, 99489, 99471, 99421, 99421, 99361, 99134, 99134, 99134, 99134, 99134, 98901, 98901, 98901, 98886, 98840, 98840, 98840, 98837, 98824, 98810, 98810, 98810, 98792, 98792, 98792, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98761, 98761, 98734, 98700, 98700, 98616, 98616, 98616, 98589, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425]
tournam_r = range(0, 250)
ax.plot(tournam_r, tournam, label='Tournament (k = 5)')

trunc = [109644, 108654, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 106439, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105712, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408, 105408]
trunc_r = range(0, 250)
ax.plot(trunc_r, trunc, label='Truncate')

roulette = [109842, 106604, 106427, 106414, 106414, 106414, 106240, 106240, 105910, 105910, 105910, 105910, 105910, 105910, 105910, 105910, 105910, 105540, 105540, 105540, 105446, 105219, 105219, 105219, 105219, 105219, 105219, 105219, 105219, 105203, 105203, 105203, 105203, 105203, 105203, 105203, 105203, 105203, 105203, 105203, 105203, 105203, 105203, 104987, 104987, 104987, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104916, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104555, 104471, 104471, 104471, 104471, 104471, 104471, 104471, 104471, 104471, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430, 104430]
roulette_r = range(0, 250)
ax.plot(roulette_r, roulette, label='Roulette')

rank = [108945, 107657, 106163, 106160, 106160, 106160, 106160, 106160, 105889, 105692, 105065, 105065, 105065, 105065, 105065, 105065, 105065, 105065, 105065, 105065, 105065, 105065, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610, 104610]
rank_r = range(0, 250)
ax.plot(rank_r, rank, label='Rank')

legend = ax.legend(loc='upper right', fontsize='large')

plt.xlabel('Iterace')
plt.ylabel('Čas [s]')

plt.title('')

plt.savefig('plotComparisonSelections.png')
plt.show()
