from typing import List
from nagini_contracts.contracts import *
from nagini_contracts.obligations import MustTerminate


def get_factors(n: int) -> List[int]:
    # contratos mínimos, sem mudar a lógica
    Requires(n >= 1)
    # medida de término simples: a cada chamada recursiva n diminui
    Requires(MustTerminate(n + 1))
    Ensures(list_pred(Result()))

    if n == 1:
        return []

    for i in range(2, int(n ** 0.5) + 1):
        if n % i == 0:
            return [i] + get_factors(n // i)

    return [n]


# -------------------- TESTES --------------------


def test_1() -> None:
    Assert(get_factors(1) == [])


def test_100() -> None:
    Assert(get_factors(100) == [2, 2, 5, 5])


def test_101() -> None:
    Assert(get_factors(101) == [101])


def test_104() -> None:
    Assert(get_factors(104) == [2, 2, 2, 13])


def test_2() -> None:
    Assert(get_factors(2) == [2])


def test_3() -> None:
    Assert(get_factors(3) == [3])


def test_17() -> None:
    Assert(get_factors(17) == [17])


def test_63() -> None:
    Assert(get_factors(63) == [3, 3, 7])


def test_74() -> None:
    Assert(get_factors(74) == [2, 37])


def test_73() -> None:
    Assert(get_factors(73) == [73])


def test_9837() -> None:
    Assert(get_factors(9837) == [3, 3, 1093])
