from nagini_contracts.contracts import *


def sqrt(x: float, epsilon: float) -> float:
    Requires(x >= 0.0)
    Requires(epsilon > 0.0)
    # Resultado não-negativo
    Ensures(Result() >= 0.0)

    approx: float = x / 2.0
    while abs(x - approx ** 2) > epsilon:
        Invariant(x >= 0.0)
        Invariant(epsilon > 0.0)
        # Mantemos approx positivo (evita divisão por zero e problemas numéricos)
        Invariant(approx > 0.0)
        approx = 0.5 * (approx + x / approx)
    return approx

def test1() -> None:
    r = sqrt(2.0, 0.01)
    # Em vez de checar igualdade exata com 1.4166..., checamos a propriedade:
    Assert(abs(2.0 - r * r) <= 0.01)


def test2() -> None:
    r = sqrt(2.0, 0.5)
    Assert(abs(2.0 - r * r) <= 0.5)


def test3() -> None:
    r = sqrt(2.0, 0.3)
    Assert(abs(2.0 - r * r) <= 0.3)


def test4() -> None:
    r = sqrt(4.0, 0.2)
    Assert(abs(4.0 - r * r) <= 0.2)


def test5() -> None:
    r = sqrt(170.0, 0.03)
    Assert(abs(170.0 - r * r) <= 0.03)

