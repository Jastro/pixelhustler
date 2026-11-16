#!/usr/bin/env python3
# Carra: "the map was hand made in Tiled, tile by tile"
# Jastro: "and now a script makes it. Progress."
# Carra: "you asked me to write the script"
# Jastro: "delegation is also progress"

import os, random

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..")

# -------------------------------------------------------------------------
#   LAYOUT
#   A grid of lots separated by roads. "lot" widths/heights include the
#   lot edge tiles (the curb sits in the middle of them). Edge lots start
#   one tile outside the map so the city looks like it keeps going.
# -------------------------------------------------------------------------

COLS = [("lot", 5), ("road", 1), ("lot", 6), ("road", 2), ("lot", 6), ("road", 1), ("lot", 6), ("road", 1), ("lot", 5)]
ROWS = [("lot", 5), ("road", 1), ("lot", 5), ("road", 1), ("lot", 5), ("road", 1), ("lot", 5)]

STORY = {
    (1, 1): ("CARRA_HQ", "CARRA HQ"),
    (2, 1): ("CAFE", "DISCORD|CAFE"),
    (3, 1): ("VENUE", "VIRCON|WARS"),
    (1, 2): ("HOME", "CHIP|BUSTER"),
    (2, 2): ("PARKING", None),
    (3, 2): ("JOYZONE", "JOYZONE"),
}

ENTRANCE_FACADE = {"CAFE": 8, "JOYZONE": 9, "VENUE": 10, "HOME": 11}

SIGN_TILES = {
    "VIRCOBAN": 13, "PUZTRIX": 14, "BLOCKDUDE": 15, "ZNAX": 16, "WATERNET": 17, "RUBIDO": 18,
    "OCEAN|STORM": 19, "VITRIS": 23, "HAUNTY": 25, "JELLYFISH": 26, "FLAPPY": 27, "WORM": 28,
    "BLIPS": 29, "CARRA HQ": 33, "CHIP|BUSTER": 34, "V32": 35,
    "DISCORD|CAFE": 43, "JOYZONE": 44, "VIRCON|WARS": 45,
}
COMMUNITY_SIGNS = ["VIRCOBAN", "PUZTRIX", "BLOCKDUDE", "ZNAX", "WATERNET", "RUBIDO", "OCEAN|STORM",
                   "VITRIS", "HAUNTY", "JELLYFISH", "FLAPPY", "WORM", "BLIPS", "V32"]
DECOR_TILES = [40, 41, 42]

GROUND_ROAD, GROUND_VDASH, GROUND_HDASH = 1, 8, 9
ZEBRA_V, ZEBRA_H = 2, 3
ZEBRA_LOT_RIGHT, ZEBRA_LOT_LEFT, ZEBRA_LOT_BOTTOM, ZEBRA_LOT_TOP = 4, 5, 6, 7
LOT_9SLICE = {(0, 0): 10, (1, 0): 11, (2, 0): 12, (0, 1): 20, (1, 1): 21, (2, 1): 22, (0, 2): 30, (1, 2): 31, (2, 2): 32}
ROOF_9SLICE = {(0, 0): 36, (1, 0): 38, (2, 0): 37, (0, 1): 49, (1, 1): 24, (2, 1): 39, (0, 2): 46, (1, 2): 48, (2, 2): 47}

WALL_LEFT, WALL_RIGHT, WALL_TOP, WALL_BOTTOM = 1, 2, 4, 8


def spans(spec):
    out, pos = [], -1
    for kind, size in spec:
        out.append((kind, pos, pos + size - 1))
        pos += size
    return out, pos - 1


