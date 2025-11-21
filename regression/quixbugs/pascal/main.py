
# def pascal(n):
#     rows = [[1]]
#     for r in range(1, n):
#         row = []
#         for c in range(0, r + 1):
#             upleft = rows[r - 1][c - 1] if c > 0 else 0
#             upright = rows[r - 1][c] if c < r else 0
#             row.append(upleft + upright)
#         rows.append(row)

#     return rows

# assert pascal(1) == [[1]]
# assert pascal(2) == [[1], [1, 1]]
# assert pascal(3) == [[1], [1, 1], [1, 2, 1]]
# assert pascal(4) == [[1], [1, 1], [1, 2, 1], [1, 3, 3, 1]]
# assert pascal(5) == [[1], [1, 1], [1, 2, 1], [1, 3, 3, 1], [1, 4, 6, 4, 1]]

from typing import List
from nagini_contracts.contracts import *


def pascal(n: int) -> List[List[int]]:
    """
    Generate the first n rows of Pascal's triangle.
    Implementation unchanged.
    """
    Requires(n >= 1)
    Ensures(list_pred(Result()))
    Ensures(Forall(range(0, len(Result())), lambda i: list_pred(Result()[i])))

    rows: List[List[int]] = [[1]]
    for r in range(1, n):
        row: List[int] = []
        for c in range(0, r + 1):
            upleft = rows[r - 1][c - 1] if c > 0 else 0
            upright = rows[r - 1][c] if c < r else 0
            row.append(upleft + upright)
        rows.append(row)

    return rows


# ---------------------- TESTS ----------------------

def test_pascal_1() -> None:
    Assert(pascal(1) == [[1]])


def test_pascal_2() -> None:
    Assert(pascal(2) == [[1], [1, 1]])


def test_pascal_3() -> None:
    Assert(pascal(3) == [[1], [1, 1], [1, 2, 1]])


def test_pascal_4() -> None:
    Assert(pascal(4) == [[1], [1, 1], [1, 2, 1], [1, 3, 3, 1]])


def test_pascal_5() -> None:
    Assert(pascal(5) == [[1], [1, 1], [1, 2, 1], [1, 3, 3, 1], [1, 4, 6, 4, 1]])
