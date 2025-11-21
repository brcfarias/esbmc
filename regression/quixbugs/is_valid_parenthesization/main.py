from nagini_contracts.contracts import *


def is_valid_parenthesization(parens: str) -> bool:
    """
    Checks whether a string of parentheses is well-formed.
    Implementation unchanged — only wrapped for Nagini.
    """
    Requires(parens is not None)
    Ensures(Result() == True or Result() == False)

    depth: int = 0
    for paren in parens:
        if paren == '(':
            depth += 1
        else:
            depth -= 1
            # If at any point depth goes negative, it is invalid
            if depth < 0:
                return False

    # A valid parenthesization must end at depth 0
    return depth == 0


# ------------------- Tests (converted from asserts) -------------------


def test_valid1() -> None:
    Assert(is_valid_parenthesization("((()()))()") == True)


def test_invalid1() -> None:
    Assert(is_valid_parenthesization(")()(") == False)


def test_invalid2() -> None:
    Assert(is_valid_parenthesization("((") == False)
