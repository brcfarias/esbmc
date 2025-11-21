
# def next_permutation(perm):
#     for i in range(len(perm) - 2, -1, -1):
#         if perm[i] < perm[i + 1]:
#             for j in range(len(perm) - 1, i, -1):
#                 if perm[i] < perm[j]:
#                     next_perm = list(perm)
#                     next_perm[i], next_perm[j] = perm[j], perm[i]
#                     next_perm[i + 1:] = reversed(next_perm[i + 1:])
#                     return next_perm
                

# assert next_permutation([3, 2, 4, 1]) == [3, 4, 1, 2]
# assert next_permutation([3, 5, 6, 2, 1]) == [3, 6, 1, 2, 5]
# assert next_permutation([3, 5, 6, 2]) == [3, 6, 2, 5]
# assert next_permutation([4, 5, 1, 7, 9]) == [4, 5, 1, 9, 7]
# assert next_permutation([4, 5, 8, 7, 1]) == [4, 7, 1, 5, 8]
# assert next_permutation([9, 5, 2, 6, 1]) == [9, 5, 6, 1, 2]
# assert next_permutation([44, 5, 1, 7, 9]) == [44, 5, 1, 9, 7]
# assert next_permutation([3, 4, 5]) == [3, 5, 4]

from typing import List
from nagini_contracts.contracts import *


def next_permutation(perm: List[int]) -> List[int]:
    """
    Compute the next lexicographic permutation of the list 'perm'.
    Implementation is unchanged; only wrapped for Nagini.
    """
    Requires(perm is not None)
    Requires(list_pred(perm))
    Ensures(list_pred(Result()))

    for i in range(len(perm) - 2, -1, -1):
        if perm[i] < perm[i + 1]:
            for j in range(len(perm) - 1, i, -1):
                if perm[i] < perm[j]:
                    next_perm = list(perm)
                    next_perm[i], next_perm[j] = perm[j], perm[i]
                    next_perm[i + 1:] = reversed(next_perm[i + 1:])
                    return next_perm

    # original function has no explicit "else" case:
    # if no next permutation exists, Python returns None implicitly.
    # We keep that behavior (Nagini will see the missing return as "may return None").
    # If you prefer to be explicit, you could "return perm" here, but that *would*
    # change the semantics wrt the original QuixBugs code.


# ---------------------- Tests ----------------------


def test_np_1() -> None:
    Assert(next_permutation([3, 2, 4, 1]) == [3, 4, 1, 2])


def test_np_2() -> None:
    Assert(next_permutation([3, 5, 6, 2, 1]) == [3, 6, 1, 2, 5])


def test_np_3() -> None:
    Assert(next_permutation([3, 5, 6, 2]) == [3, 6, 2, 5])


def test_np_4() -> None:
    Assert(next_permutation([4, 5, 1, 7, 9]) == [4, 5, 1, 9, 7])


def test_np_5() -> None:
    Assert(next_permutation([4, 5, 8, 7, 1]) == [4, 7, 1, 5, 8])


def test_np_6() -> None:
    Assert(next_permutation([9, 5, 2, 6, 1]) == [9, 5, 6, 1, 2])


def test_np_7() -> None:
    Assert(next_permutation([44, 5, 1, 7, 9]) == [44, 5, 1, 9, 7])


def test_np_8() -> None:
    Assert(next_permutation([3, 4, 5]) == [3, 5, 4])
