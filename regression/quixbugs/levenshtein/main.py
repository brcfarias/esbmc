
# def levenshtein(source, target):
#     if source == '' or target == '':
#         return len(source) or len(target)

#     elif source[0] == target[0]:
#         return levenshtein(source[1:], target[1:])

#     else:
#         return 1 + min(
#             levenshtein(source,     target[1:]),
#             levenshtein(source[1:], target[1:]),
#             levenshtein(source[1:], target)
#         )


# assert levenshtein("electron", "neutron") == 3
# assert levenshtein("kitten", "sitting") == 3
# assert levenshtein("rosettacode", "raisethysword") == 8
# # assert levenshtein(
# #     "amanaplanacanalpanama",
# #     "docnoteidissentafastneverpreventsafatnessidietoncod"
# # ) == 42
# assert levenshtein("abcdefg", "gabcdef") == 2
# assert levenshtein("", "") == 0
# assert levenshtein("hello", "olleh") == 4

from nagini_contracts.contracts import *


def levenshtein(source: str, target: str) -> int:
    """
    Pure recursive Levenshtein distance.
    Implementation STAYS IDENTICAL to the original.
    """
    Requires(source is not None)
    Requires(target is not None)
    Ensures(Result() >= 0)

    if source == '' or target == '':
        return len(source) or len(target)

    elif source[0] == target[0]:
        return levenshtein(source[1:], target[1:])

    else:
        return 1 + min(
            levenshtein(source,     target[1:]),
            levenshtein(source[1:], target[1:]),
            levenshtein(source[1:], target)
        )


# -------------------- TESTS (converted from asserts) --------------------


def test_lev_1() -> None:
    Assert(levenshtein("electron", "neutron") == 3)


def test_lev_2() -> None:
    Assert(levenshtein("kitten", "sitting") == 3)


def test_lev_3() -> None:
    Assert(levenshtein("rosettacode", "raisethysword") == 8)


# The large-case test is commented in the original, keep it commented.
# def test_lev_big() -> None:
#     Assert(
#         levenshtein(
#             "amanaplanacanalpanama",
#             "docnoteidissentafastneverpreventsafatnessidietoncod"
#         ) == 42
#     )


def test_lev_4() -> None:
    Assert(levenshtein("abcdefg", "gabcdef") == 2)


def test_lev_5() -> None:
    Assert(levenshtein("", "") == 0)


def test_lev_6() -> None:
    Assert(levenshtein("hello", "olleh") == 4)
