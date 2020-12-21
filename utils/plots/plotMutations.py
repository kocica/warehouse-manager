import matplotlib.pyplot as plt

fig, ax = plt.subplots()

inverse = [109494, 109081, 108986, 108986, 108280, 108280, 108083, 108083, 107859, 107547, 107536, 107228, 107228, 107228, 106855, 106855, 106855, 106855, 106289, 106289, 106289, 106046, 106046, 105978, 105978, 105978, 105978, 105384, 105384, 104794, 104794, 104794, 104794, 104794, 104794, 104538, 104538, 104374, 104374, 104333, 104333, 104235, 104223, 104223, 104223, 103806, 103806, 103806, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103479, 103080, 102404, 102404, 102404, 102404, 102404, 102404, 102404, 102404, 102404, 102169, 102169, 101907, 101907, 101907, 101907, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101185, 101114, 101114, 100945, 100945, 100819, 100670, 100670, 100037, 100037, 100037, 100037, 100037, 100037, 99815, 99815, 99814, 99767, 99609, 99609, 99609, 99609, 99566, 99489, 99471, 99421, 99421, 99361, 99134, 99134, 99134, 99134, 99134, 98901, 98901, 98901, 98886, 98840, 98840, 98840, 98837, 98824, 98810, 98810, 98810, 98792, 98792, 98792, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98779, 98761, 98761, 98734, 98700, 98700, 98616, 98616, 98616, 98589, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98445, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98425, 98294, 98294, 98294, 98294, 98294, 98231, 98231, 98231, 98231, 97820, 97820, 97820, 97820, 97820, 97820, 97806, 97806, 97656, 97656, 97573, 97554, 97452, 97452, 97387, 97385, 97318, 97286, 97217, 97217, 97166, 97166, 97166, 97019, 96912, 96912, 96912, 96912, 96875, 96800, 96800, 96800, 96797, 96797, 96797, 96797, 96797, 96797, 96797, 96647, 96647, 96647, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591, 96591]
inverse_r = range(0, 345)
ax.plot(inverse_r, inverse, label='Inverse', color='blue')

ordered = [109247, 106531, 106131, 105534, 105534, 105534, 105351, 105060, 105060, 105060, 104968, 104857, 104857, 104799, 104596, 104492, 104353, 104268, 104268, 104268, 104114, 103946, 103946, 103946, 102886, 102886, 102737, 102737, 102737, 102737, 102737, 102737, 102737, 102737, 102737, 102270, 102270, 102270, 102270, 102221, 102221, 102221, 102221, 102221, 102221, 101745, 101745, 101745, 101745, 100844, 100844, 100844, 100844, 100844, 100844, 100844, 100844, 100399, 100283, 100083, 100083, 100083, 100083, 100083, 99936, 99839, 99776, 99400, 99400, 99400, 99400, 99391, 99391, 99391, 99153, 99153, 98941, 98941, 98909, 98841, 98811, 98553, 98553, 98553, 98553, 98553, 98508, 98450, 98373, 98319, 98265, 98241, 98241, 98143, 97964, 97880, 97815, 97815, 97763, 97711, 97711, 97691, 97588, 97588, 97588, 97580, 97558, 97435, 97435, 97401, 97391, 97391, 97391, 97390, 97390, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97384, 97383, 97362, 97356, 97346, 97341, 97341, 97325, 97324, 97324, 97324, 97324, 97318, 97318, 97318, 97318, 97318, 97318, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97316, 97280, 97243, 97243, 97243, 97243, 97243, 97186, 97186, 97186, 97125, 97101, 97085, 97047, 97047, 97035, 96966, 96966, 96941, 96928, 96881, 96881, 96881, 96844, 96844, 96844, 96786, 96786, 96786, 96580, 96580, 96580, 96552, 96552, 96539, 96523, 96460, 96460, 96460, 96442, 96442, 96409, 96374, 96374, 96371, 96371]
ordered_r = range(0, 345)
ax.plot(ordered_r, ordered, label='Ordered', color='orange')

legend = ax.legend(loc='upper right', fontsize='large')

plt.xlabel('Iterace')
plt.ylabel('Čas [s]')

plt.title('')

plt.savefig('plotComparisonMutations.png')
plt.show()