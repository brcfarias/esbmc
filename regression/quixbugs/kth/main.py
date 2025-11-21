
# def kth(arr, k):
#     pivot = arr[0]
#     below = [x for x in arr if x < pivot]
#     above = [x for x in arr if x > pivot]

#     num_less = len(below)
#     num_lessoreq = len(arr) - len(above)

#     if k < num_less:
#         return kth(below, k)
#     elif k >= num_lessoreq:
#         return kth(above, k - num_lessoreq)
#     else:
#         return pivot

# assert kth([1, 2, 3, 4, 5, 6, 7], 4) == 5
#assert kth([3, 6, 7, 1, 6, 3, 8, 9], 5) == 7
#assert kth([3, 6, 7, 1, 6, 3, 8, 9], 2) == 3
#assert kth([2, 6, 8, 3, 5, 7], 0) == 2
#assert kth([34, 25, 7, 1, 9], 4) == 34
#assert kth([45, 2, 6, 8, 42, 90, 322], 1) == 6
#assert kth([45, 2, 6, 8, 42, 90, 322], 6) == 322

from typing import List
from nagini_contracts.contracts import *


def kth(arr: List[int], k: int) -> int:
    """
    k-th smallest selection by quickselect partitioning.
    Implementation unchanged.
    """
    Requires(list_pred(arr))
    Requires(len(arr) > 0)
    Requires(k >= 0 and k < len(arr))
    Ensures(Result() in arr)

    pivot: int = arr[0]
    below: List[int] = [x for x in arr if x < pivot]
    above: List[int] = [x for x in arr if x > pivot]

    num_less: int = len(below)
    num_lessoreq: int = len(arr) - len(above)

    if k < num_less:
        return kth(below, k)
    elif k >= num_lessoreq:
        return kth(above, k - num_lessoreq)
    else:
        return pivot


# ---------------- TESTES ----------------

def test_kth_1() -> None:
    Assert(kth([1, 2, 3, 4, 5, 6, 7], 4) == 5)
