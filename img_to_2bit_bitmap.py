from PIL import Image

img = Image.open("image.jpg").convert("L").resize((400, 300))
w, h = img.size
pixels = list(img.getdata())

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
