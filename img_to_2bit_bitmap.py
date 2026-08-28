from PIL import Image


def fit_image(img, target_w, target_h, mode="cover", bg_color=255):
    "modes: cover, contain, stretch "
    src_w, src_h = img.size

    if mode == "stretch":
        return img.resize((target_w, target_h))

    src_ratio = src_w / src_h
    target_ratio = target_w / target_h

    if mode == "cover":
        if src_ratio > target_ratio:
            new_h = target_h
            new_w = int(src_ratio * new_h)
        else:
            new_w = target_w
            new_h = int(new_w / src_ratio)

        resized = img.resize((new_w, new_h))

        left = (new_w - target_w) // 2
        top = (new_h - target_h) // 2
        return resized.crop((left, top, left + target_w, top + target_h))

    elif mode == "contain":
        if src_ratio > target_ratio:
            new_w = target_w
            new_h = int(new_w / src_ratio)
        else:
            new_h = target_h
            new_w = int(src_ratio * new_h)

        resized = img.resize((new_w, new_h))

        canvas = Image.new("L", (target_w, target_h), bg_color)
        left = (target_w - new_w) // 2
        top = (target_h - new_h) // 2
        canvas.paste(resized, (left, top))
        return canvas

    else:
        raise ValueError(f"unknown mode: {mode}")


TARGET_W, TARGET_H = 400, 300
MODE = "contain"  # "cover" | "contain" | "stretch"

img = Image.open("/home/miocasa/Pictures/Twitter/20260309_011701.jpg").convert("L")  # .rotate(90)
img = fit_image(img, TARGET_W, TARGET_H, mode=MODE, bg_color=255)

w, h = img.size
pixels = list(img.get_flattened_data())

byte_width = (w + 3) // 4
data = bytearray(byte_width * h)

for j in range(h):
    for i in range(w):
        grey = pixels[j * w + i]
        level2 = grey >> 6
        shift = (3 - (i % 4)) * 2
        idx = j * byte_width + i // 4
        data[idx] |= (level2 << shift)

with open("src/image_2bpp.h", "w") as f:
    f.write(f"const unsigned char my_2bpp_bitmap[] = {{\n")
    f.write(", ".join(str(b) for b in data))
    f.write("\n};\n")
