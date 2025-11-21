
# def lcs_length(s, t):
#     from collections import Counter

#     dp = Counter()

#     for i in range(len(s)):
#         for j in range(len(t)):
#             if s[i] == t[j]:
#                 dp[i, j] = dp[i - 1, j - 1] + 1

#     return max(dp.values()) if dp else 0

# assert lcs_length("witch", "sandwich") == 2
# assert lcs_length("meow", "homeowner") == 4
# assert lcs_length("fun", "") == 0
# assert lcs_length("fun", "function") == 3
# assert lcs_length("cyborg", "cyber") == 3
# assert lcs_length("physics", "physics") == 7
# assert lcs_length("space age", "pace a") == 6
# assert lcs_length("flippy", "floppy") == 3
# assert lcs_length("acbdegcedbg", "begcfeubk") == 3

from typing import *
from nagini_contracts.contracts import *


def lcs_length(s: str, t: str) -> int:
    """
    Longest Common Substring length (as in the original QuixBugs code).
    Implementation is unchanged.
    """
    Requires(s is not None)
    Requires(t is not None)
    Ensures(Result() >= 0)

    from collections import Counter
    dp: dict = Counter()  # type annotation so Nagini is happy

    for i in range(len(s)):
        for j in range(len(t)):
            if s[i] == t[j]:
                dp[i, j] = dp[i - 1, j - 1] + 1

    return max(dp.values()) if dp else 0


# -------------------- Tests (from original asserts) --------------------


def test_lcs_1() -> None:
    Assert(lcs_length("witch", "sandwich") == 2)


def test_lcs_2() -> None:
    Assert(lcs_length("meow", "homeowner") == 4)


def test_lcs_3() -> None:
    Assert(lcs_length("fun", "") == 0)


def test_lcs_4() -> None:
    Assert(lcs_length("fun", "function") == 3)


def test_lcs_5() -> None:
    Assert(lcs_length("cyborg", "cyber") == 3)


def test_lcs_6() -> None:
    Assert(lcs_length("physics", "physics") == 7)


def test_lcs_7() -> None:
    Assert(lcs_length("space age", "pace a") == 6)


def test_lcs_8() -> None:
    Assert(lcs_length("flippy", "floppy") == 3)


def test_lcs_9() -> None:
    Assert(lcs_length("acbdegcedbg", "begcfeubk") == 3)

