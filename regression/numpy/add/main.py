import numpy as np

a = np.add(1.0, 4.0)
assert a == 5.0

b = np.add(1, 2)
assert b == 3

c = np.add(127, 1, dtype=np.int8)
assert c == -128

d = np.add([1,2], [3,4])
assert d[0] == 4
assert d[1] == 6
