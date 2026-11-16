#!/usr/bin/env python3

import os, math, random
from PIL import Image, ImageDraw, ImageFilter

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..")

W, H = 160, 90
SCALE = 4

CYAN = (0, 235, 255)
PINK = (255, 40, 170)
YELLOW = (255, 225, 60)

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
    "#": ["01010", "11111", "01010", "11111", "01010"], ".": ["000", "000", "000", "000", "010"],
}


def rgba(c, a=255):
    return tuple(c[:3]) + (a,)


def mix(a, b, t):
    return tuple(int(a[i] + (b[i] - a[i]) * t) for i in range(3))


def shade(c, f):
    return tuple(max(0, min(255, int(v * f))) for v in c[:3])


def text(img, x, y, s, color, scale=1):
    d = ImageDraw.Draw(img)
    for ch in s.upper():
        g = FONT.get(ch, FONT[" "])
        for r, row in enumerate(g):
            for c, bit in enumerate(row):
                if bit == "1":
                    d.rectangle([x + c * scale, y + r * scale, x + c * scale + scale - 1, y + r * scale + scale - 1], fill=rgba(color))
        x += (len(g[0]) + 1) * scale


# -------------------------------------------------------------------------

def room():
    img = Image.new("RGBA", (W, H))
    d = ImageDraw.Draw(img)
    rnd = random.Random(7)

    for y in range(H):
        d.line([(0, y), (W, y)], fill=rgba(mix((22, 16, 40), (10, 8, 20), y / H)))

    d.rectangle([3, 6, 47, 54], fill=rgba((8, 6, 14)))
    for y in range(8, 53):
        d.line([(5, y), (45, y)], fill=rgba(mix((30, 14, 60), (70, 20, 70), (y - 8) / 45)))
    for bx, bw, bh in ((5, 8, 26), (13, 7, 36), (20, 10, 22), (30, 6, 40), (36, 9, 30)):
        d.rectangle([bx, 52 - bh, bx + bw, 52], fill=rgba((14, 10, 26)))
        for wy in range(52 - bh + 3, 50, 4):
            for wx in range(bx + 1, bx + bw - 1, 3):
                if rnd.random() < 0.45:
                    d.point((wx, wy), fill=rgba(rnd.choice([CYAN, PINK, YELLOW, (255, 200, 120)])))
    d.rectangle([22, 18, 28, 21], fill=rgba(PINK))
    d.line([(25, 21), (25, 30)], fill=rgba((40, 20, 50)))
    for y in range(9, 53, 4):
        d.line([(5, y), (45, y)], fill=rgba((8, 6, 14), 150))
    d.line([(25, 8), (25, 52)], fill=rgba((8, 6, 14)))

    d.rectangle([50, 12, 56, 30], fill=rgba((40, 14, 50)), outline=rgba(PINK))
    text(img, 52, 14, "O", CYAN)
    text(img, 52, 20, "S", CYAN)

    d.rectangle([0, 68, W, H], fill=rgba((26, 18, 36)))
    d.line([(0, 68), (W, 68)], fill=rgba((70, 50, 90)))
    d.line([(0, 69), (W, 69)], fill=rgba((40, 28, 56)))

    d.rectangle([101, 60, 113, 69], fill=rgba((18, 16, 26)))
    d.rectangle([94, 67, 120, 69], fill=rgba((22, 20, 30)))

    d.rectangle([58, 5, 156, 62], fill=rgba((12, 11, 18)))
    d.rectangle([58, 5, 156, 62], outline=rgba((34, 32, 46)))
    d.line([(59, 6), (155, 6)], fill=rgba((60, 58, 80)))
    d.rectangle([61, 8, 153, 58], fill=rgba((6, 6, 12)))

    d.rectangle([61, 8, 153, 58], fill=rgba((24, 20, 40)))
    for gx in range(61, 154, 12):
        d.line([(gx, 8), (gx, 58)], fill=rgba((36, 30, 58)))
    d.rectangle([61, 30, 153, 34], fill=rgba((16, 14, 28)))
    d.rectangle([98, 8, 102, 58], fill=rgba((16, 14, 28)))
    for yy in range(9, 58, 5):
        d.point((100, yy), fill=rgba(CYAN))
    for xx in range(63, 152, 5):
        d.point((xx, 32), fill=rgba(CYAN))
    for bx, by in ((66, 12), (80, 14), (108, 11), (126, 13), (70, 40), (112, 40), (134, 42)):
        d.rectangle([bx, by, bx + 12, by + 10], fill=rgba((26, 24, 42)), outline=rgba(CYAN))
    d.rectangle([90, 36, 93, 42], fill=rgba((235, 95, 20)))
    d.rectangle([117, 22, 120, 28], fill=rgba(YELLOW))
    d.rectangle([61, 8, 153, 11], fill=rgba((10, 8, 18)))
    text(img, 63, 9, "PIXEL HUSTLER 0.99", (200, 200, 220))
    d.point((150, 9), fill=rgba((255, 80, 80)))

    kx0, ky0 = 72, 73
    d.rectangle([kx0, ky0, kx0 + 64, ky0 + 9], fill=rgba((16, 14, 22)), outline=rgba((40, 36, 52)))
    for r in range(3):
        for c in range(15):
            t = c / 14
            col = mix(PINK, CYAN, t) if r != 1 else mix(CYAN, YELLOW, t)
            d.point((kx0 + 3 + c * 4, ky0 + 2 + r * 3), fill=rgba(shade(col, 0.8)))
    d.rectangle([142, 74, 150, 83], fill=rgba((235, 235, 240)))
    d.rectangle([150, 76, 152, 80], outline=rgba((235, 235, 240)))
    d.rectangle([143, 75, 149, 77], fill=rgba((90, 50, 30)))

    fig = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    f = ImageDraw.Draw(fig)
    hair = (15, 15, 22)
    jacket = (150, 60, 16)
    f.ellipse([-14, 64, 76, 118], fill=rgba(shade(jacket, 0.5)))
    f.ellipse([-10, 67, 68, 114], fill=rgba(shade(jacket, 0.62)))
    f.line([(38, 68), (44, 90)], fill=rgba((40, 30, 30)))
    f.rectangle([24, 58, 40, 70], fill=rgba((90, 64, 50)))
    f.ellipse([14, 30, 50, 66], fill=rgba(hair))
    for sx, sy in ((16, 36), (20, 30), (26, 27), (33, 26), (40, 28), (46, 33)):
        f.polygon([(sx - 3, sy + 6), (sx, sy - 4), (sx + 4, sy + 6)], fill=rgba(hair))
    f.ellipse([48, 46, 52, 53], fill=rgba((150, 105, 80)))
    f.point((50, 48), fill=rgba((110, 75, 60)))

    alpha = fig.getchannel("A")
    fp = fig.load()
    ap = alpha.load()
    lit = fig.copy()
    lp = lit.load()
    for y in range(H):
        for x in range(W):
            if not ap[x, y]:
                continue
            right = ap[x + 1, y] if x + 1 < W else 0
            right2 = ap[x + 2, y] if x + 2 < W else 0
            up = ap[x, y - 1] if y > 0 else 0
            base = fp[x, y][:3]
            if not right:
                lp[x, y] = rgba(mix(base, CYAN, 0.85))
            elif not right2:
                lp[x, y] = rgba(mix(base, CYAN, 0.45))
            elif not up and x > 22:
                lp[x, y] = rgba(mix(base, CYAN, 0.5))
    img.alpha_composite(lit)
    return img