def build():
    rnd = random.Random(32)
    col_spans, W = spans(COLS)
    row_spans, H = spans(ROWS)

    lot_cols = [(a, b) for k, a, b in col_spans if k == "lot"]
    lot_rows = [(a, b) for k, a, b in row_spans if k == "lot"]
    vroads = [(a, b - a + 1) for k, a, b in col_spans if k == "road"]
    hroads = [(a, b - a + 1) for k, a, b in row_spans if k == "road"]

    ground = [[0] * W for _ in range(H)]
    roofs = [[0] * W for _ in range(H)]
    walls = [[0] * W for _ in range(H)]
    building_id = [[-1] * W for _ in range(H)]

    def inside(x, y):
        return 0 <= x < W and 0 <= y < H

    col_kind = {}
    for k, a, b in col_spans:
        for x in range(a, b + 1):
            col_kind[x] = k
    row_kind = {}
    for k, a, b in row_spans:
        for y in range(a, b + 1):
            row_kind[y] = k

    for y in range(H):
        for x in range(W):
            if col_kind[x] == "road" and row_kind[y] == "road":
                ground[y][x] = GROUND_ROAD
            elif col_kind[x] == "road":
                ground[y][x] = GROUND_VDASH
            elif row_kind[y] == "road":
                ground[y][x] = GROUND_HDASH

    lots = []
    for j, (y0, y1) in enumerate(lot_rows):
        for i, (x0, x1) in enumerate(lot_cols):
            lots.append((i, j, x0, y0, x1, y1))
            for y in range(y0, y1 + 1):
                for x in range(x0, x1 + 1):
                    if not inside(x, y):
                        continue
                    sx = 0 if x == x0 else (2 if x == x1 else 1)
                    sy = 0 if y == y0 else (2 if y == y1 else 1)
                    ground[y][x] = LOT_9SLICE[(sx, sy)]

    # buildings
    buildings = []
    entrances = {}
    for (i, j, x0, y0, x1, y1) in lots:
        story = STORY.get((i, j))
        if story and story[0] == "PARKING":
            continue
        fx0, fy0, fx1, fy1 = x0 + 1, y0 + 1, x1 - 1, y1 - 1
        bid = len(buildings)
        facade = rnd.choice([0, 1, 2, 3, 4, 5, 6, 7, 12, 13, 14, 15])
        buildings.append((fx0, fy0, fx1, fy1, facade))
        centers = []
        for y in range(fy0, fy1 + 1):
            for x in range(fx0, fx1 + 1):
                if not inside(x, y):
                    continue
                sx = 0 if x == fx0 else (2 if x == fx1 else 1)
                sy = 0 if y == fy0 else (2 if y == fy1 else 1)
                roofs[y][x] = ROOF_9SLICE[(sx, sy)] + 1
                building_id[y][x] = bid
                if (sx, sy) == (1, 1):
                    centers.append((x, y))
        if story:
            if centers:
                cx, cy = centers[0]
                roofs[cy][cx] = SIGN_TILES[story[1]] + 1
                for k, (x, y) in enumerate(centers[1:]):
                    roofs[y][x] = rnd.choice(DECOR_TILES) + 1
            if story[0] in ENTRANCE_FACADE:
                door_x = x0 + 3
                entrances[story[0]] = (door_x, y1, ENTRANCE_FACADE[story[0]])
        else:
            for x, y in centers:
                if rnd.random() < 0.6:
                    roofs[y][x] = SIGN_TILES[rnd.choice(COMMUNITY_SIGNS)] + 1
                elif rnd.random() < 0.6:
                    roofs[y][x] = rnd.choice(DECOR_TILES) + 1
    if "CARRA_HQ" not in entrances:
        pass

    # walls: flags on building tiles, facade texture on the neighbour tile
    for y in range(H):
        for x in range(W):
            bid = building_id[y][x]
            if bid < 0:
                continue
            facade = buildings[bid][4]
            flags = 0
            for flag, dx, dy in ((WALL_LEFT, -1, 0), (WALL_RIGHT, 1, 0), (WALL_TOP, 0, -1), (WALL_BOTTOM, 0, 1)):
                nx, ny = x + dx, y + dy
                if inside(nx, ny) and building_id[ny][nx] < 0:
                    flags |= flag
                    if walls[ny][nx] == 0:
                        walls[ny][nx] = 16 + facade
            walls[y][x] = flags
    for name, (dx, dy, facade) in entrances.items():
        walls[dy][dx] = 16 + facade

    # crosswalks between interior lots (mid block)
    crosswalks = []
    interior_cols = lot_cols[1:-1]
    interior_rows = lot_rows[1:-1]
    for (ax, ak) in vroads:
        left = [c for c in interior_cols if c[1] == ax - 1]
        right = [c for c in interior_cols if c[0] == ax + ak]
        if not left or not right:
            continue
        for (y0, y1) in interior_rows:
            r = y0 + 2
            ground[r][ax - 1] = ZEBRA_LOT_RIGHT
            for x in range(ax, ax + ak):
                ground[r][x] = ZEBRA_V
            ground[r][ax + ak] = ZEBRA_LOT_LEFT
            crosswalks.append(("V", ax, ak, r, left[0], right[0], (y0, y1)))
    for (by, bk) in hroads:
        up = [r for r in interior_rows if r[1] == by - 1]
        down = [r for r in interior_rows if r[0] == by + bk]
        if not up or not down:
            continue
        for (x0, x1) in interior_cols:
            c = x0 + 2
            ground[by - 1][c] = ZEBRA_LOT_BOTTOM
            for y in range(by, by + bk):
                ground[y][c] = ZEBRA_H
            ground[by + bk][c] = ZEBRA_LOT_TOP
            crosswalks.append(("H", by, bk, c, up[0], down[0], (x0, x1)))

    return dict(W=W, H=H, ground=ground, roofs=roofs, walls=walls, vroads=vroads, hroads=hroads,
                lots=lots, lot_cols=lot_cols, lot_rows=lot_rows, entrances=entrances, crosswalks=crosswalks,
                interior_cols=interior_cols, interior_rows=interior_rows)


