#!/usr/bin/env python3
# Interiors are drawn in 3/4 view: floors seen from above, the north wall
# shows its front face (so screens and posters can be read).

import os, random
from PIL import Image, ImageDraw

from gen_intro import FONT, rgba, mix, shade

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..")
S = 40
COLS = 10

CYAN = (0, 235, 255)
PINK = (255, 40, 170)
YELLOW = (255, 225, 60)

T_FLOOR_A, T_FLOOR_B, T_WALL, T_WALL_FACE = 1, 2, 3, 5
T_SCREEN_L, T_SCREEN_M, T_SCREEN_R = 6, 7, 8
T_POSTER, T_NEON = 9, 10
T_COUNTER_L, T_COUNTER_M, T_COUNTER_R, T_COFFEE = 12, 13, 14, 15
T_TABLE, T_CHAIR_DOWN, T_CHAIR_UP, T_STOOL = 16, 17, 18, 19
T_PLANT, T_ARCADE, T_MAT, T_DOOR = 20, 21, 22, 23
T_WINDOW, T_POSTER_OS, T_DESK_PC, T_DESK_2ND, T_DESK_MUG = 24, 25, 26, 27, 28
T_BED_TOP, T_BED_BOTTOM, T_RUG, T_STAIRS_DOWN, T_STAIRS_UP = 29, 30, 31, 32, 33
T_SHELF, T_SOFA_L, T_SOFA_R, T_TV, T_FRAME = 34, 35, 36, 37, 38
T_WOOD_A, T_WOOD_B, T_DESK_CHAIR, T_FRIDGE = 39, 40, 41, 42
T_H_PLANT, T_H_TABLE, T_H_CHAIR_DOWN, T_H_CHAIR_UP, T_H_MAT = 43, 44, 45, 46, 47
ROWS = 5

SOLID = [T_WALL, T_WALL_FACE, T_SCREEN_L, T_SCREEN_M, T_SCREEN_R, T_POSTER, T_NEON,
         T_COUNTER_L, T_COUNTER_M, T_COUNTER_R, T_COFFEE, T_TABLE, T_PLANT, T_ARCADE,
         T_WINDOW, T_POSTER_OS, T_DESK_PC, T_DESK_2ND, T_DESK_MUG, T_BED_TOP, T_BED_BOTTOM,
         T_SHELF, T_SOFA_L, T_SOFA_R, T_TV, T_FRAME, T_FRIDGE, T_H_PLANT, T_H_TABLE]


def text(img, x, y, s, color, scale=1):
    d = ImageDraw.Draw(img)
    for ch in s.upper():
        g = FONT.get(ch, FONT[" "])
        for r, row in enumerate(g):
            for c, bit in enumerate(row):
                if bit == "1":
                    d.rectangle([x + c * scale, y + r * scale, x + c * scale + scale - 1, y + r * scale + scale - 1], fill=rgba(color))
        x += (len(g[0]) + 1) * scale


