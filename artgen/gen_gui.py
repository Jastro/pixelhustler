#!/usr/bin/env python3
# OceanStorm's dialog frame: 552 x 124. Portraits: 100 x 100.
# Same sizes. Same layout. Different pixels.
# Jastro: "so it's a new dialog system"
# Carra: "it's the old one with a purple coat of paint"

import os
from PIL import Image, ImageDraw, ImageFilter

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..")

FRAME_W, FRAME_H = 552, 150
PORTRAIT = 100

CYAN = (0, 235, 255)
PINK = (255, 40, 170)
DARK = (14, 12, 26)
PANEL = (28, 22, 48)


def rgba(c, a=255):
    return tuple(c[:3]) + (a,)


def shade(c, f):
    return tuple(max(0, min(255, int(v * f))) for v in c[:3])


def glow_rect(img, box, color, width=2, glow=3):
    d = ImageDraw.Draw(img)
    x0, y0, x1, y1 = box
    for g in range(glow, 0, -1):
        d.rectangle([x0 - g, y0 - g, x1 + g, y1 + g], outline=rgba(color, 30 + 25 * (glow - g)), width=1)
    d.rectangle(box, outline=rgba(color), width=width)


def dialog_frame():
    img = Image.new("RGBA", (FRAME_W, FRAME_H), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rectangle([4, 4, FRAME_W - 5, FRAME_H - 5], fill=rgba(DARK))
    for y in range(6, FRAME_H - 6, 3):
        d.line([(6, y), (FRAME_W - 7, y)], fill=rgba((22, 18, 40)))
    glow_rect(img, [4, 4, FRAME_W - 5, FRAME_H - 5], CYAN, 2, 3)

    d.rectangle([10, 10, 127, FRAME_H - 11], fill=rgba(PANEL))
    d.rectangle([12, 116, 125, FRAME_H - 13], fill=rgba(DARK))
    d.line([(12, 115), (125, 115)], fill=rgba(PINK, 160))
    glow_rect(img, [10, 10, 127, FRAME_H - 11], PINK, 1, 2)
    d.rectangle([134, 10, FRAME_W - 11, FRAME_H - 11], fill=rgba(PANEL))

    for x in range(140, FRAME_W - 20, 12):
        d.rectangle([x, 0, x + 5, 3], fill=rgba((175, 180, 198)))
        d.rectangle([x, FRAME_H - 4, x + 5, FRAME_H - 1], fill=rgba((175, 180, 198)))
    d.ellipse([FRAME_W - 22, 12, FRAME_W - 14, 20], fill=rgba(PINK))
    return img


def synth_background(img, top, bottom, grid):
    d = ImageDraw.Draw(img)
    w, h = img.size
    for y in range(h):
        t = y / h
        d.line([(0, y), (w, y)], fill=rgba(tuple(int(top[i] + (bottom[i] - top[i]) * t) for i in range(3))))
    horizon = h * 11 // 20
    d.ellipse([w // 2 - 12, horizon - 14, w // 2 + 12, horizon + 10], fill=rgba((255, 120, 60)))
    for y in range(horizon - 10, horizon + 8, 3):
        d.line([(w // 2 - 13, y), (w // 2 + 13, y)], fill=rgba(top))
    d.rectangle([0, horizon, w, h], fill=rgba((20, 10, 36)))
    for i in range(0, 6):
        y = horizon + int((i / 5) ** 1.8 * (h - horizon))
        d.line([(0, y), (w, y)], fill=rgba(grid))
    for x in range(-40, w + 40, 8):
        d.line([(w // 2 + (x - w // 2) // 5, horizon), (x, h)], fill=rgba(grid))


def outline(fig, color=(8, 6, 16)):
    alpha = fig.getchannel("A").point(lambda a: 255 if a > 0 else 0)
    ring = alpha.filter(ImageFilter.MaxFilter(3))
    out = Image.new("RGBA", fig.size, (0, 0, 0, 0))
    out.paste(Image.new("RGBA", fig.size, rgba(color)), (0, 0), ring)
    out.alpha_composite(fig)
    return out


def portrait(who):
    S = 50
    bg = Image.new("RGBA", (S, S))
    fig = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(fig)
    skin = (232, 180, 136)
    skin_dark = shade(skin, 0.78)

    if who == "jastro":
        synth_background(bg, (60, 20, 90), (150, 40, 120), (255, 40, 170))
        jacket, trim, hair = (20, 170, 80), (170, 255, 120), (18, 16, 22)
    elif who == "chip":
        synth_background(bg, (40, 12, 30), (170, 60, 30), (255, 180, 40))
        jacket, trim, hair = (225, 90, 20), (255, 210, 60), (15, 15, 22)
    elif who == "joyrider":
        synth_background(bg, (50, 10, 60), (120, 20, 110), (255, 225, 60))
        jacket, trim, hair = (110, 40, 160), (255, 225, 60), (60, 40, 30)
    elif who == "unknown":
        synth_background(bg, (18, 16, 30), (40, 34, 60), (90, 80, 130))
        jacket, trim, hair = (50, 46, 70), (90, 80, 130), (40, 36, 56)
    else:
        synth_background(bg, (10, 30, 60), (30, 60, 110), (0, 235, 255))
        jacket, trim, hair = (26, 26, 32), (26, 26, 32), (52, 42, 40)

    d.pieslice([4, 36, 45, 70], 180, 360, fill=rgba(jacket))
    d.line([(10, 42), (24, 49)], fill=rgba(trim))
    d.line([(39, 42), (25, 49)], fill=rgba(trim))
    d.rectangle([20, 32, 29, 40], fill=rgba(skin_dark))
    d.ellipse([13, 10, 36, 36], fill=rgba(skin))
    d.ellipse([28, 12, 36, 34], fill=rgba(skin_dark))
    d.rectangle([12, 21, 14, 26], fill=rgba(skin_dark))
    d.rectangle([35, 21, 37, 26], fill=rgba(skin_dark))

    if who == "jastro":
        beard = (34, 28, 32)
        beard_hi = (54, 46, 50)
        fade = (62, 56, 64)
        d.rectangle([8, 3, 41, 35], fill=(0, 0, 0, 0))
        d.rectangle([20, 32, 29, 40], fill=rgba(skin_dark))
        d.ellipse([15, 8, 34, 36], fill=rgba(skin))
        d.ellipse([28, 10, 34, 34], fill=rgba(skin_dark))
        d.rectangle([13, 20, 15, 25], fill=rgba(skin_dark))
        d.rectangle([34, 20, 36, 25], fill=rgba(skin_dark))
        d.rectangle([15, 11, 17, 20], fill=rgba(fade))
        d.rectangle([32, 11, 34, 20], fill=rgba(fade))
        d.chord([15, 5, 34, 22], 180, 360, fill=rgba(hair))
        d.polygon([(15, 12), (16, 6), (21, 2), (28, 1), (34, 3), (37, 7), (35, 11), (29, 8), (22, 8), (17, 11)], fill=rgba(hair))
        d.line([(19, 5), (27, 3)], fill=rgba((80, 80, 100)))
        d.line([(28, 3), (33, 4)], fill=rgba((60, 60, 78)))
        d.polygon([(13, 27), (16, 27), (17, 31), (20, 35), (24, 39), (13, 39)], fill=(0, 0, 0, 0))
        d.polygon([(36, 27), (33, 27), (32, 31), (29, 35), (25, 39), (36, 39)], fill=(0, 0, 0, 0))
        d.rectangle([20, 32, 29, 40], fill=rgba(skin_dark))
        d.polygon([(15, 20), (16, 20), (16, 25), (20, 27), (29, 27), (33, 25), (33, 20), (34, 20),
                   (34, 26), (33, 31), (30, 35), (25, 38), (19, 35), (16, 31), (15, 26)], fill=rgba(beard))
        d.line([(20, 27), (29, 27)], fill=rgba(beard_hi))
        for bx, by in ((18, 31), (22, 35), (27, 35), (31, 30), (24, 37)):
            d.point((bx, by), fill=rgba(beard_hi))
        d.line([(22, 31), (27, 31)], fill=rgba((90, 50, 50)))
        d.rectangle([16, 15, 21, 16], fill=rgba(hair))
        d.rectangle([28, 15, 33, 16], fill=rgba(hair))
        d.rectangle([15, 18, 22, 23], outline=rgba((10, 8, 14)))
        d.rectangle([27, 18, 34, 23], outline=rgba((10, 8, 14)))
        d.line([(23, 19), (26, 19)], fill=rgba((10, 8, 14)))
        d.rectangle([16, 19, 21, 22], fill=rgba((200, 220, 235)))
        d.rectangle([28, 19, 33, 22], fill=rgba((200, 220, 235)))
        d.rectangle([18, 20, 19, 21], fill=rgba((40, 30, 30)))
        d.rectangle([30, 20, 31, 21], fill=rgba((40, 30, 30)))
        d.line([(16, 19), (17, 19)], fill=rgba((255, 255, 255)))
        d.line([(28, 19), (29, 19)], fill=rgba((255, 255, 255)))
        d.line([(24, 22), (24, 26)], fill=rgba(skin_dark))
        d.point((38, 4), fill=rgba((255, 255, 180)))
        d.point((39, 3), fill=rgba((255, 255, 255)))
        d.point((40, 4), fill=rgba((255, 255, 180)))
        d.point((39, 5), fill=rgba((255, 255, 180)))
    elif who == "joyrider":
        d.chord([12, 5, 37, 27], 180, 360, fill=rgba((110, 40, 160)))
        d.rectangle([11, 14, 38, 17], fill=rgba((110, 40, 160)))
        d.rectangle([8, 15, 14, 18], fill=rgba((90, 30, 130)))
        d.line([(12, 13), (37, 13)], fill=rgba((255, 225, 60)))
        d.rectangle([22, 7, 27, 11], fill=rgba((255, 225, 60)))
        d.point((24, 9), fill=rgba((110, 40, 160)))
        d.line([(16, 19), (22, 19)], fill=rgba(hair))
        d.line([(27, 18), (33, 20)], fill=rgba(hair))
        d.rectangle([17, 21, 21, 22], fill=rgba((250, 250, 250)))
        d.rectangle([28, 21, 32, 22], fill=rgba((250, 250, 250)))
        d.point((19, 22), fill=rgba((30, 30, 40)))
        d.point((30, 22), fill=rgba((30, 30, 40)))
        d.line([(20, 28), (28, 28)], fill=rgba((100, 70, 50)))
        d.polygon([(22, 32), (27, 32), (26, 35), (23, 35)], fill=rgba((100, 70, 50)))
        d.line([(21, 30), (27, 30)], fill=rgba((120, 50, 45)))
        d.line([(27, 30), (29, 28)], fill=rgba((120, 50, 45)))
        d.rectangle([34, 40, 40, 47], fill=rgba((20, 20, 30)))
        d.rectangle([35, 41, 39, 43], fill=rgba((0, 235, 255)))
        d.point((36, 45), fill=rgba((255, 40, 170)))
        d.point((38, 45), fill=rgba((255, 225, 60)))
    elif who == "unknown":
        d.rectangle([0, 0, 49, 49], fill=(0, 0, 0, 0))
        d.pieslice([6, 36, 43, 70], 180, 360, fill=rgba((30, 26, 44)))
        d.ellipse([14, 10, 35, 35], fill=rgba((30, 26, 44)))
        d.arc([14, 10, 35, 35], 200, 330, fill=rgba((90, 80, 130)))
        for (x, y) in ((22, 17), (23, 16), (24, 16), (25, 16), (26, 17), (26, 18), (25, 19), (24, 20), (24, 21), (24, 24)):
            d.point((x, y), fill=rgba((0, 235, 255)))
    elif who == "chip":
        d.rectangle([17, 40, 32, 49], fill=rgba((30, 30, 40)))
        d.line([(24, 40), (24, 49)], fill=rgba(trim))
        d.pieslice([12, 7, 37, 27], 180, 360, fill=rgba(hair))
        d.polygon([(12, 17), (14, 5), (19, 10), (22, 2), (26, 9), (31, 1), (33, 9), (38, 6), (37, 17)], fill=rgba(hair))
        d.rectangle([12, 14, 14, 21], fill=rgba(hair))
        d.line([(26, 5), (30, 3)], fill=rgba((80, 80, 110)))
        d.line([(16, 18), (22, 20)], fill=rgba(hair))
        d.line([(27, 20), (33, 18)], fill=rgba(hair))
        d.rectangle([17, 21, 21, 23], fill=rgba((250, 250, 250)))
        d.rectangle([28, 21, 32, 23], fill=rgba((250, 250, 250)))
        d.rectangle([19, 21, 20, 23], fill=rgba((40, 90, 60)))
        d.rectangle([30, 21, 31, 23], fill=rgba((40, 90, 60)))
        d.line([(21, 30), (27, 30)], fill=rgba((120, 50, 45)))
        d.line([(27, 30), (29, 28)], fill=rgba((120, 50, 45)))
    else:
        stubble = (88, 70, 62)
        beard = (48, 38, 36)
        d.chord([12, 8, 37, 32], 180, 360, fill=rgba(stubble))
        d.rectangle([12, 16, 14, 22], fill=rgba(stubble))
        d.rectangle([35, 16, 37, 22], fill=rgba(stubble))
        d.ellipse([19, 13, 30, 19], fill=rgba(skin))
        d.point((17, 16), fill=rgba(skin))
        d.point((32, 16), fill=rgba(skin))
        for px, py in ((18, 11), (23, 10), (28, 11), (20, 14), (31, 13)):
            d.point((px, py), fill=rgba((120, 100, 90)))
        d.chord([13, 10, 36, 36], 22, 158, fill=rgba(beard))
        d.rectangle([13, 21, 14, 28], fill=rgba(beard))
        d.rectangle([35, 21, 36, 28], fill=rgba(beard))
        d.line([(20, 27), (28, 27)], fill=rgba(beard))
        d.chord([20, 28, 29, 32], 0, 180, fill=rgba((100, 30, 35)))
        d.line([(21, 30), (28, 30)], fill=rgba((245, 245, 245)))
        d.line([(16, 18), (21, 18)], fill=rgba(beard))
        d.line([(28, 18), (33, 18)], fill=rgba(beard))
        d.rectangle([15, 20, 22, 23], outline=rgba((150, 150, 165)))
        d.rectangle([27, 20, 34, 23], outline=rgba((150, 150, 165)))
        d.line([(23, 21), (26, 21)], fill=rgba((150, 150, 165)))
        d.rectangle([18, 21, 19, 22], fill=rgba((40, 30, 30)))
        d.rectangle([30, 21, 31, 22], fill=rgba((40, 30, 30)))
        d.point((16, 21), fill=rgba((230, 240, 255)))
        d.point((28, 21), fill=rgba((230, 240, 255)))
        d.line([(15, 42), (21, 42)], fill=rgba((60, 210, 110)))
        d.line([(15, 44), (21, 44)], fill=rgba((0, 200, 230)))
        d.point((16, 41), fill=rgba((60, 210, 110)))
        d.point((18, 41), fill=rgba((60, 210, 110)))
        d.rectangle([34, 40, 42, 49], fill=rgba((235, 235, 240)))
        d.rectangle([42, 42, 44, 46], outline=rgba((235, 235, 240)))
        d.rectangle([35, 41, 41, 43], fill=rgba((90, 50, 30)))
        for y in (36, 33, 30):
            d.point((37 + (y % 2), y), fill=rgba((200, 200, 210), 180))

    if who in ("chip", "joyrider"):
        d.line([(24, 22), (24, 27)], fill=rgba(skin_dark))
    elif who == "carra":
        d.line([(24, 23), (24, 26)], fill=rgba(skin_dark))
    else:
        d.line([(24, 23), (24, 26)], fill=rgba(skin_dark))
    bg.alpha_composite(outline(fig))
    frame = ImageDraw.Draw(bg)
    frame.rectangle([0, 0, S - 1, S - 1], outline=rgba((8, 6, 16)))
    return bg.resize((PORTRAIT, PORTRAIT), Image.NEAREST)


if __name__ == "__main__":
    out_tex = os.path.join(ROOT, "assets", "textures")
    dialog_frame().save(os.path.join(out_tex, "TextureDialog.png"))
    portraits = Image.new("RGBA", (PORTRAIT * 5, PORTRAIT), (0, 0, 0, 0))
    for i, who in enumerate(("jastro", "carra", "chip", "joyrider", "unknown")):
        portraits.paste(portrait(who), (PORTRAIT * i, 0))
    portraits.save(os.path.join(out_tex, "TexturePortraits.png"))

    preview = Image.new("RGBA", (640, 360), (26, 22, 44, 255))
    frame = dialog_frame()
    preview.alpha_composite(frame, (310 - FRAME_W // 2, 279 - FRAME_H // 2))
    preview.alpha_composite(portraits.crop((200, 0, 300, 100)), (103 - 50, 268 - 50))
    preview.alpha_composite(portraits, (170, 20))
    os.makedirs(os.path.join(HERE, "out"), exist_ok=True)
    preview.resize((1280, 720), Image.NEAREST).save(os.path.join(HERE, "out", "_dialog.png"))
    print("OK")
