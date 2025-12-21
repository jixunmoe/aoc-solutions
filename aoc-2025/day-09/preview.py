import re
import sys

def build_svg(path: str, width: int, height: int, solution=None) -> str:
    if solution:
        x1, y1 = solution[0]
        x2, y2 = solution[1]
        w = abs(x2 - x1)
        h = abs(y2 - y1)
        rect_solution = f"""<rect 
            x="{min(x1,x2):.2f}" y="{min(y1,y2):.2f}" width="{w:.2f}" height="{h:.2f}" 
            fill="#0000ff33" stroke="#0000ffaa" stroke-width="1"
            />
        """
    return f'''<?xml version="1.0" standalone="no"?>
<svg width="{width}" height="{height}" xmlns="http://www.w3.org/2000/svg" version="1.1">
  <path d="{path}" fill="white" stroke="#00cc00" />
  {rect_solution if rect_solution else ''}
</svg>
'''

with open("input.txt", "r") as f:
    lines = f.read().strip().splitlines()

coords = [(int(x), int(y)) for line in lines for x, y in [line.split(",")]]

def get_delta(p1, p2):
    dx = p2[0] - p1[0]
    dy = p2[1] - p1[1]
    if dx == 0:
        return f"V {p2[1]:.2f}"
    elif dy == 0:
        return f"H {p2[0]:.2f}"
    else:
        return f"L {p2[0]:.2f} {p2[1]:.2f}"

scale = 1/200
coords = [(x * scale, y * scale) for x, y in coords]

solution = None
if solution_str := sys.argv[1] if len(sys.argv) > 1 else "":
    solution = [(int(m.group(1)), int(m.group(2))) for m in re.finditer(r'(\d+),\s*(\d+)', solution_str)]

min_x = min(x for x, y in coords)
min_y = min(y for x, y in coords)
max_x = max(x for x, y in coords)
max_y = max(y for x, y in coords)

width = max_x - min_x + 10
height = max_y - min_y + 10

coords = [(x - min_x + 5, y - min_y + 5) for x, y in coords]
solution = [(x * scale - min_x + 5, y * scale - min_y + 5) for x, y in solution] if solution else None

path = f"M {coords[0][0]:.2f} {coords[0][1]:.2f} "+ " ".join(
    get_delta(coords[i], coords[i + 1]) for i in range(len(coords) - 1)
) + " Z"

svg_content = build_svg(path, width, height, solution)
with open("preview.svg", "w") as f:
    f.write(svg_content)