def floor(dark):
    img = Image.new("RGBA", (S, S), rgba((34, 26, 52) if dark else (42, 32, 64)))
    d = ImageDraw.Draw(img)
    d.rectangle([0, 0, S - 1, S - 1], outline=rgba((26, 20, 40)))
    rnd = random.Random(dark)
    for _ in range(18):
        img.putpixel((rnd.randrange(S), rnd.randrange(S)), rgba((50, 40, 76) if dark else (56, 44, 84)))
    if dark:
        d.point((S // 2, S // 2), fill=rgba((0, 140, 160)))
    return img


def wall():
    img = Image.new("RGBA", (S, S), rgba((16, 12, 26)))
    d = ImageDraw.Draw(img)
    for y in range(0, S, 8):
        d.line([(0, y), (S, y)], fill=rgba((22, 17, 36)))
    return img


def wall_face():
    img = Image.new("RGBA", (S, S), rgba((58, 36, 80)))
    d = ImageDraw.Draw(img)
    for y in range(0, S, 10):
        d.line([(0, y), (S, y)], fill=rgba((46, 28, 64)))
        off = 0 if (y // 10) % 2 == 0 else 10
        for x in range(off, S, 20):
            d.line([(x, y), (x, y + 9)], fill=rgba((46, 28, 64)))
    d.rectangle([0, S - 4, S - 1, S - 1], fill=rgba((20, 14, 30)))
    d.line([(0, S - 5), (S - 1, S - 5)], fill=rgba(PINK))
    return img


def screen(part):
    img = wall_face()
    d = ImageDraw.Draw(img)
    x0 = 4 if part == "L" else 0
    x1 = S - 5 if part == "R" else S - 1
    d.rectangle([x0, 3, x1, S - 8], fill=rgba((20, 18, 34)))
    d.line([(x0, 3), (x1, 3)], fill=rgba(CYAN))
    d.line([(x0, S - 8), (x1, S - 8)], fill=rgba(CYAN))
    if part == "L":
        d.line([(x0, 3), (x0, S - 8)], fill=rgba(CYAN))
        d.rectangle([x0 + 2, 5, x0 + 10, S - 10], fill=rgba((30, 26, 46)))
        for y in range(8, S - 12, 5):
            d.point((x0 + 5, y), fill=rgba((140, 130, 170)))
        text(img, x0 + 14, 6, "#", (130, 120, 160))
        text(img, x0 + 20, 6, "GEN", (230, 230, 240))
    if part == "R":
        d.line([(x1, 3), (x1, S - 8)], fill=rgba(CYAN))
    rnd = random.Random(part)
    cols = [(60, 220, 120), (255, 80, 90), (255, 160, 40), (60, 220, 250), (255, 220, 80)]
    for y in range(15, S - 11, 5):
        start = x0 + (14 if part == "L" else 2)
        length = rnd.randrange(8, 26)
        d.rectangle([start, y, start + 2, y + 2], fill=rgba(rnd.choice(cols)))
        d.line([(start + 5, y + 1), (min(x1 - 2, start + 5 + length), y + 1)], fill=rgba((170, 165, 200)))
    return img


def poster():
    img = wall_face()
    d = ImageDraw.Draw(img)
    d.rectangle([4, 4, S - 5, S - 9], fill=rgba((235, 235, 240)))
    d.rectangle([4, 4, S - 5, 11], fill=rgba((0, 70, 140)))
    text(img, 7, 5, "SUNY", (255, 255, 255))
    text(img, 7, 14, "V32", (0, 70, 140))
    d.line([(7, 22), (32, 22)], fill=rgba((120, 120, 130)))
    d.line([(7, 25), (28, 25)], fill=rgba((120, 120, 130)))
    d.line([(8, 28), (30, 29)], fill=rgba((230, 40, 60)))
    return img


def neon_sign():
    img = wall_face()
    d = ImageDraw.Draw(img)
    d.rectangle([2, 6, S - 3, S - 12], fill=rgba((24, 16, 36)))
    text(img, 5, 10, "CAFE", PINK)
    for x in range(5, 34):
        for y in range(9, 17):
            if img.getpixel((x, y))[:3] == PINK:
                d.point((x, y + 9), fill=rgba(PINK, 60))
    return img


def counter(kind):
    img = floor(False)
    d = ImageDraw.Draw(img)
    x0 = 3 if kind == "L" else 0
    x1 = S - 4 if kind == "R" else S - 1
    d.rectangle([x0, 4, x1, S - 6], fill=rgba((90, 50, 30)))
    d.rectangle([x0, 4, x1, 12], fill=rgba((150, 90, 50)))
    d.line([(x0, 13), (x1, 13)], fill=rgba((60, 30, 20)))
    d.line([(x0, S - 6), (x1, S - 6)], fill=rgba(YELLOW))
    if kind == "C":
        d.rectangle([12, 0, 28, 14], fill=rgba((180, 185, 200)), outline=rgba((80, 80, 95)))
        d.rectangle([15, 3, 25, 7], fill=rgba((30, 30, 40)))
        d.point((17, 5), fill=rgba((255, 60, 60)))
        d.point((20, 5), fill=rgba((60, 255, 120)))
        d.rectangle([17, 10, 22, 13], fill=rgba((240, 240, 245)))
    return img


def table(base=None):
    img = base() if base else floor(True)
    d = ImageDraw.Draw(img)
    d.ellipse([4, 6, S - 5, S - 3], fill=rgba((10, 8, 16), 120))
    d.ellipse([3, 3, S - 5, S - 6], fill=rgba((120, 70, 40)), outline=rgba((70, 40, 20)))
    d.ellipse([7, 6, S - 9, S - 12], outline=rgba((150, 95, 55)))
    d.rectangle([9, 11, 22, 20], fill=rgba((150, 155, 170)), outline=rgba((60, 60, 75)))
    d.rectangle([10, 12, 21, 16], fill=rgba((20, 60, 90)))
    d.line([(11, 13), (18, 13)], fill=rgba(CYAN))
    d.line([(11, 15), (16, 15)], fill=rgba((170, 255, 120)))
    d.ellipse([25, 17, 30, 22], fill=rgba((240, 240, 245)))
    d.point((27, 19), fill=rgba((90, 50, 30)))
    d.point((28, 19), fill=rgba((90, 50, 30)))
    return img


def chair(facing_down, base=None):
    img = base() if base else floor(False)
    d = ImageDraw.Draw(img)
    d.rounded_rectangle([10, 10, 29, 29], radius=3, fill=rgba((0, 120, 140)), outline=rgba((0, 70, 90)))
    if facing_down:
        d.rectangle([10, 8, 29, 13], fill=rgba((0, 90, 110)))
    else:
        d.rectangle([10, 26, 29, 31], fill=rgba((0, 90, 110)))
    return img


def stool():
    img = floor(True)
    d = ImageDraw.Draw(img)
    d.ellipse([12, 12, 27, 27], fill=rgba((200, 40, 120)), outline=rgba((120, 20, 70)))
    d.ellipse([15, 15, 20, 20], fill=rgba((240, 110, 190)))
    return img


def plant(base=None):
    img = base() if base else floor(False)
    d = ImageDraw.Draw(img)
    d.ellipse([10, 14, 30, 34], fill=rgba((90, 60, 40)))
    for cx, cy in ((20, 12), (13, 18), (27, 18), (17, 24), (24, 25), (20, 19)):
        d.ellipse([cx - 7, cy - 7, cx + 7, cy + 7], fill=rgba((30, 150, 80)), outline=rgba((20, 100, 55)))
    d.point((20, 12), fill=rgba((120, 255, 150)))
    return img


def arcade():
    img = floor(False)
    d = ImageDraw.Draw(img)
    d.rectangle([4, 2, S - 5, S - 3], fill=rgba((40, 20, 70)), outline=rgba(PINK))
    d.rectangle([8, 5, S - 9, 18], fill=rgba((10, 10, 20)), outline=rgba(CYAN))
    for x in range(10, S - 10, 4):
        d.point((x, 11), fill=rgba(YELLOW))
    d.ellipse([11, 23, 17, 29], fill=rgba((255, 60, 60)))
    d.ellipse([22, 23, 27, 28], fill=rgba(CYAN))
    d.ellipse([28, 25, 32, 29], fill=rgba(YELLOW))
    return img


def mat(base=None):
    img = base() if base else floor(True)
    d = ImageDraw.Draw(img)
    d.rectangle([2, 6, S - 3, S - 3], fill=rgba((120, 20, 70)), outline=rgba(PINK))
    return img


def door():
    img = Image.new("RGBA", (S, S), rgba((20, 16, 34)))
    d = ImageDraw.Draw(img)
    for y in range(S):
        d.line([(0, y), (S, y)], fill=rgba(mix((60, 220, 255), (20, 16, 34), y / S)))
    d.line([(0, 0), (S, 0)], fill=rgba((255, 255, 255)))
    return img


def wood(dark):
    base = (74, 46, 40) if dark else (84, 52, 44)
    img = Image.new("RGBA", (S, S), rgba(base))
    d = ImageDraw.Draw(img)
    for y in range(0, S, 10):
        d.line([(0, y), (S, y)], fill=rgba(shade(base, 0.7)))
        off = 8 if (y // 10) % 2 == 0 else 26
        d.line([(off, y), (off, y + 9)], fill=rgba(shade(base, 0.7)))
    rnd = random.Random(7 + dark)
    for _ in range(14):
        x, y = rnd.randrange(S), rnd.randrange(S)
        img.putpixel((x, y), rgba(shade(base, 1.15)))
    return img


def window():
    img = wall_face()
    d = ImageDraw.Draw(img)
    d.rectangle([3, 3, S - 4, S - 9], fill=rgba((10, 8, 30)), outline=rgba((120, 100, 150)))
    rnd = random.Random(3)
    for x in range(5, S - 5, 5):
        h = rnd.randrange(8, 22)
        d.rectangle([x, S - 10 - h, x + 3, S - 10], fill=rgba((24, 18, 46)))
        for wy in range(S - 8 - h, S - 11, 3):
            if rnd.random() < 0.4:
                d.point((x + 1, wy), fill=rgba(rnd.choice([YELLOW, CYAN, PINK])))
    d.point((10, 7), fill=rgba((255, 255, 255)))
    d.point((29, 9), fill=rgba((200, 200, 255)))
    d.line([(S // 2, 3), (S // 2, S - 9)], fill=rgba((120, 100, 150)))
    return img


def poster_os():
    img = wall_face()
    d = ImageDraw.Draw(img)
    d.rectangle([5, 3, S - 6, S - 8], fill=rgba((10, 60, 120)), outline=rgba((230, 230, 240)))
    d.rectangle([6, 20, S - 7, S - 9], fill=rgba((20, 110, 170)))
    text(img, 8, 5, "OCEAN", (255, 255, 255))
    text(img, 8, 11, "STORM", YELLOW)
    d.polygon([(14, 24), (26, 24), (24, 27), (16, 27)], fill=rgba((90, 90, 100)))
    d.line([(20, 21), (20, 24)], fill=rgba((200, 200, 210)))
    d.line([(12, 21), (28, 21)], fill=rgba((200, 200, 210)))
    return img


def desk(kind):
    img = wood(False)
    d = ImageDraw.Draw(img)
    x0 = 3 if kind == "L" else 0
    x1 = S - 4 if kind == "R" else S - 1
    d.rectangle([x0, 14, x1, S - 5], fill=rgba((30, 26, 40)))
    d.rectangle([x0, 14, x1, 20], fill=rgba((60, 54, 78)))
    d.line([(x0, S - 5), (x1, S - 5)], fill=rgba(CYAN))
    if kind == "M":
        d.rectangle([4, 0, S - 5, 16], fill=rgba((14, 12, 22)), outline=rgba((90, 90, 110)))
        d.rectangle([6, 2, S - 7, 14], fill=rgba((8, 10, 26)))
        for bx, by in ((9, 5), (18, 5), (27, 5), (9, 10), (22, 10)):
            d.rectangle([bx, by, bx + 3, by + 1], fill=rgba(CYAN))
        d.point((21, 7), fill=rgba(YELLOW))
        d.point((15, 8), fill=rgba((255, 120, 40)))
        d.rectangle([17, 16, 22, 18], fill=rgba((70, 70, 90)))
        d.rectangle([8, 22, 31, 27], fill=rgba((20, 18, 28)), outline=rgba((90, 80, 120)))
        for kx in range(10, 30, 3):
            d.point((kx, 24), fill=rgba(PINK))
    elif kind == "L":
        d.rectangle([6, 4, 24, 16], fill=rgba((14, 12, 22)), outline=rgba((90, 90, 110)))
        d.rectangle([8, 6, 22, 14], fill=rgba((12, 30, 20)))
        for ly in range(7, 14, 2):
            d.line([(9, ly), (9 + (ly * 7) % 12, ly)], fill=rgba((80, 255, 120)))
        d.rectangle([28, 8, 33, 18], fill=rgba((40, 36, 52)), outline=rgba((20, 18, 28)))
        d.ellipse([29, 10, 32, 13], fill=rgba((90, 90, 110)))
    else:
        d.rectangle([6, 10, 22, 18], fill=rgba((230, 230, 235)), outline=rgba((120, 120, 130)))
        d.rectangle([8, 12, 20, 14], fill=rgba((20, 20, 26)))
        text(img, 8, 15, "V32", (20, 20, 26))
        d.ellipse([25, 10, 33, 18], fill=rgba((240, 240, 245)), outline=rgba((120, 120, 130)))
        d.ellipse([27, 12, 31, 16], fill=rgba((90, 50, 30)))
    return img


def desk_chair():
    img = wood(True)
    d = ImageDraw.Draw(img)
    d.ellipse([8, 10, 31, 33], fill=rgba((10, 8, 16), 110))
    d.rounded_rectangle([10, 8, 29, 29], radius=4, fill=rgba((200, 30, 90)), outline=rgba((110, 10, 50)))
    d.rectangle([10, 26, 29, 31], fill=rgba((40, 30, 50)))
    return img


def bed(top):
    img = wood(False)
    d = ImageDraw.Draw(img)
    if top:
        d.rectangle([3, 2, S - 4, S - 1], fill=rgba((230, 225, 235)), outline=rgba((120, 110, 140)))
        d.rounded_rectangle([7, 6, S - 8, 18], radius=4, fill=rgba((250, 250, 255)), outline=rgba((170, 160, 190)))
        d.rectangle([3, 26, S - 4, S - 1], fill=rgba((235, 95, 20)))
        d.line([(3, 26), (S - 4, 26)], fill=rgba((255, 210, 60)))
    else:
        d.rectangle([3, 0, S - 4, S - 4], fill=rgba((235, 95, 20)), outline=rgba((140, 50, 10)))
        d.line([(3, 0), (3, S - 4)], fill=rgba((140, 50, 10)))
        for y in range(6, S - 6, 8):
            d.line([(6, y), (S - 7, y + 2)], fill=rgba((200, 70, 10)))
        d.rectangle([3, S - 7, S - 4, S - 4], fill=rgba((120, 80, 60)))
    return img


def rug():
    img = wood(True)
    d = ImageDraw.Draw(img)
    d.rectangle([0, 0, S - 1, S - 1], fill=rgba((60, 24, 90)))
    for y in range(0, S, 8):
        for x in range((y // 8) % 2 * 4, S, 8):
            d.point((x, y), fill=rgba((100, 50, 140)))
    d.point((S // 2, S // 2), fill=rgba(CYAN))
    return img


def stairs(down):
    img = Image.new("RGBA", (S, S), rgba((20, 16, 30)))
    d = ImageDraw.Draw(img)
    steps = 5
    for k in range(steps):
        t = k / (steps - 1)
        if not down:
            t = 1 - t
        col = mix((96, 64, 54), (26, 18, 26), t)
        y0 = k * S // steps
        d.rectangle([2, y0, S - 3, y0 + S // steps - 1], fill=rgba(col))
        d.line([(2, y0), (S - 3, y0)], fill=rgba(shade(col, 1.3)))
    d.line([(1, 0), (1, S)], fill=rgba((60, 50, 70)))
    d.line([(S - 2, 0), (S - 2, S)], fill=rgba((60, 50, 70)))
    cx = S // 2
    if down:
        d.polygon([(cx - 5, 16), (cx + 5, 16), (cx, 22)], fill=rgba(YELLOW))
    else:
        d.polygon([(cx - 5, 22), (cx + 5, 22), (cx, 16)], fill=rgba(YELLOW))
    return img


def shelf():
    img = wood(False)
    d = ImageDraw.Draw(img)
    d.rectangle([2, 0, S - 3, S - 6], fill=rgba((50, 34, 30)), outline=rgba((30, 20, 18)))
    rnd = random.Random(11)
    for row_y in (3, 17):
        x = 5
        while x < S - 8:
            w = rnd.choice([3, 4, 5])
            d.rectangle([x, row_y, x + w - 1, row_y + 11], fill=rgba(rnd.choice([CYAN, PINK, YELLOW, (120, 255, 120), (240, 240, 245)])))
            x += w + 1
        d.line([(3, row_y + 12), (S - 4, row_y + 12)], fill=rgba((80, 56, 48)))
    return img


def sofa(left):
    img = wood(True)
    d = ImageDraw.Draw(img)
    x0 = 3 if left else 0
    x1 = S - 1 if left else S - 4
    d.rectangle([x0, 6, x1, S - 4], fill=rgba((0, 110, 130)), outline=rgba((0, 60, 80)))
    d.rectangle([x0, 26, x1, S - 4], fill=rgba((0, 80, 100)))
    if left:
        d.rectangle([x0, 6, x0 + 5, S - 4], fill=rgba((0, 80, 100)))
    else:
        d.rectangle([x1 - 5, 6, x1, S - 4], fill=rgba((0, 80, 100)))
    d.rectangle([x0 + 8, 10, x1 - 8, 22], fill=rgba((0, 140, 160)))
    return img


def tv():
    img = wall_face()
    d = ImageDraw.Draw(img)
    d.rectangle([2, 4, S - 3, S - 10], fill=rgba((10, 10, 16)), outline=rgba((70, 70, 90)))
    d.rectangle([4, 6, S - 5, S - 12], fill=rgba((20, 30, 70)))
    text(img, 7, 9, "NEWS", YELLOW)
    d.line([(6, 17), (30, 17)], fill=rgba((230, 60, 70)))
    d.line([(6, 20), (24, 20)], fill=rgba((200, 200, 220)))
    return img


def frame():
    img = wall_face()
    d = ImageDraw.Draw(img)
    d.rectangle([9, 5, S - 10, S - 11], fill=rgba((200, 170, 90)))
    d.rectangle([11, 7, S - 12, S - 13], fill=rgba((150, 200, 230)))
    d.ellipse([14, 12, 19, 17], fill=rgba((232, 180, 136)))
    d.ellipse([21, 11, 26, 16], fill=rgba((232, 180, 136)))
    d.rectangle([13, 17, 20, S - 13], fill=rgba((235, 95, 20)))
    d.rectangle([21, 16, 27, S - 13], fill=rgba((110, 40, 160)))
    return img


def fridge():
    img = wood(False)
    d = ImageDraw.Draw(img)
    d.rectangle([4, 0, S - 5, S - 4], fill=rgba((220, 225, 235)), outline=rgba((120, 125, 140)))
    d.line([(4, 14), (S - 5, 14)], fill=rgba((120, 125, 140)))
    d.rectangle([S - 10, 4, S - 8, 11], fill=rgba((120, 125, 140)))
    d.rectangle([S - 10, 18, S - 8, 28], fill=rgba((120, 125, 140)))
    d.rectangle([9, 18, 16, 24], fill=rgba((255, 225, 60)))
    return img


def build_tileset():
    tiles = {
        T_FLOOR_A: floor(False), T_FLOOR_B: floor(True), T_WALL: wall(), T_WALL_FACE: wall_face(),
        T_SCREEN_L: screen("L"), T_SCREEN_M: screen("M"), T_SCREEN_R: screen("R"),
        T_POSTER: poster(), T_NEON: neon_sign(),
        T_COUNTER_L: counter("L"), T_COUNTER_M: counter("M"), T_COUNTER_R: counter("R"), T_COFFEE: counter("C"),
        T_TABLE: table(), T_CHAIR_DOWN: chair(True), T_CHAIR_UP: chair(False), T_STOOL: stool(),
        T_PLANT: plant(), T_ARCADE: arcade(), T_MAT: mat(), T_DOOR: door(),
        T_WINDOW: window(), T_POSTER_OS: poster_os(),
        T_DESK_PC: desk("M"), T_DESK_2ND: desk("L"), T_DESK_MUG: desk("R"),
        T_BED_TOP: bed(True), T_BED_BOTTOM: bed(False), T_RUG: rug(),
        T_STAIRS_DOWN: stairs(True), T_STAIRS_UP: stairs(False), T_SHELF: shelf(),
        T_SOFA_L: sofa(True), T_SOFA_R: sofa(False), T_TV: tv(), T_FRAME: frame(),
        T_WOOD_A: wood(False), T_WOOD_B: wood(True), T_DESK_CHAIR: desk_chair(), T_FRIDGE: fridge(),
        T_H_PLANT: plant(lambda: wood(False)), T_H_TABLE: table(lambda: wood(True)),
        T_H_CHAIR_DOWN: chair(True, lambda: wood(False)), T_H_CHAIR_UP: chair(False, lambda: wood(False)),
        T_H_MAT: mat(lambda: wood(True)),
    }
    sheet = Image.new("RGBA", (COLS * S, ROWS * S), (0, 0, 0, 0))
    for idx, img in tiles.items():
        sheet.paste(img, ((idx % COLS) * S, (idx // COLS) * S))
    return sheet


CAFE = [
    "WWWWWWWWWWWWWWWW",
    "WFF[===]FPFNFFFW",
    "W<............<W",
    "W..v..v..lmcmrAW",
    "W..T..T...s.s..W",
    "W..^..^.......AW",
    "W..v..v........W",
    "W..T..T........W",
    "W..^..^MM......W",
    "WWWWWWWDDWWWWWWW",
]

# Chip's house. Upstairs: his room. Downstairs: kitchen, sofa, front door.
HOME_UP = [
    "WWWWWWWWWWWWWW",
    "WFFwwFOFFFwwFW",
    "W.qpo..xx...bW",
    "W..h........BW",
    "W....~~~~....W",
    "W....~~~~...SW",
    "W<..........SW",
    "WWWWWWWWWWWWWW",
]

HOME_DOWN = [
    "WWWWWWWWWWWWWW",
    "WFFVFFGFFFFFFW",
    "Wflmcr......UW",
    "W...........UW",
    "W..LR....v...W",
    "W........T...W",
    "W<....MM.^...W",
    "WWWWWWDDWWWWWW",
]

HOME_LEGEND = {
    "w": T_WINDOW, "O": T_POSTER_OS, "q": T_DESK_2ND, "p": T_DESK_PC, "o": T_DESK_MUG,
    "h": T_DESK_CHAIR, "x": T_SHELF, "b": T_BED_TOP, "B": T_BED_BOTTOM, "~": T_RUG,
    "<": T_H_PLANT, "T": T_H_TABLE, "v": T_H_CHAIR_DOWN, "^": T_H_CHAIR_UP, "M": T_H_MAT,
    "S": T_STAIRS_DOWN, "U": T_STAIRS_UP, "V": T_TV, "G": T_FRAME, "f": T_FRIDGE,
    "L": T_SOFA_L, "R": T_SOFA_R,
}

LEGEND = {
    "W": T_WALL, "F": T_WALL_FACE, "[": T_SCREEN_L, "=": T_SCREEN_M, "]": T_SCREEN_R,
    "P": T_POSTER, "N": T_NEON, "l": T_COUNTER_L, "m": T_COUNTER_M, "r": T_COUNTER_R, "c": T_COFFEE,
    "T": T_TABLE, "v": T_CHAIR_DOWN, "^": T_CHAIR_UP, "s": T_STOOL, "<": T_PLANT, "A": T_ARCADE,
    "M": T_MAT, "D": T_DOOR,
}


def build_map(rows, wooden=False):
    grid = []
    for y, row in enumerate(rows):
        out = []
        for x, ch in enumerate(row):
            if ch == "." and wooden:
                out.append(T_WOOD_A if (x + y) % 2 == 0 else T_WOOD_B)
            elif ch == ".":
                out.append(T_FLOOR_A if (x + y) % 2 == 0 else T_FLOOR_B)
            elif wooden and ch in HOME_LEGEND:
                out.append(HOME_LEGEND[ch])
            else:
                out.append(LEGEND[ch])
        grid.append(out)
    return grid


def write_tileset(sheet):
    tex = os.path.join(ROOT, "assets", "textures", "TextureInterior.png")
    sheet.save(tex)
    sheet.save(os.path.join(ROOT, "assets", "maps", "TileSetInterior.png"))
    open(os.path.join(ROOT, "assets", "maps", "TileSetInterior.tsx"), "w").write(
        '<?xml version="1.0" encoding="UTF-8"?>\n'
        f'<tileset version="1.1" tiledversion="2018.09.12" name="TileSetInterior" tilewidth="{S}" tileheight="{S}" tilecount="{COLS * ROWS}" columns="{COLS}">\n'
        f' <image source="TileSetInterior.png" width="{COLS * S}" height="{ROWS * S}"/>\n'
        '</tileset>\n')


def write_map(sheet, grid, name):
    H, W = len(grid), len(grid[0])
    data = "\n".join(",".join(str(v + 1 if v else 0) for v in row) + ("," if i < H - 1 else "") for i, row in enumerate(grid))
    open(os.path.join(ROOT, "assets", "maps", f"TileMap{name}.tmx"), "w").write(f'''<?xml version="1.0" encoding="UTF-8"?>
<map version="1.1" tiledversion="2018.09.12" orientation="orthogonal" renderorder="right-down" width="{W}" height="{H}" tilewidth="{S}" tileheight="{S}" infinite="0" nextlayerid="2" nextobjectid="1">
 <tileset firstgid="1" source="TileSetInterior.tsx"/>
 <layer id="1" name="Map{name}" width="{W}" height="{H}">
  <data encoding="csv">
{data}
</data>
 </layer>
</map>
''')
    preview = Image.new("RGBA", (W * S, H * S), (0, 0, 0, 255))
    for y, row in enumerate(grid):
        for x, v in enumerate(row):
            preview.alpha_composite(sheet.crop(((v % COLS) * S, (v // COLS) * S, (v % COLS) * S + S, (v // COLS) * S + S)), (x * S, y * S))
    preview.resize((W * S * 2, H * S * 2), Image.NEAREST).save(os.path.join(HERE, "out", f"_interior_{name.lower()}.png"))
    return W, H


def write_header(sizes):
    h = [
        "// GENERATED BY artgen/gen_interior.py - DO NOT EDIT BY HAND\n",
        "    #ifndef INTERIORDATA_HPP\n    #define INTERIORDATA_HPP\n\n",
        f"#define INTERIOR_TILE {S}\n",
    ]
    for name, (W, H) in sizes:
        h.append(f"#define {name}TilesX {W}\n#define {name}TilesY {H}\n")
    h += [
        f"#define InteriorTileStairsDown {T_STAIRS_DOWN}\n#define InteriorTileStairsUp {T_STAIRS_UP}\n",
        f"#define NumInteriorTileTypes {COLS * ROWS}\n\n",
        "extern bool[ NumInteriorTileTypes ] InteriorTileSolid;\n",
        "\n    #endif\n",
    ]
    open(os.path.join(ROOT, "include", "core", "InteriorData.h"), "w").write("".join(h))
    solid = ", ".join("true" if i in SOLID else "false" for i in range(COLS * ROWS))
    open(os.path.join(ROOT, "src", "core", "InteriorData.c"), "w").write(
        "// GENERATED BY artgen/gen_interior.py - DO NOT EDIT BY HAND\n"
        '    #include "../../include/core/InteriorData.h"\n\n'
        f"bool[ NumInteriorTileTypes ] InteriorTileSolid = {{ {solid} }};\n")


if __name__ == "__main__":
    sheet = build_tileset()
    write_tileset(sheet)
    sizes = [
        ("Cafe", write_map(sheet, build_map(CAFE), "Cafe")),
        ("HomeUp", write_map(sheet, build_map(HOME_UP, True), "HomeUp")),
        ("HomeDown", write_map(sheet, build_map(HOME_DOWN, True), "HomeDown")),
    ]
    write_header(sizes)
    print("OK", sizes)