# -------------------------------------------------------------------------
#   OUTPUT
# -------------------------------------------------------------------------

def csv(grid):
    return "\n".join(",".join(str(v) for v in row) + ("," if i < len(grid) - 1 else "") for i, row in enumerate(grid))


def write_tmx(city):
    W, H = city["W"], city["H"]
    ground_gids = [[v + 1 if v else 0 for v in row] for row in city["ground"]]
    tmx = f'''<?xml version="1.0" encoding="UTF-8"?>
<map version="1.1" tiledversion="2018.09.12" orientation="orthogonal" renderorder="right-down" width="{W}" height="{H}" tilewidth="80" tileheight="80" infinite="0" nextlayerid="6" nextobjectid="1">
 <tileset firstgid="1" source="TileSetCity.tsx"/>
 <layer id="2" name="MapGround" width="{W}" height="{H}">
  <data encoding="csv">
{csv(ground_gids)}
</data>
 </layer>
 <layer id="5" name="MapRoofs" width="{W}" height="{H}" visible="0">
  <data encoding="csv">
{csv(city["roofs"])}
</data>
 </layer>
</map>
'''
    open(os.path.join(ROOT, "assets", "maps", "TileMapCity.tmx"), "w").write(tmx)

    wall_gids = [[v + 1 if v else 0 for v in row] for row in city["walls"]]
    tmx = f'''<?xml version="1.0" encoding="UTF-8"?>
<map version="1.1" tiledversion="2018.09.12" orientation="orthogonal" renderorder="right-down" width="{W}" height="{H}" tilewidth="80" tileheight="80" infinite="0" nextlayerid="4" nextobjectid="1">
 <tileset firstgid="1" source="TileSetSides.tsx"/>
 <layer id="3" name="MapWalls" width="{W}" height="{H}">
  <data encoding="csv">
{csv(wall_gids)}
</data>
 </layer>
</map>
'''
    open(os.path.join(ROOT, "assets", "maps", "TileMapWalls.tmx"), "w").write(tmx)


