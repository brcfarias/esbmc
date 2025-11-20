from typing import List
from nagini_contracts.contracts import *
from nagini_contracts.obligations import MustTerminate


def quicksort(arr: List[int]) -> List[int]:
    """
    Pure recursive quicksort.
    Nagini-friendly: no comprehensions, no chained comparisons,
    explicit loops, clear termination measure.
    """
    Requires(list_pred(arr))
    # Termination measure: length decreases strictly in every recursive call
    Requires(MustTerminate(len(arr) + 1))

    Ensures(list_pred(arr))          # arr not modified
    Ensures(list_pred(Result()))     # result is a list
    Ensures(len(Result()) == len(arr))
    # Result sorted:
    Ensures(
        Forall(range(0, len(Result()) - 1),
               lambda i: Result()[i] <= Result()[i + 1])
    )

    n: int = len(arr)
    if n == 0:
        return []

    pivot: int = arr[0]

    # Build lesser = [x in arr[1:] if x < pivot]
    lesser: List[int] = []
    i: int = 1
    while i < n:
        Invariant(1 <= i and i <= n)
        Invariant(list_pred(lesser))
        # Keep lesser sorted only after recursive call (not needed as invariant)
        if arr[i] < pivot:
            lesser.append(arr[i])
        i += 1

    # Build greater = [x in arr[1:] if x >= pivot]
    greater: List[int] = []
    j: int = 1
    while j < n:
        Invariant(1 <= j and j <= n)
        Invariant(list_pred(greater))
        if arr[j] >= pivot:
            greater.append(arr[j])
        j += 1

    sorted_lesser: List[int] = quicksort(lesser)
    sorted_greater: List[int] = quicksort(greater)

    # Concatenate: sorted_lesser + [pivot] + sorted_greater
    result: List[int] = []

    # append sorted_lesser
    k: int = 0
    while k < len(sorted_lesser):
        Invariant(0 <= k and k <= len(sorted_lesser))
        Invariant(list_pred(result))
        result.append(sorted_lesser[k])
        k += 1

    # append pivot
    result.append(pivot)

    # append sorted_greater
    h: int = 0
    while h < len(sorted_greater):
        Invariant(0 <= h and h <= len(sorted_greater))
        Invariant(list_pred(result))
        result.append(sorted_greater[h])
        h += 1

    return result


# ------------------- Tests -------------------

def is_sorted(a: List[int]) -> bool:
    i: int = 0
    while i + 1 < len(a):
        if a[i] > a[i + 1]:
            return False
        i += 1
    return True


def same_multiset(a: List[int], b: List[int], k: int) -> bool:
    """Check that a and b contain the same number of occurrences for values in [0, k)."""
    v: int = 0
    while v < k:
        cnt1: int = 0
        cnt2: int = 0

        i: int = 0
        while i < len(a):
            if a[i] == v:
                cnt1 += 1
            i += 1

        j: int = 0
        while j < len(b):
            if b[j] == v:
                cnt2 += 1
            j += 1

        if cnt1 != cnt2:
            return False

        v += 1

    return True


def test_quicksort_simple() -> None:
    arr: List[int] = [1, 2, 6, 72, 7, 33, 4]
    r: List[int] = quicksort(arr)

    # Sortedness
    Assert(is_sorted(r))

    # Same number of elements (pigeonhole property)
    Assert(len(r) == len(arr))

    # Correct multiset (for inputs in small range; here values < 100)
    Assert(same_multiset(arr, r, 100))
