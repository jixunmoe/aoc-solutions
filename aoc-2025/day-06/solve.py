import argparse
import numpy as np


def solve_p1(input_text: str) -> int:
    numbers_str = input_text.splitlines()
    ops = numbers_str.pop().split()
    numbers = np.array([np.array(list(map(int, n.split()))) for n in numbers_str])

    results = []
    for i, op in enumerate(ops, start=0):
        if op == "+":
            results.append(np.sum(numbers[:, i], dtype=np.int64))
        elif op == "*":
            results.append(np.prod(numbers[:, i], dtype=np.int64))
    return np.sum(results, dtype=np.int64).item()


def solve_p2(input_text: str, verbose=False) -> int:
    lines = input_text.splitlines()
    line_len = max(map(len, lines))
    lines = [line.ljust(line_len, " ") for line in lines]

    values = []
    result = 0
    for i in range(line_len - 1, -1, -1):
        number = "".join(line[i] for line in lines).strip()
        if not number:
            assert not values
            continue
        if number[-1] not in "+*":
            values.append(int(number))
        else:
            op = number[-1]
            values.append(int(number[:-1].strip()))
            fn = np.sum if op == "+" else np.prod
            fn_result = fn(values, dtype=np.int64).item()
            if verbose:
                print(f"{values} {op} => {fn_result}")
            values = []
            result += fn_result
    return result


def solve(input_path: str, verbose=False, /, **_kwargs):
    with open(input_path, "r", encoding="utf-8") as file:
        input_text = file.read().strip().replace("\r", "")

    p1 = solve_p1(input_text)
    print(f"p1: {p1}")

    p2 = solve_p2(input_text, verbose=True)
    print(f"p2: {p2}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", nargs="?", default="sample.txt")
    parser.add_argument("-v", "--verbose", action="store_true")
    args = parser.parse_args()
    solve(args.input, verbose=args.verbose)


if __name__ == "__main__":
    main()
