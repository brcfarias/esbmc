from typing import List
from nagini_contracts.contracts import *


def max_sublist_sum(arr: List[int]) -> int:
    # We treat arr as a list we have permission to read
    Requires(list_pred(arr))
    Ensures(list_pred(arr))
    # Result is always >= 0 in this variant of Kadane
    Ensures(Result() >= 0)

    max_ending_here: int = 0
    max_so_far: int = 0

    for x in arr:
        # Kadane's algorithm
        max_ending_here = max(0, max_ending_here + x)
        max_so_far = max(max_so_far, max_ending_here)

    return max_so_far


def test1() -> None:
    arr: List[int] = [4, -5, 2, 1, -1, 3]
    # This is exactly your original assertion, just using Nagini's Assert
    Assert(max_sublist_sum(arr) == 5)
