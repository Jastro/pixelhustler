#!/usr/bin/env python3
# Jastro: "the city should look like Vircon32"
# Carra: "Vircon32 is a virtual console. It doesn't look like anything."
# Jastro: "then it looks like a motherboard. With neon. At night."
# Carra: "motherboards don't have night"
# Jastro: "this one does"

import os, math, random
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..")
T = 80

ASPHALT = (22, 19, 38)
MARK = (215, 250, 255)
MARK_GLOW = (0, 235, 255)
SIDEWALK = (46, 34, 70)
SIDEWALK_GRID = (35, 26, 55)
TRACE = (72, 58, 120)
VIA = (215, 180, 70)
CURB_NEON = (255, 40, 170)
CURB_DARK = (110, 15, 70)
LOT_NEON = (0, 235, 255)

CHIP_BODY = (26, 24, 42)
CHIP_GRID = (36, 33, 56)
CHIP_EDGE = (12, 10, 22)
PIN = (175, 180, 198)
PIN_HI = (235, 238, 250)
PIN_LO = (95, 98, 115)
CHIP_RING = (0, 235, 255)

NEONS = [(255, 40, 170), (0, 235, 255), (255, 225, 60), (170, 255, 0), (255, 140, 0), (190, 90, 255)]

ORIGINAL_CLASSES = {
    (98, 98, 98): "road",
    (118, 91, 83): "sidewalk",
    (212, 212, 212): "mark",
    (156, 113, 99): "curb",
    (80, 35, 21): "curb_dark",
    (165, 153, 85): "lot",
}

FONT = {
    "A": ["010", "101", "111", "101", "101"], "B": ["110", "101", "110", "101", "110"],
    "C": ["011", "100", "100", "100", "011"], "D": ["110", "101", "101", "101", "110"],
    "E": ["111", "100", "110", "100", "111"], "F": ["111", "100", "110", "100", "100"],
    "G": ["011", "100", "101", "101", "011"], "H": ["101", "101", "111", "101", "101"],
    "I": ["111", "010", "010", "010", "111"], "J": ["001", "001", "001", "101", "010"],
    "K": ["101", "101", "110", "101", "101"], "L": ["100", "100", "100", "100", "111"],
    "M": ["101", "111", "101", "101", "101"], "N": ["110", "101", "101", "101", "101"],
    "O": ["010", "101", "101", "101", "010"], "P": ["110", "101", "110", "100", "100"],
    "Q": ["010", "101", "101", "110", "011"], "R": ["110", "101", "110", "101", "101"],
    "S": ["011", "100", "010", "001", "110"], "T": ["111", "010", "010", "010", "010"],
    "U": ["101", "101", "101", "101", "111"], "V": ["101", "101", "101", "101", "010"],
    "W": ["101", "101", "111", "111", "101"], "X": ["101", "101", "010", "101", "101"],
    "Y": ["101", "101", "010", "010", "010"], "Z": ["111", "001", "010", "100", "111"],
    "0": ["111", "101", "101", "101", "111"], "1": ["010", "110", "010", "010", "111"],
    "2": ["110", "001", "010", "100", "111"], "3": ["110", "001", "010", "001", "110"],
    "4": ["101", "101", "111", "001", "001"], "5": ["111", "100", "110", "001", "110"],
    "6": ["011", "100", "111", "101", "111"], "7": ["111", "001", "010", "010", "010"],
    "8": ["111", "101", "111", "101", "111"], "9": ["111", "101", "111", "001", "110"],
    " ": ["000", "000", "000", "000", "000"], "-": ["000", "000", "111", "000", "000"],
}

ICONS = {
    "ghost": [
        "0011111100", "0111111110", "1111111111", "1100110011", "1100110011",
        "1111111111", "1111111111", "1111111111", "1101101101", "1001001001"],
    "tetromino": [
        "0000000000", "0000000000", "1111111111", "1111111111", "1111111111",
        "0001111000", "0001111000", "0001111000", "0000000000", "0000000000"],
    "bird": [
        "0000111000", "0001111100", "0011110110", "1111111111", "1111111110",
        "0111111100", "0011111000", "0001110000", "0000000000", "0000000000"],
    "cat": [
        "1000000001", "1100000011", "1111111111", "1111111111", "1101111011",
        "1111111111", "1111001111", "0111111110", "0011111100", "0000000000"],
    "joystick": [
        "0000110000", "0001111000", "0001111000", "0000110000", "0000110000",
        "0000110000", "0111111110", "1111111111", "1111111111", "0111111110"],
    "gamepad": [
        "0000000000", "0111111110", "1111111111", "1011111101", "0001111000",
        "1011110101", "1111111111", "1111001111", "0110000110", "0000000000"],
    "jelly": [
        "0011111100", "0111111110", "1111111111", "1111111111", "1111111111",
        "0101010100", "0101010100", "1010101010", "0101010100", "1010101000"],
}


