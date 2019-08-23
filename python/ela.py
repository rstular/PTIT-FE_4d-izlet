import os
import time

from PIL import Image, ImageChops, ImageEnhance

QUALITY = 90

original_image = "image.jpg"
output_image = "out.jpg"
tmp_copy = "tmp.image.jpg"

im = Image.open(original_image)

im.save(tmp_copy, "JPEG", quality=QUALITY)

tmp_img = Image.open(tmp_copy)

ela_img = ImageChops.difference(im, tmp_img)

extrema = ela_img.getextrema()

max_diff = max([ex[1] for ex in extrema])

scale = 255.0 / max_diff

ela_img = ImageEnhance.Brightness(ela_img).enhance(scale)

os.remove(tmp_copy)

ela_img.save(output_image)