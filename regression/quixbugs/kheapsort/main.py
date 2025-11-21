from typing import List
from nagini_contracts.contracts import *
import heapq


def kheapsort(arr: List[int], k: int) -> List[int]:
    """
    k-heapsort: mantém um heap de tamanho k e emite os elementos em ordem.
    Sem generators, para ser compatível com Nagini, mas com a mesma lógica
    de heap do código original.
    """
    Requires(list_pred(arr))
    Requires(k >= 0)
    Ensures(list_pred(arr))          # não destrói o array de entrada
    Ensures(list_pred(Result()))
    Ensures(len(Result()) == len(arr))

    heap: List[int] = arr[:k]
    heapq.heapify(heap)

    result: List[int] = []

    # Parte que antes fazia `for x in arr[k:]: yield heappushpop(...)`
    i: int = k
    while i < len(arr):
        Invariant(list_pred(arr))
        Invariant(list_pred(heap))
        Invariant(list_pred(result))
        Invariant(k <= i and i <= len(arr))

        x: int = arr[i]
        y: int = heapq.heappushpop(heap, x)
        result.append(y)
        i += 1

    # Parte que antes fazia `while heap: yield heappop(heap)`
    while heap:
        Invariant(list_pred(heap))
        Invariant(list_pred(result))
        z: int = heapq.heappop(heap)
        result.append(z)

    return result


# --------- Testes (mesma intenção dos originais) ---------

def test1() -> None:
    Assert(kheapsort([1, 2, 3, 4, 5], 0) == [1, 2, 3, 4, 5])


def test2() -> None:
    Assert(kheapsort([3, 2, 1, 5, 4], 2) == [1, 2, 3, 4, 5])


def test3() -> None:
    Assert(kheapsort([5, 4, 3, 2, 1], 4) == [1, 2, 3, 4, 5])


def test4() -> None:
    Assert(kheapsort([3, 12, 5, 1, 6], 3) == [1, 3, 5, 6, 12])