def mix(a, b, t):
    return tuple(int(a[i] + (b[i] - a[i]) * t) for i in range(3))


def rgba(c, a=255):
    return tuple(c[:3]) + (a,)


def noisy(c, rnd, amount=0.08):
    f = 1 + rnd.uniform(-amount, amount)
    return tuple(max(0, min(255, int(v * f))) for v in c)


def glow_line(img, pts, color, width=1, glow=2):
    d = ImageDraw.Draw(img)
    for g in range(glow, 0, -1):
        d.line(pts, fill=rgba(color, 40 + 20 * (glow - g)), width=width + 2 * g)
    d.line(pts, fill=rgba(color), width=width)


def text_width(text, scale):
    return len(text) * 4 * scale - scale


def draw_text(img, x, y, text, color, scale=2, glow=True):
    layer = Image.new("RGBA", img.size, (0, 0, 0, 0))
    d = ImageDraw.Draw(layer)
    cx = x
    for ch in text:
        g = FONT.get(ch, FONT[" "])
        for r, row in enumerate(g):
            for c, bit in enumerate(row):
                if bit == "1":
                    d.rectangle([cx + c * scale, y + r * scale, cx + c * scale + scale - 1, y + r * scale + scale - 1], fill=rgba(color))
        cx += 4 * scale
    if glow:
        halo = Image.new("RGBA", img.size, (0, 0, 0, 0))
        a = layer.getchannel("A")
        for dx in (-1, 0, 1):
            for dy in (-1, 0, 1):
                halo.paste(Image.new("RGBA", img.size, rgba(color, 70)), (dx, dy), a)
        img.alpha_composite(halo)
    img.alpha_composite(layer)


def draw_icon(img, x, y, name, color, scale=2):
    bits = ICONS[name]
    layer = Image.new("RGBA", img.size, (0, 0, 0, 0))
    d = ImageDraw.Draw(layer)
    for r, row in enumerate(bits):
        for c, bit in enumerate(row):
            if bit == "1":
                d.rectangle([x + c * scale, y + r * scale, x + c * scale + scale - 1, y + r * scale + scale - 1], fill=rgba(color))
    halo = Image.new("RGBA", img.size, (0, 0, 0, 0))
    a = layer.getchannel("A")
    for dx in (-2, -1, 0, 1, 2):
        for dy in (-2, -1, 0, 1, 2):
            if abs(dx) + abs(dy) <= 2:
                halo.paste(Image.new("RGBA", img.size, rgba(color, 45)), (dx, dy), a)
    img.alpha_composite(halo)
    img.alpha_composite(layer)


# -------------------------------------------------------------------------
#   GROUND
# -------------------------------------------------------------------------

def classify(px):
    best, name = None, None
    for col, cls in ORIGINAL_CLASSES.items():
        dist = sum((px[i] - col[i]) ** 2 for i in range(3))
        if best is None or dist < best:
            best, name = dist, cls
    return name


