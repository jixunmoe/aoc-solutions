import argparse


def solve_p1(text: str):
    count = 0
    location = 50
    for line in text.splitlines():
        direction = -1 if line[0] == "L" else 1
        distance = direction * int(line[1:])
        location = (location + distance) % 100
        if location == 0:
            count += 1
    return count


def solve(input_path: str, /, verbose=False, **_kwargs):
    with open(input_path, "r", encoding="utf-8") as file:
        input_text = file.read().strip().replace("\r", "")

    p1 = solve_p1(input_text)
    print(f"Part 1: {p1}")
    print("Part 2: see `solve_p2.c`")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", nargs="?", default="sample.txt")
    parser.add_argument("-v", "--verbose", action="store_true")
    args = parser.parse_args()
    solve(args.input, verbose=args.verbose)


if __name__ == "__main__":
    main()