def light_overlay():
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    px = img.load()
    cx, cy = 107, 33
    bayer = [[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]]
    for y in range(H):
        for x in range(W):
            dx, dy = (x - cx) / 70.0, (y - cy) / 45.0
            dist = math.sqrt(dx * dx + dy * dy)
            v = max(0.0, 1.0 - dist)
            if 58 <= x <= 156 and 5 <= y <= 62:
                v *= 0.35
            v = v ** 1.6
            level = int(v * 16)
            if bayer[y % 4][x % 4] < (v * 16 - level) * 16:
                level += 1
            a = min(255, level * 14)
            if a:
                px[x, y] = (40, 170, 255, a)
    return img


# -------------------------------------------------------------------------

CHAT_W, CHAT_H = 640, 360

USERS = [
    ("Chip Buster", (235, 95, 20)),
    ("Jastro", (20, 190, 90)),
    ("Ridote", (230, 50, 60)),
    ("Totespare", (255, 140, 0)),
    ("Freak1337", (170, 255, 0)),
    ("kidandcat", (0, 200, 230)),
    ("Paletochen", (255, 210, 60)),
    ("Carra", (120, 110, 200)),
]


def chat_background():
    img = Image.new("RGBA", (CHAT_W, CHAT_H), rgba((30, 26, 46)))
    d = ImageDraw.Draw(img)
    d.rectangle([0, 0, 43, CHAT_H], fill=rgba((12, 10, 20)))
    icons = [((255, 40, 170), "V"), ((0, 200, 230), "J"), ((170, 255, 0), "O"), ((255, 140, 0), "S")]
    for i, (col, ch) in enumerate(icons):
        y = 10 + i * 40
        d.ellipse([7, y, 36, y + 29], fill=rgba(shade(col, 0.6) if i else col))
        text(img, 16, y + 10, ch, (250, 250, 255), 2)
    d.rectangle([2, 12, 4, 36], fill=rgba((250, 250, 255)))

    d.rectangle([44, 0, 183, CHAT_H], fill=rgba((20, 17, 32)))
    d.rectangle([44, 0, 183, 28], fill=rgba((16, 13, 26)))
    text(img, 52, 9, "VIRCON32 OFICIAL", (240, 240, 250), 2)
    channels = ["general", "showcase", "ocean-storm", "virconstein", "help", "memes"]
    for i, ch in enumerate(channels):
        y = 44 + i * 22
        if ch == "showcase":
            d.rectangle([48, y - 5, 179, y + 14], fill=rgba((44, 38, 66)))
        text(img, 54, y, "#", (130, 120, 160), 2)
        text(img, 66, y, ch, (240, 240, 250) if ch == "showcase" else (140, 130, 170), 2)
    d.ellipse([168, 44 + 2 * 22, 176, 52 + 2 * 22], fill=rgba((230, 50, 60)))

    d.rectangle([44, CHAT_H - 40, 183, CHAT_H], fill=rgba((14, 12, 22)))
    d.ellipse([50, CHAT_H - 33, 74, CHAT_H - 9], fill=rgba((235, 95, 20)))
    text(img, 58, CHAT_H - 26, "C", (250, 250, 255), 2)
    d.ellipse([68, CHAT_H - 15, 76, CHAT_H - 7], fill=rgba((60, 210, 90)), outline=rgba((14, 12, 22)))
    text(img, 82, CHAT_H - 27, "Chip Buster", (240, 240, 250), 1)
    text(img, 82, CHAT_H - 19, "online", (120, 200, 130), 1)

    d.rectangle([184, 0, CHAT_W, 28], fill=rgba((26, 22, 40)))
    d.line([(184, 28), (CHAT_W, 28)], fill=rgba((14, 12, 22)))
    text(img, 194, 9, "#", (130, 120, 160), 2)
    text(img, 206, 9, "showcase", (240, 240, 250), 2)
    text(img, 290, 11, "- show your vircon32 games", (120, 110, 150), 1)

    d.rounded_rectangle([192, CHAT_H - 36, CHAT_W - 10, CHAT_H - 10], radius=6, fill=rgba((40, 34, 60)))
    text(img, 204, CHAT_H - 26, "message #showcase", (110, 100, 140), 1)
    return img


