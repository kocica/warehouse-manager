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

no_trial = [108973, 106345, 106170, 105799, 105799, 105633, 104748, 104748, 104748, 104748, 104595, 103764, 103764, 103764, 103764, 103461, 103461, 103323, 103323, 103111, 103109, 103109, 103109, 102966, 102966, 102966, 102966, 102936, 102836, 102836, 102657, 102657, 102657, 102657, 102657, 102506, 102506, 102506, 102392, 102392, 102277, 102277, 102277, 102149, 102148, 102090, 101898, 101898, 101898, 101898, 101801, 101801, 101801, 101795, 101733, 101733, 101733, 101651, 101651, 101606, 101606, 101606, 101560, 101560, 101448, 101448, 101448, 101448, 101346, 101346, 101346, 101346, 101317, 101317, 101317, 101275, 101275, 101275, 101252, 101230, 101230, 101230, 101230, 101230, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214, 101214]
no_trial_r = range(0, 700)
ax.plot(no_trial_r, no_trial, label='No trial')

trial_10 = [109494, 109081, 108986, 108986, 108280, 108280, 108083, 108083, 107859, 107547, 107536, 107228, 107228, 107228, 106855, 106855, 106855, 106855, 106289, 106289, 106289, 106046, 106046, 105978, 105978, 105978, 105978, 105384, 105384, 104794, 104794, 104794, 104794, 104794, 104794, 104538, 104538, 104374, 104374, 104333, 104333, 104235, 104223, 104223, 104223, 103806, 103806, 103806, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103080, 102404, 102404, 102404, 102404, 102404, 102404, 102404, 102404, 102404, 102169, 102169, 101907, 101907, 101907, 101907, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101114, 101114, 100945, 100945, 100819, 100670, 100670, 100037, 100037, 100037, 100037, 100037, 100037, 99815, 99815, 99814, 99767, 99609, 99609, 99609, 99609, 99566, 99489, 99471, 99421, 99421, 99361, 99134, 99134, 99134, 99134, 99134, 98901, 98901, 98901, 98886, 98840, 98840, 98840, 98837, 98824, 98810, 98810, 98810, 98792, 98792, 98792, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98761, 98761, 98734, 98700, 98700, 98616, 98616, 98616, 98589, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98294, 98294, 98294, 98294, 98294, 98231, 98231, 98231, 98231, 97820, 97820, 97820, 97820, 97820, 97820, 97806, 97806, 97656, 97656, 97573, 97554, 97452, 97452, 97387, 97385, 97318, 97286, 97217, 97217, 97166, 97166, 97166, 97019, 96912, 96912, 96912, 96912, 96875, 96800, 96800, 96800, 96797, 96797, 96797, 96797, 96797, 96797, 96797, 96647, 96647, 96647, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96576, 96461, 96410, 96399, 96399, 96399, 96384, 96384, 96277, 96277, 96222, 96222, 96222, 96145, 96145, 96145, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104, 96104]
trial_10_r = range(0, 700)
ax.plot(trial_10_r, trial_10, label='Trial = 10')

trial_30 = [109207, 105899, 105899, 105613, 104863, 104863, 104706, 104706, 104652, 104652, 104569, 104325, 104325, 104073, 103779, 103779, 103477, 103477, 103477, 103477, 102843, 102843, 102843, 102843, 102843, 102734, 102464, 102437, 102437, 102232, 102232, 102232, 102232, 102046, 101878, 101878, 101401, 101401, 101399, 101399, 101360, 101265, 101265, 101076, 101076, 100778, 100778, 100717, 100681, 100681, 100681, 100573, 100527, 100527, 100331, 100178, 100178, 100178, 100159, 100159, 100101, 100101, 100101, 100088, 100088, 100088, 100034, 99761, 99761, 99761, 99761, 99699, 99699, 99699, 99699, 99698, 99664, 99660, 99660, 99654, 99654, 99654, 99654, 99654, 99654, 99654, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99653, 99604, 99604, 99604, 99503, 99503, 99503, 99503, 99495, 99417, 99407, 99407, 99347, 99347, 99347, 99347, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99339, 99336, 99336, 99336, 99324, 99308, 99308, 99277, 99159, 99119, 99119, 99000, 99000, 99000, 99000, 99000, 99000, 98998, 98981, 98981, 98954, 98954, 98951, 98951, 98951, 98917, 98882, 98841, 98841, 98808, 98796, 98796, 98792, 98792, 98792, 98790, 98751, 98751, 98751, 98733, 98733, 98704, 98704, 98704, 98704, 98702, 98702, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98701, 98565, 98565, 98562, 98562, 98257, 98068, 98068, 98068, 97969, 97969, 97969, 97956, 97834, 97834, 97834, 97766, 97726, 97635, 97490, 97403, 97403, 97361, 97313, 97222, 97222, 97152, 97111, 97098, 97098, 97056, 96993, 96979, 96973, 96917, 96897, 96897, 96886, 96870, 96863, 96840, 96840, 96840, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96833, 96707, 96707, 96707, 96707, 96707, 96707, 96656, 96656, 96570, 96561, 96535, 96489, 96451, 96451, 96417, 96417, 96316, 96316, 96316, 96316, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96309, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294, 96294]
trial_30_r = range(0, 700)
ax.plot(trial_30_r, trial_30, label='Trial = 30')

legend = ax.legend(loc='upper right')

plt.xlabel('Iterace')
plt.ylabel('Čas [s]')

plt.title('')

plt.savefig('plotComparisonTrials.pdf', format="pdf")
plt.show()