def write_sides_tileset():
    from PIL import Image, ImageDraw
    img = Image.new("RGBA", (320, 640), (30, 26, 46, 255))
    d = ImageDraw.Draw(img)
    for gid in range(1, 33):
        v = gid - 1
        x, y = (v % 4) * 80, (v // 4) * 80
        d.rectangle([x, y, x + 79, y + 79], outline=(80, 70, 110, 255))
        if 1 <= v <= 15:
            if v & 1: d.rectangle([x, y, x + 5, y + 79], fill=(255, 40, 170, 255))
            if v & 2: d.rectangle([x + 74, y, x + 79, y + 79], fill=(255, 40, 170, 255))
            if v & 4: d.rectangle([x, y, x + 79, y + 5], fill=(255, 40, 170, 255))
            if v & 8: d.rectangle([x, y + 74, x + 79, y + 79], fill=(255, 40, 170, 255))
        elif v >= 16:
            d.rectangle([x + 10, y + 10, x + 69, y + 69], fill=(0, 120, 140, 255))
        d.text((x + 30, y + 32), str(v), fill=(255, 255, 255, 255))
    img.save(os.path.join(ROOT, "assets", "maps", "TileSetSides.png"))
    open(os.path.join(ROOT, "assets", "maps", "TileSetSides.tsx"), "w").write(
        '<?xml version="1.0" encoding="UTF-8"?>\n'
        '<tileset version="1.1" tiledversion="2018.09.12" name="TileSetSides" tilewidth="80" tileheight="80" tilecount="32" columns="4">\n'
        ' <image source="TileSetSides.png" width="320" height="640"/>\n'
        '</tileset>\n')


def write_city_data(city):
    W, H = city["W"], city["H"]
    vroads, hroads = city["vroads"], city["hroads"]
    lots = [(x0, y0, x1, y1) for (x0, x1) in city["interior_cols"] for (y0, y1) in city["interior_rows"]]

    def lot_index(xs, ys):
        return lots.index((xs[0], ys[0], xs[1], ys[1]))

    cw = []
    for c in city["crosswalks"]:
        if c[0] == "V":
            _, ax, ak, r, left, right, rows = c
            la = lot_index(left, rows)
            lb = lot_index(right, rows)
            y = r * 80 + 40
            cw.append(dict(x0=int((ax - 0.5) * 80), y0=r * 80 + 4, x1=int((ax + ak + 0.5) * 80), y1=r * 80 + 76,
                           lot_a=la, side_a=1, ax=int((ax - 0.75) * 80), ay=y,
                           lot_b=lb, side_b=3, bx=int((ax + ak + 0.75) * 80), by=y))
        else:
            _, by, bk, col, up, down, cols = c
            la = lot_index(cols, up)
            lb = lot_index(cols, down)
            x = col * 80 + 40
            cw.append(dict(x0=col * 80 + 4, y0=int((by - 0.5) * 80), x1=col * 80 + 76, y1=int((by + bk + 0.5) * 80),
                           lot_a=la, side_a=2, ax=x, ay=int((by - 0.75) * 80),
                           lot_b=lb, side_b=0, bx=x, by=int((by + bk + 0.75) * 80)))

    ent = city["entrances"]
    names = ["CAFE", "JOYZONE", "VENUE", "HOME"]

    def arr(ctype, name, values):
        return f"{ctype}[ {len(values)} ] {name} = {{ " + ", ".join(str(v) for v in values) + " };\n"

    h = []
    h.append("// *****************************************************************************\n")
    h.append("//  GENERATED BY artgen/gen_map.py - DO NOT EDIT BY HAND\n")
    h.append("//  Jastro edited it by hand once. The city lost a street.\n")
    h.append("// *****************************************************************************\n")
    h.append("    #ifndef CITYDATA_HPP\n    #define CITYDATA_HPP\n\n")
    h.append(f"#define MapTilesX {W}\n#define MapTilesY {H}\n\n")
    h.append(f"#define NumVRoads {len(vroads)}\n#define NumHRoads {len(hroads)}\n")
    h.append(f"#define NumIntersections {len(vroads) * len(hroads)}\n")
    h.append(f"#define NumCrosswalks {len(cw)}\n#define NumPedLots {len(lots)}\n\n")
    for i, n in enumerate(names):
        h.append(f"#define Entrance{n.capitalize()} {i}\n")
    h.append(f"#define NumEntrances {len(names)}\n\n")
    h.append("extern int[ NumVRoads ] VRoadX;\nextern int[ NumVRoads ] VRoadWidth;\n")
    h.append("extern int[ NumHRoads ] HRoadY;\nextern int[ NumHRoads ] HRoadHeight;\n")
    for f in ("X0", "Y0", "X1", "Y1", "LotA", "SideA", "AX", "AY", "LotB", "SideB", "BX", "BY"):
        h.append(f"extern int[ NumCrosswalks ] Crosswalk{f};\n")
    for f in ("X0", "Y0", "X1", "Y1"):
        h.append(f"extern int[ NumPedLots ] PedLot{f};\n")
    h.append("extern int[ NumEntrances ] EntranceX;\nextern int[ NumEntrances ] EntranceY;\n")
    h.append("extern int[ 4 ] ParkingX;\nextern int[ 4 ] ParkingY;\n")
    h.append("\n    #endif\n")
    open(os.path.join(ROOT, "include", "core", "CityData.h"), "w").write("".join(h))

    c = []
    c.append("// GENERATED BY artgen/gen_map.py - DO NOT EDIT BY HAND\n")
    c.append('    #include "../../include/core/CityData.h"\n\n')
    c.append(arr("int", "VRoadX", [a for a, k in vroads]))
    c.append(arr("int", "VRoadWidth", [k for a, k in vroads]))
    c.append(arr("int", "HRoadY", [a for a, k in hroads]))
    c.append(arr("int", "HRoadHeight", [k for a, k in hroads]))
    keys = [("X0", "x0"), ("Y0", "y0"), ("X1", "x1"), ("Y1", "y1"), ("LotA", "lot_a"), ("SideA", "side_a"),
            ("AX", "ax"), ("AY", "ay"), ("LotB", "lot_b"), ("SideB", "side_b"), ("BX", "bx"), ("BY", "by")]
    for f, k in keys:
        c.append(arr("int", f"Crosswalk{f}", [w[k] for w in cw]))
    c.append(arr("int", "PedLotX0", [int((x0 + 0.75) * 80) for x0, y0, x1, y1 in lots]))
    c.append(arr("int", "PedLotY0", [int((y0 + 0.75) * 80) for x0, y0, x1, y1 in lots]))
    c.append(arr("int", "PedLotX1", [int((x1 + 0.25) * 80) for x0, y0, x1, y1 in lots]))
    c.append(arr("int", "PedLotY1", [int((y1 + 0.25) * 80) for x0, y0, x1, y1 in lots]))
    c.append(arr("int", "EntranceX", [ent[n][0] * 80 + 40 for n in names]))
    c.append(arr("int", "EntranceY", [ent[n][1] * 80 + 22 for n in names]))
    px0, py0 = [(x0, y0) for (i, j, x0, y0, x1, y1) in city["lots"] if STORY.get((i, j), ("",))[0] == "PARKING"][0]
    c.append(arr("int", "ParkingX", [int((px0 + 1.5 + k * 1.1) * 80) for k in range(4)]))
    c.append(arr("int", "ParkingY", [int((py0 + 2.5) * 80)] * 4))
    open(os.path.join(ROOT, "src", "core", "CityData.c"), "w").write("".join(c))


if __name__ == "__main__":
    city = build()
    write_tmx(city)
    write_sides_tileset()
    write_city_data(city)
    print("city", city["W"], "x", city["H"], "entrances", city["entrances"], "crosswalks", len(city["crosswalks"]))
