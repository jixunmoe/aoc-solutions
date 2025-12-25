import argparse


def parse_blueprint(blueprint):
    lines = blueprint.splitlines()
    content = [list(x) for x in lines[1:]]
    return content


def parse_line(line):
    [dimension, sizes] = line.split(": ")
    dimentions = [int(x) for x in dimension.split("x")]
    sizes_list = [int(x) for x in sizes.split(" ")]
    return (dimentions, sizes_list)


def diagram_size(diagram):
    return sum([sum([1 if x == "#" else 0 for x in line]) for line in diagram])


def solve(input_file, verbose=False):
    with open(input_file, encoding="utf-8") as f:
        data = f.read().strip().replace("\r", "")
    blocks = data.split("\n\n")
    diagrams = [parse_blueprint(blueprint) for blueprint in blocks[:-1]]
    diagram_sizes = [diagram_size(diagram) for diagram in diagrams]
    inputs = [parse_line(line) for line in blocks[-1].splitlines()]

    ok = 0
    for problem in inputs:
        ((x, y), required_counts) = problem
        region_size = x * y
        occupied_size = sum([a * b for (a, b) in zip(diagram_sizes, required_counts)])
        if occupied_size <= region_size:
            ok += 1
    print(ok)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", nargs="?", default="sample.txt")
    parser.add_argument("-v", "--verbose", action="store_true")
    args = parser.parse_args()
    solve(args.input, verbose=args.verbose)


if __name__ == "__main__":
    main()
