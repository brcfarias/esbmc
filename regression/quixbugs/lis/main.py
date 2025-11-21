
# def lis(arr):
#     ends = {}
#     longest = 0

#     for i, val in enumerate(arr):

#         prefix_lengths = [j for j in range(1, longest + 1) if arr[ends[j]] < val]

#         length = max(prefix_lengths) if prefix_lengths else 0

#         if length == longest or val < arr[ends[length + 1]]:
#             ends[length + 1] = i
#             longest = max(longest, length + 1)

#     return longest

# assert lis([]) == 0
# assert lis([3]) == 1
# assert lis([10, 20, 11, 32, 22, 48, 43]) == 4
# assert lis([4, 2, 1]) == 1
# assert lis([5, 1, 3, 4, 7]) == 4
# assert lis([4, 1]) == 1
# assert lis([-1, 0, 2]) == 3
# assert lis([0, 2]) == 2
# assert lis([4, 1, 5, 3, 7, 6, 2]) == 3
# assert lis([10, 22, 9, 33, 21, 50, 41, 60, 80]) == 6
# assert lis([7, 10, 9, 2, 3, 8, 1]) == 3
# assert lis([9, 11, 2, 13, 7, 15]) == 4

from typing import List, Dict
from nagini_contracts.contracts import *


def lis(arr: List[int]) -> int:
    """
    Longest Increasing Subsequence (length only).
    Implementation unchanged; only wrapped for Nagini.
    """
    Requires(list_pred(arr))
    Ensures(Result() >= 0)

    ends: Dict[int, int] = {}
    longest: int = 0

    for i, val in enumerate(arr):
        prefix_lengths: List[int] = [
            j for j in range(1, longest + 1) if arr[ends[j]] < val
        ]

        length: int = max(prefix_lengths) if prefix_lengths else 0

        if length == longest or val < arr[ends[length + 1]]:
            ends[length + 1] = i
            longest = max(longest, length + 1)

    return longest


# ---------------------- Tests ----------------------


def test_lis_empty() -> None:
    Assert(lis([]) == 0)


def test_lis_single() -> None:
    Assert(lis([3]) == 1)


def test_lis_example1() -> None:
    Assert(lis([10, 20, 11, 32, 22, 48, 43]) == 4)


def test_lis_descending() -> None:
    Assert(lis([4, 2, 1]) == 1)


def test_lis_example2() -> None:
    Assert(lis([5, 1, 3, 4, 7]) == 4)


def test_lis_two_elements() -> None:
    Assert(lis([4, 1]) == 1)


def test_lis_negative() -> None:
    Assert(lis([-1, 0, 2]) == 3)


def test_lis_two_inc() -> None:
    Assert(lis([0, 2]) == 2)


def test_lis_example3() -> None:
    Assert(lis([4, 1, 5, 3, 7, 6, 2]) == 3)


def test_lis_long1() -> None:
    Assert(lis([10, 22, 9, 33, 21, 50, 41, 60, 80]) == 6)


def test_lis_long2() -> None:
    Assert(lis([7, 10, 9, 2, 3, 8, 1]) == 3)


def test_lis_long3() -> None:
    Assert(lis([9, 11, 2, 13, 7, 15]) == 4)
