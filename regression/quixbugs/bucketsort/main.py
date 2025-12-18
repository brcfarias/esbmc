
def foo() -> None:
    counts = [0]
    counts[0] += 1
    assert counts[0] == 1

foo()
# assert bucketsort([3, 11, 2, 9, 1, 5], 12) == [1, 2, 3, 5, 9, 11]
# assert bucketsort([3, 2, 4, 2, 3, 5], 6) == [2, 2, 3, 3, 4, 5]
