s = open("weights/hw/test_image.mem").read().strip()
v = int(s, 2)
words = [(v >> (32 * k)) & 0xFFFFFFFF for k in range(25)]
print("static const u32 test_img[25] = {" + ", ".join(f"0x{w:08X}" for w in words) + "};")