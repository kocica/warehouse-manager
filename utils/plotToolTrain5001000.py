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

ga = [109564, 106395, 105964, 105584, 105370, 105335, 105135, 105094, 104865, 104199, 104199, 104199, 104199, 104199, 104199, 103909, 103909, 103909, 103909, 103909, 103169, 102972, 102972, 102972, 102972, 102972, 102941, 102941, 102941, 102941, 102941, 102877, 102877, 102877, 102877, 102272, 101728, 101728, 101728, 101728, 101728, 101728, 101728, 101728, 101728, 101694, 101694, 101575, 101575, 101575, 101549, 101549, 101455, 101418, 101288, 101019, 101019, 101019, 100812, 100772, 100772, 100699, 100699, 100699, 100699, 100699, 99922, 99922, 99922, 99884, 99543, 99489, 99489, 99483, 99483, 99483, 99483, 99483, 99483, 99483, 99483, 99483, 99483, 99271, 99200, 99200, 99200, 99200, 99171, 99171, 98414, 98414, 98414, 98113, 97931, 97749, 97744, 97744, 97744, 97593, 97593, 97593, 97593, 97255, 96980, 96980, 96980, 96616, 96616, 96616, 96462, 96462, 96462, 96462, 96462, 96462, 96462, 96462, 96462, 96462, 96462, 96462, 96462, 96462, 96462, 96462, 96462, 96462, 95829, 95829, 95829, 95829, 95829, 95829, 95829, 95829, 95829, 95829, 95829, 95826, 95826, 95826, 95826, 95826, 95826, 95826, 95826, 95826, 95600, 95599, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94803, 94637, 94637, 94457, 94457, 94457, 94457, 94457, 94299, 94299, 94299, 94059, 94059, 93783, 93783, 93783, 93783, 93451, 93451, 93451, 93451, 93451, 93451, 93451, 93451, 93451, 93451, 93451, 93451, 93451, 93451, 93418, 93418, 93418, 93418, 93418, 93418, 93342, 93342, 93342, 93342, 93342, 93342, 93328, 93319, 93136, 93136, 93136, 93136, 93129, 92863, 92863, 92863, 92863, 92863, 92863, 92863, 92863, 92863, 92804, 92709, 92709, 92602, 92602, 92498, 92403, 92403, 92403, 92403, 92335, 92335, 92296, 92296, 92296, 92296, 92246, 92246, 92246, 92246, 92196, 92196, 92196, 92196, 92196, 92180, 92160, 92160, 92085, 92059, 92058, 92043, 91908, 91908, 91908, 91908, 91908, 91908, 91908, 91908, 91908, 91907, 91907, 91907, 91907, 91907, 91907, 91907, 91907, 91905, 91903, 91903, 91903, 91903, 91903, 91903, 91903, 91903, 91902, 91902, 91902, 91902, 91902, 91902, 91902, 91902, 91902, 91902, 91902, 91902, 91902, 91901, 91848, 91848, 91399, 91230, 91230, 91230, 91230, 91195, 91068, 91068, 90825, 90825, 90825, 90825, 90609, 90609, 90609, 90609, 90257, 90257, 90257, 90257, 90257, 90257, 90257, 90088, 90088, 90088, 90088, 90088, 90088, 90073, 90073, 90054, 90043, 90020, 90020, 90020, 90018, 89987, 89987, 89980, 89980, 89972, 89972, 89972, 89967, 89967, 89967, 89967, 89967, 89967, 89967, 89961, 89961, 89961, 89961, 89961, 89961, 89961, 89961, 89961, 89955, 89954, 89954, 89954, 89954, 89954, 89954, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89951, 89915, 89889, 89889, 89889, 89856, 89842, 89842, 89842, 89776, 89776, 89776, 89763, 89763, 89763, 89763, 89763, 89763, 89763, 89755, 89755, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89753, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89747, 89721, 89721, 89709, 89701, 89701, 89701, 89701, 89696, 89696, 89684, 89676, 89676, 89671, 89671, 89663, 89655, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89642, 89528, 89528, 89528, 89525, 89522, 89459, 89459, 89455, 89418, 89418, 89412, 89412, 89412, 89392, 89381, 89376, 89376, 89376, 89376, 89376, 89376, 89376, 89376, 89376, 89376, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89375, 89087, 89087, 89087, 89087, 89085, 89084, 89000, 89000, 89000, 89000, 88980, 88880, 88880, 88880, 88844, 88844, 88836, 88755, 88755, 88747, 88681, 88629, 88629, 88629, 88629, 88629, 88629, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88625, 88622, 88622, 88622, 88622, 88622, 88621, 88621, 88595, 88586, 88561, 88561, 88561, 88520, 88488, 88488, 88476, 88476, 88476, 88476, 88472, 88460, 88460, 88458, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88447, 88426, 88425, 88425, 88425, 88369, 88365, 88356, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336, 88336]
ga_r = range(0, 1000)
ax.plot(ga_r, ga, label='GA')

legend = ax.legend(loc='upper right', fontsize='large')

plt.xlabel('Iterace')
plt.ylabel('Čas [s]')

plt.title('')

plt.savefig('plotTrain5001000.png')
plt.show()
