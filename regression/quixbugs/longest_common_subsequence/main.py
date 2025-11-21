
# def longest_common_subsequence(a, b):
#     if not a or not b:
#         return ''

#     elif a[0] == b[0]:
#         return a[0] + longest_common_subsequence(a[1:], b[1:])

#     else:
#         return max(
#             longest_common_subsequence(a, b[1:]),
#             longest_common_subsequence(a[1:], b),
#             key=len
#         )

# assert longest_common_subsequence("headache", "pentadactyl") == "eadac"
# assert longest_common_subsequence("daenarys", "targaryen") == "aary"
# assert longest_common_subsequence("XMJYAUZ", "MZJAWXU") == "MJAU"
# assert longest_common_subsequence("thisisatest", "testing123testing") == "tsitest"
# assert longest_common_subsequence("1234", "1224533324") == "1234"
# assert longest_common_subsequence("abcbdab", "bdcaba") == "bcba"
# assert longest_common_subsequence("TATAGC", "TAGCAG") == "TAAG"
# assert longest_common_subsequence("ABCBDAB", "BDCABA") == "BCBA"
# assert longest_common_subsequence("ABCD", "XBCYDQ") == "BCD"
# assert longest_common_subsequence("acbdegcedbg", "begcfeubk") == "begceb"

from nagini_contracts.contracts import *


def longest_common_subsequence(a: str, b: str) -> str:
    """
    Pure recursive LCS — implementation unchanged.
    """
    Requires(a is not None)
    Requires(b is not None)
    Ensures(Result() is not None)

    if not a or not b:
        return ''

    elif a[0] == b[0]:
        return a[0] + longest_common_subsequence(a[1:], b[1:])

    else:
        return max(
            longest_common_subsequence(a, b[1:]),
            longest_common_subsequence(a[1:], b),
            key=len
        )


# ---------------------- TESTS ----------------------

def test_lcs_1() -> None:
    Assert(longest_common_subsequence("headache", "pentadactyl") == "eadac")


def test_lcs_2() -> None:
    Assert(longest_common_subsequence("daenarys", "targaryen") == "aary")


def test_lcs_3() -> None:
    Assert(longest_common_subsequence("XMJYAUZ", "MZJAWXU") == "MJAU")


def test_lcs_4() -> None:
    Assert(longest_common_subsequence("thisisatest", "testing123testing") == "tsitest")


def test_lcs_5() -> None:
    Assert(longest_common_subsequence("1234", "1224533324") == "1234")


def test_lcs_6() -> None:
    Assert(longest_common_subsequence("abcbdab", "bdcaba") == "bcba")


def test_lcs_7() -> None:
    Assert(longest_common_subsequence("TATAGC", "TAGCAG") == "TAAG")


def test_lcs_8() -> None:
    Assert(longest_common_subsequence("ABCBDAB", "BDCABA") == "BCBA")


def test_lcs_9() -> None:
    Assert(longest_common_subsequence("ABCD", "XBCYDQ") == "BCD")


def test_lcs_10() -> None:
    Assert(longest_common_subsequence("acbdegcedbg", "begcfeubk") == "begceb")
