import argparse


def find_repeat_digits(min_int: int, max_int: int, verbose=False, partition_size=2):
    min_len = len(str(min_int))
    max_len = len(str(max_int))

    if min_len % partition_size != 0:
        min_len += partition_size - (min_len % partition_size)
        min_int = 10 ** (min_len - 1)
    if max_len % partition_size != 0:
        max_len -= max_len % partition_size
        max_int = (10**max_len) - 1

    if min_int > max_int:
        return

    min_str = str(min_int)
    max_str = str(max_int)

    min_left = int(min_str[: len(min_str) // partition_size])
    max_left = int(max_str[: len(max_str) // partition_size])

    lower_bound = min(min_left, max_left)
    upper_bound = max(min_left, max_left)
    for i in range(lower_bound, upper_bound + 1):
        repeat_value = int(str(i) * partition_size)
        if min_int <= repeat_value <= max_int:
            if verbose:
                print(f"  - {repeat_value}")
            yield repeat_value


def solve(input_path: str, /, verbose=False, **_kwargs):
    with open(input_path, "r", encoding="utf-8") as file:
        input_text = file.read().strip().replace("\r", "")

    values_by_2 = set()
    values_by_all = set()

    for min, max in map(lambda x: tuple(map(int, x.split("-"))), input_text.split(",")):
        if verbose:
            print(f"Range: {(min, max)}")
        sum_by_2 = set(find_repeat_digits(min, max, verbose))
        values_by_2.update(sum_by_2)
        values_by_all.update(sum_by_2)
        for i in range(3, 11):
            values_by_all.update(set(find_repeat_digits(min, max, verbose, i)))
    p1 = sum(values_by_2)
    p2 = sum(values_by_all)
    print(f"p1: {p1}")
    print(f"p2: {p2}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", nargs="?", default="sample.txt")
    parser.add_argument("-v", "--verbose", action="store_true")
    args = parser.parse_args()
    solve(args.input, verbose=args.verbose)


if __name__ == "__main__":
    main()
