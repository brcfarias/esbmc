from typing import List
from nagini_contracts.contracts import *


def knapsack(capacity: int, items: List[List[int]]) -> int:
    """
    0/1 knapsack dynamic programming.
    Implementation is kept exactly the same as in the original code.
    """
    Requires(capacity >= 0)
    Requires(items is not None)
    Ensures(Result() >= 0)

    from collections import defaultdict
    memo: dict = defaultdict(int)

    for i in range(1, len(items) + 1):
        weight, value = items[i - 1]

        for j in range(1, capacity + 1):
            memo[i, j] = memo[i - 1, j]

            if weight <= j:
                memo[i, j] = max(
                    memo[i, j],
                    value + memo[i - 1, j - weight]
                )

    return memo[len(items), capacity]


# ---------------------- Tests ----------------------


def test_knapsack_1() -> None:
    Assert(knapsack(100, [[60, 10], [50, 8], [20, 4], [20, 4], [8, 3], [3, 2]]) == 19)


def test_knapsack_2() -> None:
    Assert(knapsack(40, [[30, 10], [50, 5], [10, 20], [40, 25]]) == 30)


def test_knapsack_3() -> None:
    Assert(knapsack(750, [
        [70, 135], [73, 139], [77, 149], [80, 150], [82, 156],
        [87, 163], [90, 173], [94, 184], [98, 192], [106, 201],
        [110, 210], [113, 214], [115, 221], [118, 229], [120, 240]
    ]) == 1458)


def test_knapsack_4() -> None:
    Assert(knapsack(26, [[12, 24], [7, 13], [11, 23], [8, 15], [9, 16]]) == 51)


def test_knapsack_5() -> None:
    Assert(knapsack(50, [[31, 70], [10, 20], [20, 39], [19, 37], [4, 7], [3, 5], [6, 10]]) == 107)


def test_knapsack_6() -> None:
    Assert(knapsack(190, [[56, 50], [59, 50], [80, 64], [64, 46], [75, 50], [17, 5]]) == 150)


def test_knapsack_7() -> None:
    Assert(knapsack(104, [
        [25, 350], [35, 400], [45, 450], [5, 20],
        [25, 70], [3, 8], [2, 5], [2, 5]
    ]) == 900)


def test_knapsack_8() -> None:
    Assert(knapsack(165, [
        [23, 92], [31, 57], [29, 49], [44, 68], [53, 60],
        [38, 43], [63, 67], [85, 84], [89, 87], [82, 72]
    ]) == 309)


def test_knapsack_9() -> None:
    Assert(knapsack(170, [
        [41, 442], [50, 525], [49, 511], [59, 593],
        [55, 546], [57, 564], [60, 617]
    ]) == 1735)