def repaint_ground_tile(src, seed):
    rnd = random.Random(seed)
    cls = [[classify(src.getpixel((x, y))) for x in range(T)] for y in range(T)]
    out = Image.new("RGBA", (T, T))
    for y in range(T):
        for x in range(T):
            c = cls[y][x]
            if c == "road":
                col = noisy(ASPHALT, rnd, 0.18)
            elif c == "sidewalk":
                col = SIDEWALK_GRID if (x % 20 == 0 or y % 20 == 0) else noisy(SIDEWALK, rnd, 0.05)
            elif c == "mark":
                inner = all(0 <= x + dx < T and 0 <= y + dy < T and cls[y + dy][x + dx] == "mark"
                            for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1), (-2, 0), (2, 0), (0, -2), (0, 2)))
                col = mix(MARK, ASPHALT, 0.55) if inner else MARK
            elif c == "curb":
                col = CURB_NEON
            elif c == "curb_dark":
                col = CURB_DARK
            else:
                col = LOT_NEON
            out.putpixel((x, y), rgba(col))

    for y in range(T):
        for x in range(T):
            c = cls[y][x]
            if c not in ("road", "sidewalk"):
                continue
            for src_cls, glow_col in (("mark", MARK_GLOW), ("curb", CURB_NEON), ("lot", LOT_NEON)):
                near = 0
                for dy in (-2, -1, 0, 1, 2):
                    for dx in (-2, -1, 0, 1, 2):
                        xx, yy = x + dx, y + dy
                        if 0 <= xx < T and 0 <= yy < T and cls[yy][xx] == src_cls:
                            near = max(near, 3 - max(abs(dx), abs(dy)))
                if near:
                    base = out.getpixel((x, y))[:3]
                    out.putpixel((x, y), rgba(mix(base, glow_col, 0.12 * near)))

    d = ImageDraw.Draw(out)
    for _ in range(3):
        x0, y0 = rnd.randrange(6, T - 6), rnd.randrange(6, T - 6)
        x1 = min(T - 6, max(6, x0 + rnd.choice([-1, 1]) * rnd.randrange(10, 30)))
        y1 = min(T - 6, max(6, y0 + rnd.choice([-1, 1]) * rnd.randrange(10, 30)))
        path = [(x0, y0), (x1, y0), (x1, y1)]
        if all(cls[py][px] == "sidewalk" for px, py in path) and cls[y0][(x0 + x1) // 2] == "sidewalk":
            ok = True
            for (ax, ay), (bx, by) in zip(path, path[1:]):
                steps = max(abs(bx - ax), abs(by - ay))
                for s in range(steps + 1):
                    px = ax + (bx - ax) * s // max(1, steps)
                    py = ay + (by - ay) * s // max(1, steps)
                    if cls[py][px] != "sidewalk":
                        ok = False
            if ok:
                d.line(path, fill=rgba(TRACE))
                for vx, vy in (path[0], path[-1]):
                    d.rectangle([vx - 1, vy - 1, vx + 1, vy + 1], fill=rgba(VIA))
                    d.point((vx, vy), fill=rgba(CHIP_EDGE))
    return out


# -------------------------------------------------------------------------
#   ROOFS: every building is a chip seen from above
# -------------------------------------------------------------------------

def big_chip():
    S = 3 * T
    img = Image.new("RGBA", (S, S), rgba(CHIP_BODY))
    d = ImageDraw.Draw(img)
    rnd = random.Random(32)
    for y in range(S):
        for x in range(S):
            if (x % 40 == 0 or y % 40 == 0):
                img.putpixel((x, y), rgba(CHIP_GRID))
            elif rnd.random() < 0.04:
                img.putpixel((x, y), rgba(noisy(CHIP_BODY, rnd, 0.3)))

    d.rectangle([0, 0, S - 1, 13], fill=rgba(CHIP_EDGE))
    d.rectangle([0, S - 14, S - 1, S - 1], fill=rgba(CHIP_EDGE))
    d.rectangle([0, 0, 13, S - 1], fill=rgba(CHIP_EDGE))
    d.rectangle([S - 14, 0, S - 1, S - 1], fill=rgba(CHIP_EDGE))

    for p in range(5, S, 10):
        if p < 16 or p > S - 17:
            continue
        d.rectangle([p - 2, 1, p + 2, 11], fill=rgba(PIN))
        d.line([(p - 2, 1), (p - 2, 11)], fill=rgba(PIN_HI))
        d.line([(p + 2, 1), (p + 2, 11)], fill=rgba(PIN_LO))
        d.rectangle([p - 2, S - 12, p + 2, S - 2], fill=rgba(PIN))
        d.line([(p - 2, S - 12), (p - 2, S - 2)], fill=rgba(PIN_HI))
        d.line([(p + 2, S - 12), (p + 2, S - 2)], fill=rgba(PIN_LO))
        d.rectangle([1, p - 2, 11, p + 2], fill=rgba(PIN))
        d.line([(1, p - 2), (11, p - 2)], fill=rgba(PIN_HI))
        d.line([(1, p + 2), (11, p + 2)], fill=rgba(PIN_LO))
        d.rectangle([S - 12, p - 2, S - 2, p + 2], fill=rgba(PIN))
        d.line([(S - 12, p - 2), (S - 2, p - 2)], fill=rgba(PIN_HI))
        d.line([(S - 12, p + 2), (S - 2, p + 2)], fill=rgba(PIN_LO))

    d.rectangle([14, 14, S - 15, 15], fill=rgba(mix(CHIP_BODY, (255, 255, 255), 0.15)))
    d.rectangle([14, 14, 15, S - 15], fill=rgba(mix(CHIP_BODY, (255, 255, 255), 0.15)))
    d.rectangle([14, S - 16, S - 15, S - 15], fill=rgba(mix(CHIP_BODY, (0, 0, 0), 0.4)))
    d.rectangle([S - 16, 14, S - 15, S - 15], fill=rgba(mix(CHIP_BODY, (0, 0, 0), 0.4)))

    glow_line(img, [(22, 22), (S - 23, 22), (S - 23, S - 23), (22, S - 23), (22, 22)], CHIP_RING, width=2, glow=2)

    d.ellipse([28, 28, 38, 38], fill=rgba(CURB_NEON))
    d.ellipse([30, 30, 36, 36], fill=rgba((255, 200, 235)))
    return img


def chip_piece(chip, col, row):
    return chip.crop((col * T, row * T, col * T + T, row * T + T))


def roof_sign(center, text, color):
    img = center.copy()
    d = ImageDraw.Draw(img)
    lines = text.split("|")
    longest = max(lines, key=len)
    scale = 2 if text_width(longest, 2) <= 70 else 1
    w = text_width(longest, scale)
    line_h = 5 * scale + 3
    h = line_h * len(lines) - 3
    x0, y0 = (T - w) // 2, (T - h) // 2
    pad = 6 if w <= 64 else 3
    px0, py0, px1, py1 = x0 - pad, y0 - 6, x0 + w + pad - 1, y0 + h + 5
    d.rectangle([px0 + 2, py0 + 3, px1 + 2, py1 + 3], fill=(0, 0, 0, 120))
    d.rectangle([px0, py0, px1, py1], fill=rgba((12, 10, 22)))
    glow_line(img, [(px0, py0), (px1, py0), (px1, py1), (px0, py1), (px0, py0)], color, width=1, glow=2)
    for lx in (px0 + 6, px1 - 6):
        d.rectangle([lx - 1, py1 + 1, lx + 1, py1 + 6], fill=rgba(PIN_LO))
    for n, line in enumerate(lines):
        lx = (T - text_width(line, scale)) // 2
        draw_text(img, lx, y0 + n * line_h, line, color, scale)
    return img


def roof_helipad(center):
    img = center.copy()
    d = ImageDraw.Draw(img)
    d.ellipse([10, 10, 69, 69], fill=rgba((34, 32, 52)))
    d.ellipse([12, 12, 67, 67], outline=rgba((255, 225, 60)), width=2)
    d.rectangle([28, 24, 32, 55], fill=rgba((245, 245, 250)))
    d.rectangle([47, 24, 51, 55], fill=rgba((245, 245, 250)))
    d.rectangle([32, 37, 47, 41], fill=rgba((245, 245, 250)))
    for x, y in ((12, 40), (67, 40), (40, 12), (40, 67)):
        d.rectangle([x - 1, y - 1, x + 1, y + 1], fill=rgba(CURB_NEON))
    return img


def roof_fan(center):
    img = center.copy()
    d = ImageDraw.Draw(img)
    d.rectangle([8, 8, 71, 71], fill=rgba((18, 16, 30)), outline=rgba(PIN_LO))
    for x, y in ((12, 12), (67, 12), (12, 67), (67, 67)):
        d.ellipse([x - 2, y - 2, x + 2, y + 2], fill=rgba(PIN))
    d.ellipse([12, 12, 67, 67], fill=rgba((10, 8, 18)))
    glow_line(img, [(40 + 27 * math.cos(a / 10), 40 + 27 * math.sin(a / 10)) for a in range(0, 64)], (0, 235, 255), width=1, glow=1)
    for i in range(7):
        a = i * 2 * math.pi / 7
        pts = [(40, 40),
               (40 + 25 * math.cos(a), 40 + 25 * math.sin(a)),
               (40 + 22 * math.cos(a + 0.55), 40 + 22 * math.sin(a + 0.55))]
        d.polygon(pts, fill=rgba((70, 66, 100)), outline=rgba((110, 105, 150)))
    d.ellipse([33, 33, 47, 47], fill=rgba((30, 28, 46)), outline=rgba(CURB_NEON))
    draw_text(img, 35, 38, "V", CURB_NEON, 1, glow=False)
    return img


def roof_heatsink(center):
    img = center.copy()
    d = ImageDraw.Draw(img)
    d.rectangle([6, 6, 73, 73], fill=rgba((60, 64, 82)))
    for x in range(8, 72, 6):
        d.rectangle([x, 8, x + 3, 71], fill=rgba((150, 156, 178)))
        d.line([(x, 8), (x, 71)], fill=rgba((210, 215, 232)))
        d.line([(x + 3, 8), (x + 3, 71)], fill=rgba((90, 94, 115)))
    glow_line(img, [(6, 6), (73, 6), (73, 73), (6, 73), (6, 6)], (170, 255, 0), width=1, glow=1)
    return img


# -------------------------------------------------------------------------
#   WALLS: 8 facade stripes, 80 wide x 160 tall (row 0 = roof edge)
# -------------------------------------------------------------------------

WALL_H = 160


def wall_base(seed, trim):
    rnd = random.Random(seed)
    img = Image.new("RGBA", (T, WALL_H), rgba((30, 24, 48)))
    d = ImageDraw.Draw(img)
    for y in range(WALL_H):
        for x in range(T):
            base = mix((34, 27, 54), (20, 16, 34), y / WALL_H)
            if x % 20 == 0:
                base = mix(base, (0, 0, 0), 0.35)
            elif rnd.random() < 0.05:
                base = noisy(base, rnd, 0.25)
            img.putpixel((x, y), rgba(base))
    d.rectangle([0, 0, T - 1, 3], fill=rgba(mix(trim, (0, 0, 0), 0.5)))
    glow_line(img, [(0, 5), (T - 1, 5)], trim, width=1, glow=1)
    d.rectangle([0, WALL_H - 4, T - 1, WALL_H - 1], fill=rgba((12, 10, 20)))
    return img, rnd


def windows(img, rnd, rows, cols=3, x0=8, y0=16, w=14, h=12, gx=10, gy=10):
    d = ImageDraw.Draw(img)
    lit = [(255, 205, 100), (130, 240, 255), (255, 120, 205), (190, 150, 255)]
    for r in range(rows):
        for c in range(cols):
            x, y = x0 + c * (w + gx), y0 + r * (h + gy)
            if rnd.random() < 0.7:
                col = rnd.choice(lit)
                d.rectangle([x - 1, y - 1, x + w, y + h], fill=rgba(col, 60))
                d.rectangle([x, y, x + w - 1, y + h - 1], fill=rgba(col))
                d.rectangle([x, y + h // 2, x + w - 1, y + h - 1], fill=rgba(mix(col, (0, 0, 0), 0.2)))
            else:
                d.rectangle([x, y, x + w - 1, y + h - 1], fill=rgba((16, 14, 30)))
            d.rectangle([x, y, x + w - 1, y + h - 1], outline=rgba((10, 8, 18)))


def door(img, color):
    d = ImageDraw.Draw(img)
    d.rectangle([27, WALL_H - 34, 52, WALL_H - 5], fill=rgba((10, 8, 18)))
    d.rectangle([29, WALL_H - 32, 50, WALL_H - 5], fill=rgba(mix(color, (0, 0, 0), 0.55)))
    d.line([(39, WALL_H - 32), (39, WALL_H - 5)], fill=rgba((10, 8, 18)))
    glow_line(img, [(25, WALL_H - 37), (54, WALL_H - 37)], color, width=1, glow=1)


def sign_panel(img, icon, color, y=24):
    d = ImageDraw.Draw(img)
    d.rectangle([14, y, 65, y + 40], fill=rgba((10, 8, 20)))
    glow_line(img, [(14, y), (65, y), (65, y + 40), (14, y + 40), (14, y)], color, width=1, glow=2)
    draw_icon(img, 30, y + 10, icon, color, 2)


def make_wall(i):
    trim = NEONS[i % len(NEONS)]
    img, rnd = wall_base(100 + i, trim)
    d = ImageDraw.Draw(img)
    if i == 0:
        sign_panel(img, "joystick", CURB_NEON)
        windows(img, rnd, 1, y0=80)
        door(img, CURB_NEON)
    elif i == 1:
        windows(img, rnd, 6, y0=14, h=12, gy=10)
    elif i == 2:
        sign_panel(img, "tetromino", (0, 235, 255))
        windows(img, rnd, 2, y0=78)
    elif i == 3:
        sign_panel(img, "ghost", (190, 90, 255))
        windows(img, rnd, 1, y0=80)
        door(img, (190, 90, 255))
    elif i == 4:
        sign_panel(img, "bird", (255, 225, 60))
        windows(img, rnd, 2, y0=78)
    elif i == 5:
        sign_panel(img, "cat", (0, 235, 255))
        windows(img, rnd, 1, y0=80)
        door(img, (0, 235, 255))
    elif i == 6:
        for x in (10, 60):
            d.rectangle([x, 8, x + 8, WALL_H - 6], fill=rgba((70, 64, 96)))
            d.line([(x + 1, 8), (x + 1, WALL_H - 6)], fill=rgba((120, 112, 150)))
            for y in range(20, WALL_H - 10, 30):
                d.rectangle([x - 1, y, x + 9, y + 3], fill=rgba((50, 46, 70)))
        for y in range(18, WALL_H - 20, 24):
            d.rectangle([28, y, 50, y + 12], fill=rgba((18, 16, 30)), outline=rgba((60, 56, 84)))
            for yy in range(y + 2, y + 12, 3):
                d.line([(30, yy), (48, yy)], fill=rgba((40, 38, 60)))
            d.point((52, y + 2), fill=rgba((170, 255, 0)) if rnd.random() < 0.5 else rgba(CURB_NEON))
    else:
        sign_panel(img, "gamepad", (170, 255, 0))
        windows(img, rnd, 1, y0=80)
        door(img, (170, 255, 0))
    return img


# -------------------------------------------------------------------------
#   BUILD
# -------------------------------------------------------------------------

SIGN_SLOTS = [13, 14, 15, 16, 17, 18, 19, 23, 25, 26, 27, 28, 29, 33, 34, 35]
SIGNS = ["VIRCOBAN", "PUZTRIX", "BLOCKDUDE", "ZNAX", "WATERNET", "RUBIDO", "OCEAN|STORM", "VITRIS",
         "HAUNTY", "JELLYFISH", "FLAPPY", "WORM", "BLIPS", "CARRA HQ", "BIOS", "V32"]
SPECIAL_SLOTS = {40: "helipad", 41: "fan", 42: "heatsink"}

ROOF_ROLES = {36: (0, 0), 38: (1, 0), 37: (2, 0), 49: (0, 1), 24: (1, 1), 39: (2, 1), 46: (0, 2), 48: (1, 2), 47: (2, 2)}


def build():
    original = Image.open(os.path.join(HERE, "src", "TextureGround_layout.png")).convert("RGBA")
    ground = Image.new("RGBA", (10 * T, 5 * T), (0, 0, 0, 0))
    ground.paste(original.crop((0, 0, T, T)), (0, 0))

    def tile_src(i):
        r, c = divmod(i, 10)
        return original.crop((c * T, r * T, c * T + T, r * T + T))

    def put(i, img):
        r, c = divmod(i, 10)
        ground.paste(img, (c * T, r * T))

    put(1, repaint_ground_tile(Image.new("RGBA", (T, T), (98, 98, 98, 255)), 1))
    for i in [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 20, 21, 22, 30, 31, 32]:
        put(i, repaint_ground_tile(tile_src(i), i))

    chip = big_chip()
    center = chip_piece(chip, 1, 1)
    for tile, (c, r) in ROOF_ROLES.items():
        put(tile, chip_piece(chip, c, r))
    for n, (slot, text) in enumerate(zip(SIGN_SLOTS, SIGNS)):
        put(slot, roof_sign(center, text, NEONS[n % len(NEONS)]))
    put(40, roof_helipad(center))
    put(41, roof_fan(center))
    put(42, roof_heatsink(center))
    for i in (43, 44, 45):
        put(i, center)

    ground.save(os.path.join(ROOT, "assets", "textures", "TextureGround.png"))
    ground.save(os.path.join(ROOT, "assets", "maps", "TileSetCity.png"))

    walls = Image.new("RGBA", (8 * T, WALL_H), (0, 0, 0, 0))
    for i in range(8):
        walls.paste(make_wall(i), (i * T, 0))
    walls.save(os.path.join(ROOT, "assets", "textures", "TextureWalls.png"))
    return ground, walls


if __name__ == "__main__":
    ground, walls = build()
    out = os.path.join(HERE, "out")
    os.makedirs(out, exist_ok=True)
    ground.resize((ground.width * 2, ground.height * 2), Image.NEAREST).save(os.path.join(out, "_city_tiles.png"))
    walls.resize((walls.width * 2, walls.height * 2), Image.NEAREST).save(os.path.join(out, "_city_walls.png"))
    print("OK")
