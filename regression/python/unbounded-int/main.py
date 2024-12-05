
def my_int_size(n:int) -> int:
    length:int = 0

    while n > 0:
        n:int = n >> 1
        length:int = length + 1  # Count how many times the number is shifted

    return length

#a = 15
#assert my_int_size(a) == 4


b = 52435875175126190479447740508185965837690552500527637822603658699938581184513
assert b == 52435875175126190479447740508185965837690552500527637822603658699938581184513
assert b > 10

#my_int_size(b)

assert my_int_size(b) == 1
#assert my_int_size(b) == 255