def avatar(name, col):
    img = Image.new("RGBA", (26, 26), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse([0, 0, 25, 25], fill=rgba(col))
    d.ellipse([2, 2, 23, 12], fill=rgba(mix(col, (255, 255, 255), 0.18)))
    ch = name[0].upper()
    text(img, 10, 8, ch, (20, 16, 30), 2)
    return img


def toast():
    img = Image.new("RGBA", (340, 46), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle([0, 0, 339, 45], radius=6, fill=rgba((20, 16, 34), 240), outline=rgba(PINK))
    d.rounded_rectangle([1, 1, 338, 44], radius=6, outline=rgba(PINK, 90))
    return img


def build():
    lit = room()
    light = light_overlay()

    intro = Image.new("RGBA", (W * SCALE, H * SCALE * 2), (0, 0, 0, 0))
    intro.paste(lit.resize((W * SCALE, H * SCALE), Image.NEAREST), (0, 0))
    intro.paste(light.resize((W * SCALE, H * SCALE), Image.NEAREST), (0, H * SCALE))
    intro.save(os.path.join(ROOT, "assets", "textures", "TextureIntro.png"))

    chat = Image.new("RGBA", (1024, 512), (0, 0, 0, 0))
    chat.paste(chat_background(), (0, 0))
    for i, (name, col) in enumerate(USERS):
        chat.paste(avatar(name, col), (650 + (i % 4) * 30, 2 + (i // 4) * 30))
    chat.paste(toast(), (650, 70))
    chat.paste(Image.new("RGBA", (8, 8), (255, 255, 255, 255)), (650, 130))
    chat.save(os.path.join(ROOT, "assets", "textures", "TextureChat.png"))

    out = os.path.join(HERE, "out")
    os.makedirs(out, exist_ok=True)
    preview = lit.resize((W * SCALE, H * SCALE), Image.NEAREST)
    glow = light.resize((W * SCALE, H * SCALE), Image.NEAREST)
    add = Image.new("RGBA", preview.size)
    pp, gp, ap = preview.load(), glow.load(), add.load()
    for y in range(preview.height):
        for x in range(preview.width):
            r, g, b, _ = pp[x, y]
            gr, gg, gb, ga = gp[x, y]
            f = ga / 255 * 0.7
            ap[x, y] = (min(255, int(r + gr * f)), min(255, int(g + gg * f)), min(255, int(b + gb * f)), 255)
    add.save(os.path.join(out, "_intro_room.png"))
    chat.crop((0, 0, 640, 360)).save(os.path.join(out, "_intro_chat.png"))


if __name__ == "__main__":
    build()
    print("OK")
