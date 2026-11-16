#!/usr/bin/env python3
# Sound effects that OceanStorm didn't have (sirens, cash, doors...), synthesized
# from sine waves and noise. The rest of the audio is OceanStorm's, copied as-is.
#
# Jastro's first siren: a recording of himself going "nee-naw" into a laptop mic.
# Carra: "we are not shipping that". Jastro: "it has soul".

import math, os, random, struct, wave

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "..", "assets", "sounds")
RATE = 44100
rnd = random.Random(32)


def save(name, samples):
    os.makedirs(OUT, exist_ok=True)
    with wave.open(os.path.join(OUT, name + ".wav"), "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(RATE)
        w.writeframes(b"".join(struct.pack("<h", int(max(-1, min(1, s)) * 30000)) for s in samples))
    print(name, len(samples) / RATE, "s")


def silence(seconds):
    return [0.0] * int(seconds * RATE)


def square(f, t):
    return 1.0 if math.sin(2 * math.pi * f * t) >= 0 else -1.0


def envelope(i, n, attack=0.01, release=0.3):
    t = i / n
    a = min(1.0, t / attack) if attack > 0 else 1.0
    r = min(1.0, (1 - t) / release) if release > 0 else 1.0
    return a * r


def tone(freq, seconds, wave_fn="square", volume=0.5, attack=0.01, release=0.3, slide=0.0):
    n = int(seconds * RATE)
    out = []
    phase = 0.0
    for i in range(n):
        f = freq + slide * i / n
        phase += f / RATE
        if wave_fn == "square":
            v = 1.0 if (phase % 1.0) < 0.5 else -1.0
        elif wave_fn == "tri":
            v = 4 * abs((phase % 1.0) - 0.5) - 1
        else:
            v = math.sin(2 * math.pi * phase)
        out.append(v * volume * envelope(i, n, attack, release))
    return out


def noise(seconds, volume=0.5, attack=0.005, release=0.8, smooth=1):
    n = int(seconds * RATE)
    out, acc = [], 0.0
    for i in range(n):
        acc += (rnd.uniform(-1, 1) - acc) / smooth
        out.append(acc * volume * envelope(i, n, attack, release))
    return out


def mix(*tracks):
    n = max(len(t) for t in tracks)
    return [sum(t[i] for t in tracks if i < len(t)) for i in range(n)]


def siren():
    period = 1.6
    n = int(period * RATE)
    out, phase = [], 0.0
    for i in range(n):
        t = i / RATE
        f = 800 + 180 * math.sin(2 * math.pi * t / period)
        phase += f / RATE
        s = math.sin(2 * math.pi * phase)
        out.append(max(-0.6, min(0.6, s * 1.6)) * 0.55)
    return out


def cash():
    return tone(1319, 0.07, volume=0.35, release=0.2) + tone(1760, 0.22, volume=0.35, release=0.8)


def reload():
    return noise(0.03, 0.7, release=0.9) + silence(0.12) + mix(noise(0.05, 0.8, release=0.9, smooth=2), tone(180, 0.05, "sine", 0.4))


def door():
    return mix(noise(0.35, 0.35, attack=0.3, release=0.6, smooth=18), silence(0.25) + tone(70, 0.15, "sine", 0.7, release=0.9))


def busted():
    return (tone(950, 0.18, volume=0.3, release=0.1) + tone(700, 0.18, volume=0.3, release=0.1) +
            tone(950, 0.18, volume=0.3, release=0.1) + tone(700, 0.18, volume=0.3, release=0.1) +
            tone(330, 0.5, "tri", 0.5, release=0.7, slide=-120))


def thud():
    return mix(tone(90, 0.28, "sine", 0.9, attack=0.002, release=0.9, slide=-40), noise(0.08, 0.6, release=0.9, smooth=3))


def wanted():
    return tone(880, 0.1, volume=0.3, release=0.2) + tone(1175, 0.2, volume=0.3, release=0.6)


if __name__ == "__main__":
    save("siren", siren())
    save("cash", cash())
    save("reload", reload())
    save("door", door())
    save("busted", busted())
    save("thud", thud())
    save("wanted", wanted())
