#! /bin/env python3
"""Only compatible with ppm generated by GIMP!"""

map_ppm_path = "map.ppm"
map_vertices_path = "map.vertices"

def calculate_height(r, g, b):
    avg = (r+g+b)/3
    return int(avg / 255 * 9 + 1)

id_cube = (
    1, 0, 1, # up
    0, 0, 1,
    0, 1, 1,

    1, 0, 1, # up
    1, 1, 1,
    0, 1, 1,

    1, 0, 1, # front
    1, 0, 0,
    0, 0, 0,

    1, 0, 1, # front
    0, 0, 1,
    0, 0, 0,

    0, 0, 1, # left
    0, 0, 0,
    0, 1, 0,

    0, 0, 1, # left
    0, 1, 1,
    0, 1, 0,

    0, 1, 1, # back
    0, 1, 0,
    1, 1, 0,

    0, 1, 1, # back
    1, 1, 1,
    1, 1, 0,

    1, 1, 1, # right
    1, 1, 0,
    1, 0, 0,

    1, 1, 1, # right
    1, 0, 1,
    1, 0, 0,

    1, 0, 0, # bottom
    0, 0, 0,
    0, 1, 0,

    1, 0, 0, # bottom
    1, 1, 0,
    0, 1, 0,
)

uv = (
    (0, 0),
)

def gen_cube(vertices_file, x, y, z):
    for i in range(int(len(id_cube) / 3)):
        vertices_file.write("%s %s %s\n" % (
            (id_cube[i*3+0] + x) / 100,
            (id_cube[i*3+1] + y) / 100,
            (id_cube[i*3+2] + z) / 100,
        ))

x = 0
y = 0
hs = []

with open(map_ppm_path) as f:
    f.readline()
    f.readline()
    xy = f.readline().strip().split(" ")
    x = int(xy[0])
    y = int(xy[1])
    f.readline()
    for _ in range(x):
        for _ in range(y):
            r = int(f.readline().strip())
            g = int(f.readline().strip())
            b = int(f.readline().strip())
            h = calculate_height(r, g, b)
            hs.append(h)

with open(map_vertices_path, "w") as vertices_file:
    for _x in range(x):
        for _y in range(y):
            for _z in range(hs[_x * y + _y]):
                gen_cube(vertices_file, _x - int(x/2), _y - int(y/2), _z)
