#!/usr/bin/env python3
# Weapons atlas. Top half: 100% OceanStorm assets, 0% new pixels.
#
# Jastro's estimate: "porting the weapons will take two sprints"
# Actual time: one Image.paste() per sprite
# Jastro's standup: "the weapons were a huge effort but I pulled through"
#
# Bottom half (ammo, cash, shop sign, wanted badge): drawn by Carra,
# because OceanStorm had no shops. Nor money. Nor police. Nor laws.

import os
from PIL import Image, ImageDraw, ImageFilter
from gen_intro import text

HERE = os.path.dirname(os.path.abspath(__file__))
SRC = os.path.join(HERE, "src")
ATLAS = os.path.join(HERE, "..", "assets", "textures", "TextureWeapons.png")

atlas = Image.new("RGBA", (256, 160), (0, 0, 0, 0))
atlas.paste(Image.open(os.path.join(SRC, "oceanstorm_bullet.png")).convert("RGBA"), (0, 0))
atlas.paste(Image.open(os.path.join(SRC, "oceanstorm_enemyBullet.png")).convert("RGBA"), (8, 0))
atlas.paste(Image.open(os.path.join(SRC, "oceanstorm_soldierBlood.png")).convert("RGBA"), (0, 16))
atlas.paste(Image.open(os.path.join(SRC, "oceanstorm_pickups.png")).convert("RGBA"), (96, 16))
# the OceanStorm explosion. Used to blow up planes over the sea. Now it blows up taxis. Same 7 frames.
atlas.paste(Image.open(os.path.join(SRC, "oceanstorm_explosion.png")).convert("RGBA"), (0, 128))


def glow(img, color, radius=2):
    halo = Image.new("RGBA", img.size, (0, 0, 0, 0))
    mask = img.getchannel("A").filter(ImageFilter.MaxFilter(radius * 2 + 1)).point(lambda a: a * 90 // 255)
    halo.paste(Image.new("RGBA", img.size, color + (255,)), (0, 0), mask)
    halo.alpha_composite(img)
    return halo


ammo = Image.new("RGBA", (24, 20), (0, 0, 0, 0))
d = ImageDraw.Draw(ammo)
d.rectangle([2, 5, 21, 17], fill=(60, 72, 40, 255), outline=(20, 24, 14, 255))
d.rectangle([2, 5, 21, 8], fill=(80, 96, 52, 255), outline=(20, 24, 14, 255))
for i in range(4):
    x = 5 + i * 4
    d.rectangle([x, 1, x + 2, 5], fill=(230, 180, 60, 255))
    d.point((x + 1, 1), fill=(255, 240, 180, 255))
d.line([5, 12, 18, 12], fill=(255, 225, 60, 255))
d.line([5, 14, 18, 14], fill=(255, 225, 60, 255))
atlas.paste(glow(ammo, (255, 225, 60), 1), (96, 66))

cash = Image.new("RGBA", (24, 16), (0, 0, 0, 0))
d = ImageDraw.Draw(cash)
d.rectangle([1, 3, 22, 13], fill=(40, 150, 70, 255), outline=(10, 60, 25, 255))
d.rectangle([3, 5, 20, 11], outline=(120, 230, 140, 255))
d.ellipse([9, 5, 14, 11], fill=(20, 90, 40, 255))
d.line([11, 5, 11, 11], fill=(180, 255, 190, 255))
d.line([10, 6, 13, 6], fill=(180, 255, 190, 255))
d.line([10, 8, 12, 8], fill=(180, 255, 190, 255))
d.line([10, 10, 13, 10], fill=(180, 255, 190, 255))
atlas.paste(glow(cash, (80, 255, 120), 1), (124, 66))

badge = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
d = ImageDraw.Draw(badge)
d.polygon([(8, 0), (10, 5), (15, 6), (11, 9), (12, 15), (8, 12), (4, 15), (5, 9), (1, 6), (6, 5)],
          fill=(255, 255, 255, 255), outline=(120, 120, 140, 255))
atlas.paste(badge, (152, 66))

sign = Image.new("RGBA", (56, 26), (0, 0, 0, 0))
d = ImageDraw.Draw(sign)
d.rounded_rectangle([3, 3, 52, 22], radius=3, fill=(24, 8, 16, 255), outline=(255, 50, 80, 255), width=1)
text(sign, 13, 8, "AMMO", (255, 70, 100), 2)
atlas.paste(glow(sign, (255, 40, 80), 2), (96, 90))

smoke = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
sp = smoke.load()
for y in range(16):
    for x in range(16):
        r = ((x - 7.5) ** 2 + (y - 7.5) ** 2) ** 0.5 / 8
        a = max(0.0, 1 - r) ** 1.2
        sp[x, y] = (255, 255, 255, int(a * 200))
atlas.paste(smoke, (176, 66))

atlas.save(ATLAS)
print("Atlas ->", os.path.normpath(ATLAS), atlas.size)
