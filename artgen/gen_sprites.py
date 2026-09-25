#!/usr/bin/env python3
# Previous art pipeline (Jastro): MS Paint, spray can tool, 800% zoom.
# Previous art pipeline output: one (1) brown rectangle labeled "car".
#
# Jastro's art direction document: "like GTA 2 but it's night and everything is neon"
# Jastro's art direction document, page 2: "you know. arcadepunk."
# Carra: "that's not a word"
# Jastro: "it is now"

import sys, os, random
from PIL import Image, ImageDraw, ImageFilter

OUT = sys.argv[1] if len(sys.argv) > 1 else "out"
os.makedirs(OUT, exist_ok=True)

CAR_W, CAR_H = 40, 76
PED = 32

HEADLIGHT = (200, 255, 255, 255)
TAILLIGHT = (255, 40, 140, 255)
GLASS = (18, 22, 48, 255)
GLASS_HI = (90, 220, 255, 255)
TIRE = (10, 10, 16, 255)


def shade(c, f):
    return tuple(max(0, min(255, int(v * f))) for v in c[:3]) + (255,)


def with_alpha(c, a):
    return c[:3] + (a,)


def draw_car(body, neon, kind="sedan"):
    img = Image.new("RGBA", (CAR_W, CAR_H), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    x0, x1 = 4, CAR_W - 5
    y0, y1 = 3, CAR_H - 4
    if kind == "sport":
        x0, x1 = 5, CAR_W - 6

    d.rounded_rectangle([x0 - 3, y0 - 2, x1 + 3, y1 + 2], radius=10, fill=with_alpha(neon, 50))
    d.rounded_rectangle([x0 - 2, y0 - 1, x1 + 2, y1 + 1], radius=9, fill=with_alpha(neon, 80))

    for wy in (y0 + 12, y1 - 20):
        d.rectangle([x0 - 2, wy, x0 + 1, wy + 9], fill=TIRE)
        d.rectangle([x1 - 1, wy, x1 + 2, wy + 9], fill=TIRE)

    d.rounded_rectangle([x0, y0, x1, y1], radius=7, fill=shade(body, 0.4))
    inner = Image.new("L", (CAR_W, CAR_H), 0)
    ImageDraw.Draw(inner).rounded_rectangle([x0 + 1, y0 + 1, x1 - 1, y1 - 1], radius=6, fill=255)
    w = x1 - x0
    for x in range(x0 + 1, x1):
        t = (x - x0) / w
        f = 0.7 + 0.6 * max(0, 1 - abs(t - 0.35) * 2.4)
        for y in range(y0 + 1, y1):
            if inner.getpixel((x, y)):
                img.putpixel((x, y), shade(body, f))

    d.line([(x0, y0 + 8), (x0, y1 - 8)], fill=neon)
    d.line([(x1, y0 + 8), (x1, y1 - 8)], fill=neon)

    cx0, cx1 = x0 + 5, x1 - 5
    if kind == "sport":
        ws_y, roof_y0, roof_y1, rw_y = y0 + 26, y0 + 33, y0 + 50, y0 + 51
    elif kind == "van":
        ws_y, roof_y0, roof_y1, rw_y = y0 + 8, y0 + 15, y1 - 6, y1 - 5
    else:
        ws_y, roof_y0, roof_y1, rw_y = y0 + 18, y0 + 27, y0 + 52, y0 + 53

    d.polygon([(cx0 - 1, roof_y0 - 1), (cx1 + 1, roof_y0 - 1), (cx1 - 1, ws_y), (cx0 + 1, ws_y)], fill=GLASS)
    d.line([(cx0 + 2, ws_y + 2), (cx0 + 7, ws_y + 2)], fill=GLASS_HI)
    d.rectangle([cx0, roof_y0, cx1, roof_y1], fill=shade(body, 0.95), outline=shade(body, 0.5))
    if kind != "van":
        d.polygon([(cx0, rw_y), (cx1, rw_y), (cx1 - 2, rw_y + 6), (cx0 + 2, rw_y + 6)], fill=GLASS)
    d.line([(cx0 - 2, roof_y0 + 1), (cx0 - 2, roof_y1 - 1)], fill=GLASS)
    d.line([(cx1 + 2, roof_y0 + 1), (cx1 + 2, roof_y1 - 1)], fill=GLASS)
    d.rectangle([x0 - 2, ws_y + 2, x0, ws_y + 3], fill=shade(body, 0.5))
    d.rectangle([x1, ws_y + 2, x1 + 2, ws_y + 3], fill=shade(body, 0.5))

    d.rectangle([x0 + 2, y0, x0 + 8, y0 + 2], fill=HEADLIGHT)
    d.rectangle([x1 - 8, y0, x1 - 2, y0 + 2], fill=HEADLIGHT)
    d.rectangle([x0 + 2, y1 - 1, x1 - 2, y1], fill=with_alpha(TAILLIGHT, 160))
    d.rectangle([x0 + 2, y1 - 2, x0 + 8, y1], fill=TAILLIGHT)
    d.rectangle([x1 - 8, y1 - 2, x1 - 2, y1], fill=TAILLIGHT)

    if kind == "taxi":
        d.rectangle([cx0 + 3, roof_y0 + 9, cx1 - 3, roof_y0 + 14], fill=(255, 250, 120, 255), outline=(40, 30, 0, 255))
        for i in range(cx0 + 4, cx1 - 3, 2):
            d.point((i, roof_y0 + 11), fill=(40, 30, 0, 255))
        for y in range(y0 + 22, y1 - 22, 4):
            d.point((x0 + 1, y), fill=(20, 20, 20, 255))
            d.point((x1 - 1, y), fill=(20, 20, 20, 255))
    if kind == "police":
        mid = (cx0 + cx1) // 2
        d.rectangle([cx0 - 1, roof_y0 + 7, mid, roof_y0 + 13], fill=(255, 30, 60, 255))
        d.rectangle([mid + 1, roof_y0 + 7, cx1 + 1, roof_y0 + 13], fill=(40, 120, 255, 255))
        d.rectangle([cx0, roof_y1 - 8, cx1, roof_y1 - 5], fill=(230, 230, 240, 255))
    if kind == "sport":
        m = (x0 + x1) // 2
        d.line([(m - 3, y0 + 3), (m - 3, y1 - 3)], fill=neon)
        d.line([(m + 3, y0 + 3), (m + 3, y1 - 3)], fill=neon)
    if kind == "van":
        d.rectangle([cx0 + 2, roof_y0 + 10, cx1 - 2, roof_y0 + 30], outline=neon)
        d.line([(cx0 + 4, roof_y0 + 20), (cx1 - 4, roof_y0 + 20)], fill=neon)
    return img


def draw_ped(jacket, neon, pants, hair, skin=(232, 180, 136), frame=0):
    fig = Image.new("RGBA", (PED, PED), (0, 0, 0, 0))
    d = ImageDraw.Draw(fig)
    cx, cy = PED // 2, PED // 2 + 1
    swing = [0, 1, 0, -1][frame]
    shoe = (15, 15, 20)

    for side, phase in ((-1, swing), (1, -swing)):
        foot_y = cy - phase * 8
        x0 = cx - 5 if side < 0 else cx + 2
        top, bottom = min(cy, foot_y), max(cy, foot_y)
        d.rectangle([x0, top - 1, x0 + 3, bottom + 1], fill=pants + (255,))
        d.rectangle([x0, foot_y - 2, x0 + 3, foot_y + 2], fill=shoe + (255,))

    for side, phase in ((-1, -swing), (1, swing)):
        hand_y = cy - phase * 6
        x0 = cx - 11 if side < 0 else cx + 8
        top, bottom = min(cy - 1, hand_y), max(cy - 1, hand_y)
        d.rectangle([x0, top, x0 + 3, bottom], fill=jacket + (255,))
        d.rectangle([x0, hand_y - 1, x0 + 3, hand_y + 1], fill=skin + (255,))

    d.ellipse([cx - 10, cy - 4, cx + 10, cy + 4], fill=jacket + (255,), outline=neon + (255,))
    d.arc([cx - 8, cy - 3, cx + 4, cy + 2], 200, 300, fill=shade(jacket, 1.5))
    d.ellipse([cx - 5, cy - 5, cx + 5, cy + 4], fill=hair + (255,))
    d.arc([cx - 3, cy - 3, cx + 2, cy + 1], 190, 280, fill=shade(hair, 1.8))
    d.line([(cx - 2, cy - 5), (cx + 2, cy - 5)], fill=skin + (255,))
    d.point((cx, cy - 6), fill=skin + (255,))

    alpha = fig.getchannel("A").point(lambda a: 255 if a > 0 else 0)
    outline_mask = alpha.filter(ImageFilter.MaxFilter(3))
    out = Image.new("RGBA", (PED, PED), (0, 0, 0, 0))
    glow = Image.new("RGBA", (PED, PED), (0, 0, 0, 0))
    g = ImageDraw.Draw(glow)
    g.ellipse([1, 2, PED - 2, PED - 1], fill=neon + (45,))
    g.ellipse([4, 5, PED - 5, PED - 4], fill=neon + (70,))
    out.alpha_composite(glow)
    out.paste(Image.new("RGBA", (PED, PED), (8, 6, 16, 255)), (0, 0), outline_mask)
    out.alpha_composite(fig)
    return out


cars = [
    ("car_red",    draw_car((150, 20, 60),  (255, 40, 170), "sedan")),
    ("car_blue",   draw_car((25, 35, 110),  (0, 235, 255),  "sedan")),
    ("car_taxi",   draw_car((250, 205, 0),  (255, 240, 90), "taxi")),
    ("car_police", draw_car((22, 22, 34),   (70, 120, 255), "police")),
    ("car_sport",  draw_car((95, 20, 150),  (170, 255, 0),  "sport")),
    ("car_van",    draw_car((55, 60, 75),   (255, 140, 0),  "van")),
]
peds = [
    ("ped_jastro", ((20, 190, 90),   (170, 255, 120), (30, 30, 60),  (70, 40, 20))),   # +10 charisma
    ("ped_carra",  ((70, 70, 85),    (0, 235, 255),   (25, 25, 30),  (20, 20, 20))),   # has not slept since OceanStorm
    ("ped_civil1", ((220, 30, 150),  (255, 140, 220), (40, 40, 50),  (250, 220, 60))),
    ("ped_civil2", ((240, 240, 250), (255, 60, 60),   (20, 50, 120), (140, 70, 30))),
]

for name, img in cars:
    img.save(f"{OUT}/{name}.png")
for name, (j, n, p, h) in peds:
    sheet = Image.new("RGBA", (PED * 4, PED), (0, 0, 0, 0))
    for f in range(4):
        sheet.paste(draw_ped(j, n, p, h, frame=f), (f * PED, 0))
    sheet.save(f"{OUT}/{name}.png")

W, H = 360, 230
bg = Image.new("RGBA", (W, H), (26, 22, 44, 255))
d = ImageDraw.Draw(bg)
rnd = random.Random(1)
for _ in range(1200):
    x, y = rnd.randrange(W), rnd.randrange(H)
    bg.putpixel((x, y), shade((26, 22, 44), rnd.choice([0.7, 1.35])))
d.rectangle([0, 150, W, H], fill=(48, 36, 70, 255))
for x in range(0, W, 16):
    d.line([(x, 151), (x, H)], fill=(38, 28, 58, 255))
d.line([(0, 150), (W, 150)], fill=(255, 40, 170, 255))
d.line([(0, 151), (W, 151)], fill=(255, 40, 170, 110))
for x in range(4, W, 24):
    d.rectangle([x, 96, x + 12, 97], fill=(0, 235, 255, 255))
    d.rectangle([x, 95, x + 12, 98], outline=(0, 235, 255, 70))
for i, (_, img) in enumerate(cars):
    bg.alpha_composite(img, (8 + i * 58, 8 if i % 2 == 0 else 14))
    if i == 3:
        bg.alpha_composite(img.rotate(-35, expand=True), (230, 100))
for i, (name, _) in enumerate(peds):
    sheet = Image.open(f"{OUT}/{name}.png")
    for f in range(4):
        bg.alpha_composite(sheet.crop((f * PED, 0, (f + 1) * PED, PED)), (8 + f * (PED + 6) + (i % 2) * 180, 156 + (i // 2) * (PED + 4)))
bg.resize((W * 4, H * 4), Image.NEAREST).save(f"{OUT}/_preview.png")
print("OK ->", OUT)

# WARNING: if you change the layout, update the regions in Definitions.h too.
# (Jastro changed it once. Every car became half a taxi and half a
#  pedestrian. He called it "a new vehicle class".)
ATLAS = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "assets", "textures", "TextureSprites.png")
atlas = Image.new("RGBA", (256, 80 + PED * len(peds)), (0, 0, 0, 0))
for i, (_, img) in enumerate(cars):
    atlas.paste(img, (i * CAR_W, 0))
for p, (name, _) in enumerate(peds):
    atlas.paste(Image.open(f"{OUT}/{name}.png"), (0, 80 + p * PED))
# Copied from OceanStorm's gui.png. Pixel by pixel. With love.
# Carra: "you could have drawn new ones"
# Jastro: "why? these ones already say ENTER"
signs = Image.open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "src", "oceanstorm_signs.png"))
atlas.paste(signs, (130, 82))
atlas.save(ATLAS)
print("Atlas ->", os.path.normpath(ATLAS), atlas.size)
