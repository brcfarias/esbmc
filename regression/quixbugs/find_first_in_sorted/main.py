from typing import List
from nagini_contracts.contracts import *
from nagini_contracts.obligations import MustTerminate

def find_first_in_sorted(arr: List[int], x:int) -> int:
    # Very weak contracts: just to satisfy Nagini/mypy.
    Requires(list_pred(arr))
    Ensures(list_pred(arr))
    
    # Ensures(Implies(Result() != -1,
    # Result() >= 0 and Result() < len(arr)))

    Ensures(
        Implies(
            Result() == -1,
            Forall(range(0, len(arr)), lambda j: arr[j] != x)
        )
    )

    # Ensures(
    #     Implies(
    #         Result() != -1,
    #         And(
    #             Result() >= 0,
    #             Result() < len(arr),
    #             arr[Result()] == x,
    #             Forall(range(0, Result()), lambda j: arr[j] != x)
    #         )
    #     )
    # )

    lo: int = 0
    hi: int = len(arr)

    while lo < hi:
        Invariant(list_pred(arr))
        Invariant(0 <= lo)
        Invariant(lo <= hi)
        Invariant(hi <= len(arr))
        # Antes de 'lo' não existe x
        Invariant(Forall(range(0, lo), lambda j: arr[j] != x))
        # A partir de 'hi' também não existe x
        Invariant(Forall(range(hi, len(arr)), lambda j: arr[j] != x))

        mid: int = (lo + hi) // 2

        if x == arr[mid] and (mid == 0 or x != arr[mid - 1]):
            return mid

        elif x <= arr[mid]:
            hi = mid

        else:
            lo = mid + 1

    return -1

"""
def find_first_in_sorted(arr, x):
    lo = 0
    hi = len(arr)

    while lo <= hi - 1:
        mid = (lo + hi) // 2

        if x == arr[mid] and (mid == 0 or x != arr[mid - 1]):
            return mid

        elif x <= arr[mid]:
            hi = mid

        else:
            lo = mid + 1

    return -1

def find_first_in_sorted(arr, x):
    lo = 0
    hi = len(arr)

    while lo + 1 <= hi:
        mid = (lo + hi) // 2

        if x == arr[mid] and (mid == 0 or x != arr[mid - 1]):
            return mid

        elif x <= arr[mid]:
            hi = mid

        else:
            lo = mid + 1

    return -1

"""

#assert find_first_in_sorted([3, 4, 5, 5, 5, 5, 6], 5) == 2
def test_case1() -> None:
    arr: List[int] = [3, 4, 5, 5, 5, 5, 6]
    r = find_first_in_sorted(arr, 5)
    # r precisa ser um índice válido
    Assert(r != -1)
    Assert(arr[r] == 5)
    # por contrato, não deve ter 5 antes
    Assert(Forall(range(0, r), lambda j: arr[j] != 5))


#assert find_first_in_sorted([3, 4, 5, 5, 5, 5, 6], 7) == -1
def test_case2() -> None:
    arr: List[int] = [3, 4, 5, 5, 5, 5, 6]
    r = find_first_in_sorted(arr, 7)
    Assert(r == -1)

# assert find_first_in_sorted([3, 4, 5, 5, 5, 5, 6], 2) == -1
def test_case3() -> None:
    arr: List[int] = [3, 4, 5, 5, 5, 5, 6]
    r = find_first_in_sorted(arr, 2)
    Assert(r == -1)

# assert find_first_in_sorted([3, 6, 7, 9, 9, 10, 14, 27], 14) == 6
def test_case4() -> None:
    arr: List[int] = [3, 6, 7, 9, 9, 10, 14, 27]
    r = find_first_in_sorted(arr, 14)
    Assert(r != -1)
    Assert(arr[r] == 14)
    Assert(Forall(range(0, r), lambda j: arr[j] != 14))

# assert find_first_in_sorted([0, 1, 6, 8, 13, 14, 67, 128], 80) == -1
def test_case5() -> None:
    arr: List[int] = [0, 1, 6, 8, 13, 14, 67, 128]
    r = find_first_in_sorted(arr, 80)
    Assert(r == -1)

# assert find_first_in_sorted([0, 1, 6, 8, 13, 14, 67, 128], 67) == 6
def test_case6() -> None:
    arr: List[int] = [0, 1, 6, 8, 13, 14, 67, 128]
    r = find_first_in_sorted(arr, 67)
    Assert(r != -1)
    Assert(arr[r] == 67)
    Assert(Forall(range(0, r), lambda j: arr[j] != 67))

# assert find_first_in_sorted([0, 1, 6, 8, 13, 14, 67, 128], 128) == 7
def test_case7() -> None:
    arr: List[int] = [0, 1, 6, 8, 13, 14, 67, 128]
    r = find_first_in_sorted(arr, 128)
    Assert(r != -1)
    Assert(arr[r] == 128)
    Assert(Forall(range(0, r), lambda j: arr[j] != 128))

