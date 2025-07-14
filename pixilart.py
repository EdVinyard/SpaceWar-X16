'''
Convert each frame and layer of a pixilart.com saved file (.pixil) to separate
raw sprite data file that can be loaded into Commander X16 video RAM.

usage:

    python3 pixilart.py <image-file.pixil>

'''
import sys
import os
import base64
import io
import json
import PIL.Image
import cx16palette

USAGE = __doc__

def export_layer(
        file_name_no_ext: str,
        frame_index: int,
        frame_name: str,
        layer_index: int,
        layer_name: str,
        data_uri: str,
        ) -> None:
    '''
    Export a single layer of a single frame of a .pixil file to a binary file
    that can be loaded directly into Commander X16 memory.
    '''
    print(f'\nprocessing {file_name_no_ext} '
          f'frame {frame_index} {frame_name} '
          f'layer {layer_index} {layer_name}...')
    mime_type, image_b64 = data_uri.split(',')

    # KLUDGE:  I'm not sure what the rest of the "prefix" in the `data:` URL
    # means, but it doesn't seem important (so far).  For example:
    #
    #     data:image/pngp98kjasdnasd983/24kasdjasdbase64,

    if not mime_type.startswith('data:image/png'):
        raise ValueError(f'unexpected image data URL MIME type prefix: ${mime_type}')

    image_bytes = base64.b64decode(image_b64, validate=True)
    print(f'\tPNG file size: {len(image_bytes)} bytes')

    image = PIL.Image.open(io.BytesIO(image_bytes))
    # print(image)

    x16_bin_file_content: list[int] = []
    for x in range(image.width):
        print('\t', end='')
        for y in range(image.height):
            # KLUDGE: Why is it necessary to transpose the X and Y coordinates
            # here? Is there a bug in my code, or is this some peculiarity of
            # PixilArt.com?
            coord = (y, x)                 # (column, row)
            pixel = image.getpixel(coord)  # (alpha, red, green, blue)
            palette_index = cx16palette.rgba_to_index(pixel)
            x16_bin_file_content.append(palette_index)

            print(f'{palette_index:>4}', end='')

        print(end='\n')

    frame_name = frame_name.replace(' ', '')
    layer_name = layer_name.replace(' ', '')
    x16_bin_file_name = f'{file_name_no_ext}_{frame_name}{layer_name}.bin'
    print(f'\twriting {len(x16_bin_file_content)} bytes to {x16_bin_file_name}... ', end='')
    with open(x16_bin_file_name, 'wb') as f:
        f.write(bytes(x16_bin_file_content))

    print('\tdone.')


def main(file_path):
    '''Export each frame and layer of the .pixil file to a CX16 binary file.'''
    _, file_name = os.path.split(file_path)      # example: earth.pixil
    name_no_ext, _ = os.path.splitext(file_name) # example: earth

    with open(file_path, 'r', encoding='utf-8') as pixil_file:
        pixil_json = json.load(pixil_file)

    for fi, frame in enumerate(pixil_json['frames']):
        frame_name = frame['name']
        for li, layer in enumerate(frame['layers']):
            layer_name = layer['name']
            data_uri = layer['src']
            export_layer(name_no_ext, fi, frame_name, li, layer_name, data_uri)


def print_usage_and_exit():
    '''print usage message and exit'''
    print(USAGE)
    sys.exit(1)


if __name__ == '__main__':
    if len(sys.argv) == 2:
        main(sys.argv[1])
    else:
        print_usage_and_exit()
