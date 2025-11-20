from typing import *
from nagini_contracts.contracts import *
from nagini_contracts.obligations import MustTerminate

def bitcount(n: int) -> int:
    # Pre/post conditions (you can refine later)
    Requires(n >= 0)
    Ensures(Result() >= 0)
    # Optionally: Ensures(Result() <= n.bit_length())

    count: int = 0
    while n != 0:
        Invariant(count >= 0)
        #Invariant(MustTerminate(n.bit_length() + 1))
        n &= n - 1
        count += 1
    return count


# assert bitcount(127) == 7
def test_bitcount_127() -> None:
    res = bitcount(127)
    Assert(res == 7)

# assert bitcount(128) == 1
def test_bitcount_128() -> None:
    res = bitcount(128)
    Assert(res == 1)
