#!/usr/bin/env python3
'''
Mirror a binary Commander X16 image horizontally, vertically, or diagonally
(around y=x).  The image is assumed to be headerless and square (of equal height
and width) for use as an X16 sprite (i.e., 8x8, 16x16, 32x32, or 64x64).  See

    https://github.com/X16Community/x16-docs/blob/master/X16%20Reference%20-%2009%20-%20VERA%20Programmer's%20Reference.md#sprite-attributes

for details about how to use the resulting files.

Example:

    INPUT           HORIZONTALLY    VERTICALLY      DIAGONALLY
    1 2             2 1             3 4             4 2
    3 4             4 3             1 2             3 1

usage

    python3 mirror.py <mirror-direction> <input-file> <output-file>

where

    mirror-direction    one of "horizontal", "vertical", or "diagonal"; may be
                        abbreviated "h", "v", and "d", respectively

    input-file          path to Commander X16 binary image of equal height and
                        width

    output-file         this path will be overwritten with the mirrored file
'''

import math
import sys
from typing import Callable, Iterable, Sequence, TypeAlias

USAGE = __doc__
X16Image: TypeAlias = Sequence[int]
MirrorFunction: TypeAlias = Callable[[X16Image], X16Image]


def print_usage_and_exit():
    '''Print the usage message and exit with an error status.'''
    print(USAGE)
    sys.exit(1)


def square_dimension(image: X16Image) -> int:
    '''
    Assume `image` represents an image of equal width and height.  Calculate and
    return the width/height of the image.  Raise `ValueError` if the image
    cannot be square.
    '''
    image_len = len(image)

    if image_len == 0:
        raise ValueError('image cannot be empty')

    square_size = math.floor(math.sqrt(image_len))

    if square_size**2 != image_len:
        raise ValueError(f'image of {image_len} bytes cannot be square')

    return square_size


def mirror_horizontally(image: X16Image) -> X16Image:
    '''
    Mirror the image around a vertical line that bisects the image.  For
    example:

    ```
        INPUT       OUTPUT
        1 2 3       3 2 1
        4 5 6       6 5 4
        7 8 9       9 8 7
    ```
    '''
    dim = square_dimension(image)
    result = []
    for r in range(dim):
        for c in range(dim):
            mirrored_col = dim - c - 1
            index = r * dim + mirrored_col
            result.append(image[index])

    return result


def mirror_vertically(image: X16Image) -> X16Image:
    '''
    Mirror the image around a horizontal line that bisects the image.  For
    example:

    ```
        INPUT       OUTPUT
        1 2 3       7 8 9
        4 5 6       4 5 6
        7 8 9       1 2 3
    ```
    '''
    dim = square_dimension(image)
    result = []
    for original_row in range(dim):
        for c in range(dim):
            mirrored_row = dim - original_row - 1
            index = mirrored_row * dim + c
            result.append(image[index])

    return result


def mirror_diagonally(image: X16Image) -> X16Image:
    '''
    Mirror the image around a diagonal line that bisects the image, running from
    the bottom-left to the top-right.  For example:

    ```
        INPUT       OUTPUT
        1 2 3       9 6 3
        4 5 6       8 5 2
        7 8 9       7 4 1
    '''
    dim = square_dimension(image)
    result = []
    for mirrored_row in range(dim):
        for mirrored_col in range(dim):
            original_row = dim - mirrored_col - 1
            original_col = dim - mirrored_row - 1
            index = original_row * dim + original_col
            result.append(image[index])

    return result


MIRROR_FUNCTIONS: dict[str, MirrorFunction] = {
    'h': mirror_horizontally,
    'v': mirror_vertically,
    'd': mirror_diagonally,
    }
VALID_DIRECTIONS: Iterable[str] = MIRROR_FUNCTIONS.keys()


def mirror(image: X16Image, direction: str) -> X16Image:
    '''
    Mirror the image around a line that bisects the image.  The line is
    determined by the mirror `direction` specified, which must be one of
    "horizontal", "vertical", or "diagonal".
    '''
    if direction is None or len(direction) == 0:
        directions = ", ".join(VALID_DIRECTIONS)
        raise ValueError(f'direction must be one of {directions}')

    try:
        dir_char = direction.lower()[0] # 'v', 'h', or 'd'
        return MIRROR_FUNCTIONS[dir_char](image)
    except KeyError as err:
        raise ValueError(f'invalid direction "{direction}"') from err


def read_image(path: str) -> X16Image:
    '''
    Read a headerless X16 image from a binary file.
    '''
    with open(path, 'rb') as f:
        return f.read()


def write_image(path: str, image: X16Image) -> None:
    '''
    Write a headerless X16 image to a binary file.
    '''
    with open(path, 'wb') as f:
        f.write(bytes(image))


def main(args):
    '''command line interface; see `USAGE`'''
    if len(args) != 3:
        print_usage_and_exit()

    mirror_direction, original_image_path, mirrored_image_path = args
    original_image = read_image(original_image_path)
    mirrored_image = mirror(original_image, mirror_direction)
    write_image(mirrored_image_path, mirrored_image)


if __name__ == '__main__':
    main(sys.argv[1:])
