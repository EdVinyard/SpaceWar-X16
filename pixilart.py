'''
Convert a single frame (not animated) pixilart.com saved files (.pixil) to raw
sprite data files that can be loaded into Commander X16 video RAM.
'''
import base64
import io
import PIL.Image
import cx16palette

data = 'data:image/pngp98kjasdnasd983/24kasdjasdbase64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAAXNSR0IArs4c6QAAAKxJREFUOE+lk9ENgDAIRGUnR3AFncURnMWu4AjuVEOVSq9X00Q/kXvAQWVg3xZjFV5FWGoZZEJUAegFqHha7vRjp43loIPcAKtsAA/RmAdazhiStgRoxEN8HwjJAJy7BWBdrSIynHOsZmYQ5ks3gIm1yBhEsoHMdzTQ5/wGKCyN4NeIFVpbeMR8jSZCI9GH55jKS0QfWpdZXaIJ2U00KpuEvjC6ma7X+P2E6N8LtkpPEQNZb2EAAAAASUVORK5CYII='
mime_type, image_b64 = data.split(',')

if not mime_type.startswith('data:image/png'):
    raise ValueError(f'unexpected image data URL MIME type prefix: ${mime_type}')

## KLUDGE:  I'm not sure what the rest of the "prefix" in the `data:` 
## URL means.  How much of it is important?  I guess we'll find out 
## when this breaks.

image_bytes = base64.b64decode(image_b64, validate=True)
print(f'PNG file size: {len(image_bytes)} bytes')

image = PIL.Image.open(io.BytesIO(image_bytes))
print(image)

x16_bin_file_content: list[int] = []
for x in range(image.width):
    for y in range(image.height):
        ## KLUDGE: Why is it necessary to transpose the X and Y coordinates here?
        ## Is there a bug in my code, or is this some peculiarity of PixilArt.com?
        coord = (y, x)                 # (column, row)
        pixel = image.getpixel(coord)  # (alpha, red, green, blue)
        palette_index = cx16palette.rgba_to_index(pixel)
        x16_bin_file_content.append(palette_index)

        print(f'{palette_index:>4}', end='')

    print(end='\n')

x16_bin_file_name = 'EARTH.BIN'
print(f'Writing {len(x16_bin_file_content)} bytes to {x16_bin_file_name}... ', end='')
with open(x16_bin_file_name, 'wb') as f:
    f.write(bytes(x16_bin_file_content))

print('done.')
