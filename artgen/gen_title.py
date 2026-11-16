#!/usr/bin/env python3

import os, re, math
from PIL import Image, ImageDraw, ImageFilter

from gen_intro import FONT, rgba, mix

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..")
T = 80
SW, SH = 640, 360

PINK = (255, 40, 170)
CYAN = (0, 235, 255)
YELLOW = (255, 225, 60)


def city_view():
    tiles = Image.open(os.path.join(ROOT, "assets", "textures", "TextureGround.png")).convert("RGBA")
    sprites = Image.open(os.path.join(ROOT, "assets", "textures", "TextureSprites.png")).convert("RGBA")
    tmx = open(os.path.join(ROOT, "assets", "maps", "TileMapCity.tmx")).read()
    layers = {n: [[int(v) for v in r.strip().rstrip(",").split(",")] for r in d.strip().split("\n")]
              for n, d in re.findall(r'name="(\w+)"[^>]*>\s*<data encoding="csv">(.*?)</data>', tmx, re.S)}
    H = len(layers["MapGround"])
    W = len(layers["MapGround"][0])
    city = Image.new("RGBA", (W * T, H * T), (12, 10, 22, 255))

    def draw(layer):
        for y, row in enumerate(layers[layer]):
            for x, g in enumerate(row):
                if g > 1:
                    r, c = divmod(g - 1, 10)
                    city.alpha_composite(tiles.crop((c * T, r * T, c * T + T, r * T + T)), (x * T, y * T))

    draw("MapGround")
    for tx, ty, a, t in ((11.0, 6.3, math.pi, 2), (13.0, 8.2, 0, 4), (16.0, 5.0, math.pi / 2, 3), (18.5, 4.0, -math.pi / 2, 1)):
        car = sprites.crop((t * 40, 0, t * 40 + 40, 76)).rotate(-math.degrees(a), expand=True)
        city.alpha_composite(car, (int(tx * T - car.width / 2), int(ty * T - car.height / 2)))
    draw("MapRoofs")
    return city.crop((int(10.5 * T), int(3.2 * T), int(10.5 * T) + SW, int(3.2 * T) + SH))


def big_text(img, x, y, s, color, scale, shadow=True):
    layer = Image.new("RGBA", img.size, (0, 0, 0, 0))
    d = ImageDraw.Draw(layer)
    cx = x
    for ch in s:
        g = FONT.get(ch, FONT[" "])
        for r, row in enumerate(g):
            for c, bit in enumerate(row):
                if bit == "1":
                    t = r / 4
                    col = mix(mix(color, (255, 255, 255), 0.55), color, t)
                    d.rectangle([cx + c * scale, y + r * scale, cx + c * scale + scale - 1, y + r * scale + scale - 1], fill=rgba(col))
        cx += (len(g[0]) + 1) * scale
    alpha = layer.getchannel("A")
    glow = Image.new("RGBA", img.size, (0, 0, 0, 0))
    glow.paste(Image.new("RGBA", img.size, rgba(color, 255)), (0, 0), alpha)
    glow = glow.filter(ImageFilter.GaussianBlur(scale * 0.9))
    if shadow:
        sh = Image.new("RGBA", img.size, (0, 0, 0, 0))
        sh.paste(Image.new("RGBA", img.size, (8, 4, 16, 255)), (scale // 2 + 2, scale // 2 + 2), alpha)
        img.alpha_composite(sh)
    img.alpha_composite(glow)
    img.alpha_composite(glow)
    img.alpha_composite(layer)
    return cx - x - scale


def width_of(s, scale):
    return sum((len(FONT.get(ch, FONT[" "])[0]) + 1) * scale for ch in s) - scale


def build():
    img = city_view()
    dark = Image.new("RGBA", img.size, (10, 6, 22, 150))
    img.alpha_composite(dark)
    vign = Image.new("L", img.size, 0)
    vd = ImageDraw.Draw(vign)
    for i in range(60):
        vd.rectangle([i * 3, i * 2, SW - 1 - i * 3, SH - 1 - i * 2], outline=int(170 * (1 - i / 60)))
    img.paste(Image.new("RGBA", img.size, (4, 2, 10, 255)), (0, 0), vign)

    s1 = "PIXEL"
    s2 = "HUSTLER"
    scale = 9
    big_text(img, (SW - width_of(s1, scale)) // 2, 26, s1, CYAN, scale)
    big_text(img, (SW - width_of(s2, scale)) // 2, 26 + 6 * scale, s2, PINK, scale)
    sub = "VIRCON WARS"
    big_text(img, (SW - width_of(sub, 4)) // 2, 26 + 12 * scale + 6, sub, YELLOW, 4)

    d = ImageDraw.Draw(img)
    y = 26 + 12 * scale + 6 + 30
    d.line([(150, y), (490, y)], fill=rgba(CYAN, 160))

    img.save(os.path.join(ROOT, "assets", "textures", "TextureTitle.png"))
    return img


if __name__ == "__main__":
    img = build()
    img.resize((SW * 2, SH * 2), Image.NEAREST).save(os.path.join(HERE, "out", "_title.png"))
    print("OK")
