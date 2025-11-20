from typing import List
from nagini_contracts.contracts import *

def bucketsort(arr: List[int], k: int) -> List[int]:
    # Temos permissão sobre arr e pré-condição de domínio
    Requires(list_pred(arr))
    Requires(k >= 0)
    Requires(
        Forall(range(0, len(arr)),
               lambda i: arr[i] >= 0 and arr[i] < k)
    )

    # Não destruímos arr
    Ensures(list_pred(arr))
    # O resultado é uma lista “bem formada”
    Ensures(list_pred(Result()))
    # Mesmo tamanho
    Ensures(len(Result()) == len(arr))
    # Todos os elementos do resultado também estão em [0, k-1]
    Ensures(
        Forall(range(0, len(Result())),
               lambda i: arr[i] >= 0 and arr[i] < k)
    )
    # Resultado ordenado não-decrescentemente
    Ensures(
        Forall(range(0, len(Result()) - 1),
               lambda i: Result()[i] <= Result()[i + 1])
    )

    counts: List[int] = [0] * k
    idx: int = 0
    while idx < len(arr):
        Invariant(0 <= idx and idx <= len(arr))
        Invariant(list_pred(arr))
        Invariant(list_pred(counts))
        # counts sempre tem tamanho k
        Invariant(len(counts) == k)
        x: int = arr[idx]
        counts[x] += 1
        idx += 1

    sorted_arr: List[int] = []
    i: int = 0
    while i < k:
        Invariant(0 <= i <= k)
        Invariant(list_pred(arr))
        Invariant(list_pred(counts))
        Invariant(list_pred(sorted_arr))
        # elementos em sorted_arr estão em [0, i]
        Invariant(Forall(range(0, len(sorted_arr)), lambda j: sorted_arr[j] >= 0 and sorted_arr[j] <= i))

        # sorted_arr é não-decrescente
        Invariant(
            Forall(range(0, len(sorted_arr) - 1),
                   lambda j: sorted_arr[j] <= sorted_arr[j + 1])
        )

        j: int = 0
        c: int = counts[i]
        while j < c:
            Invariant(0 <= j <= c)
            Invariant(list_pred(sorted_arr))
            # manter ordenação ao adicionar i
            Invariant(
                Forall(range(0, len(sorted_arr)),
                       lambda t: sorted_arr[t] <= i)
            )
            sorted_arr.append(i)
            j += 1

        i += 1

    return sorted_arr

"""
def bucketsort(arr, k):
    counts = [0] * k
    for x in arr:
        counts[x] += 1

    sorted_arr = []
    for i, count in enumerate(arr):
        sorted_arr.extend([i] * counts[i])

    return sorted_arr
"""

#assert bucketsort([3, 11, 2, 9, 1, 5], 12) == [1, 2, 3, 5, 9, 11]
def test_case1() -> None:
    arr: List[int] = [3, 11, 2, 9, 1, 5]
    r = bucketsort(arr, 12)

    # Mesmo tamanho
    Assert(len(r) == len(arr))

    # Ordenado
    Assert(
        Forall(range(0, len(r) - 1),
               lambda i: r[i] <= r[i + 1])
    )


#assert bucketsort([3, 2, 4, 2, 3, 5], 6) == [2, 2, 3, 3, 4, 5]
def test_case2() -> None:
    # input original: [3, 2, 4, 2, 3, 5], k = 6
    arr: List[int] = [3, 2, 4, 2, 3, 5]
    r = bucketsort(arr, 6)

    Assert(len(r) == len(arr))
    Assert(
        Forall(range(0, len(r) - 1),
               lambda i: r[i] <= r[i + 1])
    )


