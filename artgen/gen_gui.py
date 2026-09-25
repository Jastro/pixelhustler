#!/usr/bin/env python3
# OceanStorm's dialog frame: 552 x 124. Portraits: 100 x 100.
# Same sizes. Same layout. Different pixels.
# Jastro: "so it's a new dialog system"
# Carra: "it's the old one with a purple coat of paint"

import os
from PIL import Image, ImageDraw, ImageFilter

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..")

FRAME_W, FRAME_H = 552, 124
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
    d.rectangle([4, 4, FRAME_W - 5, FRAME_H - 5], fill=rgba(DARK, 240))
    for y in range(6, FRAME_H - 6, 3):
        d.line([(6, y), (FRAME_W - 7, y)], fill=rgba((22, 18, 40), 240))
    glow_rect(img, [4, 4, FRAME_W - 5, FRAME_H - 5], CYAN, 2, 3)

    d.rectangle([10, 10, 117, FRAME_H - 11], fill=rgba(PANEL))
    glow_rect(img, [10, 10, 117, FRAME_H - 11], PINK, 1, 2)
    d.rectangle([124, 10, FRAME_W - 11, FRAME_H - 11], fill=rgba(PANEL, 200))

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
        jacket, trim, hair = (20, 170, 80), (170, 255, 120), (70, 40, 20)
    else:
        synth_background(bg, (10, 30, 60), (30, 60, 110), (0, 235, 255))
        jacket, trim, hair = (70, 70, 88), (0, 235, 255), (22, 20, 24)

    d.pieslice([4, 36, 45, 70], 180, 360, fill=rgba(jacket))
    d.line([(10, 42), (24, 49)], fill=rgba(trim))
    d.line([(39, 42), (25, 49)], fill=rgba(trim))
    d.rectangle([20, 32, 29, 40], fill=rgba(skin_dark))
    d.ellipse([13, 10, 36, 36], fill=rgba(skin))
    d.ellipse([28, 12, 36, 34], fill=rgba(skin_dark))
    d.rectangle([12, 21, 14, 26], fill=rgba(skin_dark))
    d.rectangle([35, 21, 37, 26], fill=rgba(skin_dark))

    if who == "jastro":
        d.pieslice([12, 6, 37, 26], 180, 360, fill=rgba(hair))
        d.polygon([(14, 16), (20, 4), (26, 12), (31, 3), (36, 16)], fill=rgba(hair))
        d.line([(19, 8), (24, 5)], fill=rgba(shade(hair, 1.8)))
        d.rectangle([15, 19, 34, 24], fill=rgba((20, 16, 30)))
        d.rectangle([16, 20, 23, 23], fill=rgba(PINK))
        d.rectangle([26, 20, 33, 23], fill=rgba(PINK))
        d.line([(17, 20), (19, 20)], fill=rgba((255, 200, 235)))
        d.line([(27, 20), (29, 20)], fill=rgba((255, 200, 235)))
        d.chord([18, 25, 31, 33], 0, 180, fill=rgba((90, 20, 30)))
        d.rectangle([20, 29, 29, 30], fill=rgba((250, 250, 250)))
        d.point((33, 7), fill=rgba((255, 255, 180)))
        d.point((34, 6), fill=rgba((255, 255, 255)))
        d.point((35, 7), fill=rgba((255, 255, 180)))
        d.point((34, 8), fill=rgba((255, 255, 180)))
    else:
        d.pieslice([12, 8, 37, 28], 180, 360, fill=rgba(hair))
        d.rectangle([13, 14, 16, 22], fill=rgba(hair))
        d.rectangle([33, 14, 36, 22], fill=rgba(hair))
        d.arc([9, 6, 40, 34], 190, 350, fill=rgba((40, 40, 55)), width=2)
        d.rectangle([7, 18, 12, 28], fill=rgba((40, 40, 55)))
        d.rectangle([37, 18, 42, 28], fill=rgba((40, 40, 55)))
        d.rectangle([8, 20, 11, 26], fill=rgba(CYAN))
        d.rectangle([38, 20, 41, 26], fill=rgba(CYAN))
        d.line([(17, 18), (22, 19)], fill=rgba(hair))
        d.line([(27, 19), (32, 18)], fill=rgba(hair))
        d.rectangle([18, 21, 21, 22], fill=rgba((250, 250, 250)))
        d.rectangle([28, 21, 31, 22], fill=rgba((250, 250, 250)))
        d.point((20, 22), fill=rgba((30, 30, 40)))
        d.point((30, 22), fill=rgba((30, 30, 40)))
        d.line([(18, 24), (21, 24)], fill=rgba((150, 100, 140)))
        d.line([(28, 24), (31, 24)], fill=rgba((150, 100, 140)))
        d.line([(21, 30), (28, 30)], fill=rgba((120, 60, 60)))
        d.rectangle([34, 40, 42, 49], fill=rgba((235, 235, 240)))
        d.rectangle([42, 42, 44, 46], outline=rgba((235, 235, 240)))
        d.rectangle([35, 41, 41, 43], fill=rgba((90, 50, 30)))
        for y in (36, 33, 30):
            d.point((37 + (y % 2), y), fill=rgba((200, 200, 210), 180))

    d.line([(24, 22), (24, 27)], fill=rgba(skin_dark))
    bg.alpha_composite(outline(fig))
    frame = ImageDraw.Draw(bg)
    frame.rectangle([0, 0, S - 1, S - 1], outline=rgba((8, 6, 16)))
    return bg.resize((PORTRAIT, PORTRAIT), Image.NEAREST)


if __name__ == "__main__":
    out_tex = os.path.join(ROOT, "assets", "textures")
    dialog_frame().save(os.path.join(out_tex, "TextureDialog.png"))
    portraits = Image.new("RGBA", (PORTRAIT * 2, PORTRAIT), (0, 0, 0, 0))
    portraits.paste(portrait("jastro"), (0, 0))
    portraits.paste(portrait("carra"), (PORTRAIT, 0))
    portraits.save(os.path.join(out_tex, "TexturePortraits.png"))

    preview = Image.new("RGBA", (640, 360), (26, 22, 44, 255))
    frame = dialog_frame()
    preview.alpha_composite(frame, (310 - FRAME_W // 2, 280 - FRAME_H // 2))
    preview.alpha_composite(portraits.crop((0, 0, 100, 100)), (95 - 50, 280 - 50))
    preview.alpha_composite(portraits, (220, 20))
    os.makedirs(os.path.join(HERE, "out"), exist_ok=True)
    preview.resize((1280, 720), Image.NEAREST).save(os.path.join(HERE, "out", "_dialog.png"))
    print("OK")
