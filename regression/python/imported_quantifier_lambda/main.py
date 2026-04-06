from api import *


def f() -> bool:
    ensure(not current() or all_of(int, lambda idx: idx < 1))
    ensure(not (not current()) or any_of(int, lambda idx: idx >= 0))
    return True


assert f()
