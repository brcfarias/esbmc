from typing import List, Tuple
from nagini_contracts.contracts import *
from nagini_contracts.obligations import MustTerminate

def hanoi(height: int, start: int = 1, end: int = 3) -> List[Tuple[int, int]]:
    Requires(height >= 0)
    Requires(start >= 1 and start <= 3)
    Requires(end >= 1 and end <= 3)
    Requires(MustTerminate(height + 1))

    Ensures(list_pred(Result()))

    steps: List[Tuple[int, int]] = []
    if height > 0:
        helper: int = ({1, 2, 3} - {start} - {end}).pop()
        steps.extend(hanoi(height - 1, start, helper))
        steps.append((start, end))
        steps.extend(hanoi(height - 1, helper, end))

    return steps

def test_hanoi_0_1_3() -> None:
    Assert(hanoi(0, 1, 3) == [])


def test_hanoi_1_1_3() -> None:
    Assert(hanoi(1, 1, 3) == [(1, 3)])


def test_hanoi_2_1_3() -> None:
    Assert(hanoi(2, 1, 3) == [(1, 2), (1, 3), (2, 3)])


def test_hanoi_3_1_3() -> None:
    Assert(
        hanoi(3, 1, 3)
        == [
            (1, 3), (1, 2), (3, 2),
            (1, 3), (2, 1), (2, 3),
            (1, 3),
        ]
    )


def test_hanoi_4_1_3() -> None:
    Assert(
        hanoi(4, 1, 3)
        == [
            (1, 2), (1, 3), (2, 3),
            (1, 2), (3, 1), (3, 2),
            (1, 2), (1, 3), (2, 3),
            (2, 1), (3, 1), (2, 3),
            (1, 2), (1, 3), (2, 3),
        ]
    )


def test_hanoi_2_1_2() -> None:
    Assert(hanoi(2, 1, 2) == [(1, 3), (1, 2), (3, 2)])


def test_hanoi_2_1_1() -> None:
    Assert(hanoi(2, 1, 1) == [(1, 2), (1, 1), (2, 1)])


def test_hanoi_2_3_1() -> None:
    Assert(hanoi(2, 3, 1) == [(3, 2), (3, 1), (2, 1)])

