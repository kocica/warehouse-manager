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

no_params = [109494, 109081, 108986, 108986, 108280, 108280, 108083, 108083, 107859, 107547, 107536, 107228, 107228, 107228, 106855, 106855, 106855, 106855, 106289, 106289, 106289, 106046, 106046, 105978, 105978, 105978, 105978, 105384, 105384, 104794, 104794, 104794, 104794, 104794, 104794, 104538, 104538, 104374, 104374, 104333, 104333, 104235, 104223, 104223, 104223, 103806, 103806, 103806, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103080, 102404, 102404, 102404, 102404, 102404, 102404, 102404, 102404, 102404, 102169, 102169, 101907, 101907, 101907, 101907, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101114, 101114, 100945, 100945, 100819, 100670, 100670, 100037, 100037, 100037, 100037, 100037, 100037, 99815, 99815, 99814, 99767, 99609, 99609, 99609, 99609, 99566, 99489, 99471, 99421, 99421, 99361, 99134, 99134, 99134, 99134, 99134, 98901, 98901, 98901, 98886, 98840, 98840, 98840, 98837, 98824, 98810, 98810, 98810, 98792, 98792, 98792, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98761, 98761, 98734, 98700, 98700, 98616, 98616, 98616, 98589, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98294, 98294, 98294, 98294, 98294, 98231, 98231, 98231, 98231, 97820, 97820, 97820, 97820, 97820, 97820, 97806, 97806, 97656, 97656, 97573, 97554, 97452, 97452, 97387, 97385, 97318, 97286, 97217, 97217, 97166, 97166, 97166, 97019, 96912, 96912, 96912, 96912, 96875, 96800, 96800, 96800, 96797, 96797, 96797, 96797, 96797, 96797, 96797, 96647, 96647, 96647, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591]
no_params_r = range(0, 400)
ax.plot(no_params_r, no_params, label='Probabilities 0.6, 0.4, 0.2')

params_1 = [106438, 106105, 105789, 105789, 105612, 105495, 104754, 104754, 104754, 104646, 104455, 104246, 104246, 104246, 103928, 103928, 103928, 103906, 103510, 103247, 103247, 103247, 103247, 102971, 102971, 102784, 102784, 102784, 102425, 102425, 102425, 102338, 101970, 101970, 101970, 101869, 101557, 101116, 101116, 101116, 101116, 101116, 101116, 100551, 100551, 100551, 100551, 100417, 100417, 99995, 99799, 99799, 99799, 99799, 99799, 99799, 99799, 99799, 99704, 99704, 99334, 99334, 99307, 99307, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98252, 98171, 98171, 98171, 98171, 98171, 97716, 97716, 97716, 97716, 97716, 97716, 97716, 97716, 97716, 97716, 97407, 97387, 97387, 97271, 97080, 96642, 96642, 96642, 96642, 96642, 96423, 96423, 96423, 96423, 96423, 96234, 96234, 96234, 96234, 96234, 96234, 96234, 96234, 96234, 96234, 96191, 96063, 96063, 96063, 96063, 96063, 96063, 96063, 96063, 96063, 96063, 95868, 95812, 95568, 95568, 95568, 95568, 95568, 95383, 95383, 95383, 95132, 95132, 95132, 95132, 95132, 95132, 95132, 95132, 95132, 94460, 94460, 94293, 94293, 94293, 94293, 94293, 94293, 94293, 94293, 94221, 94154, 94145, 94029, 93606, 93606, 93606, 93497, 93497, 93295, 93295, 93295, 93293, 93293, 93190, 93190, 93190, 93099, 93099, 92891, 92891, 92891, 92605, 92605, 92605, 92605, 92605, 92605, 92605, 92579, 92579, 92278, 92278, 92136, 92136, 92136, 92136, 92136, 92136, 92136, 91900, 91900, 91900, 91900, 91861, 91861, 91861, 91761, 91761, 91752, 91617, 91617, 91582, 91582, 91582, 91527, 91358, 91352, 91352, 91352, 91309, 91208, 91208, 91101, 90904, 90904, 90854, 90854, 90787, 90764, 90764, 90764, 90746, 90700, 90675, 90650, 90643, 90633, 90627, 90574, 90556, 90540, 90540, 90540, 90536, 90523, 90523, 90523, 90521, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90518, 90517, 90509, 90509, 90500, 90492, 90162, 90162, 90125, 90125, 90125, 90121, 90121, 90115, 90114, 90112, 90112, 90090, 90090, 90090, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90082, 90046, 90046, 90046, 90046, 90046, 90046, 90046, 90031, 89965, 89960, 89947, 89940, 89940, 89938, 89935, 89935]
params_1_r = range(0, 400)
ax.plot(params_1_r, params_1, label='Probabilities 0.6, 0.5, 0.3')

params_2 = [106536, 105991, 105931, 105781, 105670, 105553, 103954, 103954, 103954, 103954, 103954, 103954, 103954, 103954, 103954, 103954, 103845, 103845, 103658, 103228, 103228, 103228, 103144, 103083, 103083, 102717, 102717, 102612, 102612, 102612, 102612, 102612, 102612, 102537, 102537, 102537, 101946, 101946, 101946, 101946, 101946, 101946, 101946, 101946, 101946, 101695, 101695, 101695, 100803, 100803, 100517, 100517, 100517, 100517, 100517, 100487, 100487, 100487, 100433, 100433, 100433, 100433, 100433, 100179, 99494, 99494, 99494, 99310, 99310, 99310, 99310, 99310, 99310, 99310, 99310, 99310, 99310, 98766, 98766, 98766, 98766, 98766, 98467, 98467, 98467, 98467, 98467, 98467, 98467, 98467, 98467, 98467, 98467, 98042, 98042, 98042, 98042, 98042, 98042, 97356, 97356, 97356, 97356, 97356, 97356, 97356, 97356, 97356, 97356, 96878, 96878, 96878, 96878, 96878, 96878, 96787, 96787, 96787, 96256, 96256, 96256, 96256, 96256, 96256, 96256, 96256, 96256, 96256, 96256, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95556, 95317, 95317, 95317, 95317, 95317, 95317, 95317, 95219, 95219, 95203, 95146, 94950, 94826, 94560, 94560, 94560, 94560, 94560, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002, 94002]
params_2_r = range(0, 400)
ax.plot(params_2_r, params_2, label='Probabilities 0.7, 0.6, 0.4')

legend = ax.legend(loc='upper right', fontsize='large')

plt.xlabel('Iterace')
plt.ylabel('Čas [s]')

plt.title('')

plt.savefig('plotComparisonParams.png')
plt.show()
