#!/bin/env python3
'''
Show sprite frames in the Commander X16 emulator.

This Python script works by completing a Commander X16 BASIC program, writing it
to a temporary file, and using the emulator's `-bas` option to input the program
when it starts up.

usage:

  showsprt.bash <sprite-paths...>

where

  sprite-path: path of a headerless, palette-indexed 16x16 sprite file

'''
import os
import sys
import textwrap
import tempfile

USAGE = __doc__
DEBUG = True
BASIC_TEMPLATE = textwrap.dedent('''
    10 SCREEN $80
    20 COLOR 1,6: REM WHITE ON DARKISH BLUE
    30 CLS
    40 H = 1: REM SPRITE HI ADDRESS (0 OR 1)
    {filename_array}
    1160 FOR I = 1 TO {file_count}
    1170 S = I: REM SPRITE INDEX
    1180 L = $4000 + (256 * I): REM SPRITE LO ADDRESS
    1190 X = (16*I) AND 255: REM MOD 256
    1200 Y = INT(16*I / 256)
    1210 REM PRINT S; HEX$(L); X; Y; F$(I)
    1220 BVLOAD F$(I), 8, H, L
    1230 SPRMEM S, H, L, 1
    1240 REM    IDX PRI PAL FLP WID HEI COLOR
    1250 SPRITE   S,  3,  0,  0,  1,  1
    1260 MOVSPR S, X, Y
    1270 NEXT I
    2000 LOCATE 30,20
    2010 PRINT "ANY KEY TO POWEROFF"
    2020 GET K$: IF K$="" THEN 2020
    2030 POWEROFF
    RUN
    ''')


def debug_print(*args, **kwargs):
    '''
    A synonym for `print()` when `DEBUG == True`; otherwise a no-op.
    '''
    if DEBUG:
        print(*args, **kwargs)


def basic_filename_array(image_filenames):
    '''
    Returns an untokenized, ASCII fragment of Commander X16 BASIC to declare and
    initialize an array of strings containing the supplied sprite file names.
    '''
    size = len(image_filenames)
    lines = []
    lines.append(f'100 DIM F$({size})')

    for i, filename in enumerate(image_filenames):
        line_number = 110 + 10*i
        lines.append(f'{line_number} F$({i+1}) = "{filename.upper()}"')

    return '\n'.join(lines)


def ascii_basic_program(image_filenames):
    '''
    Returns an complete, untokenized, ASCII Commander X16 BASIC program that
    displays all of the specified binary image files as sprites tiled on the
    screen.
    '''
    return BASIC_TEMPLATE.format(
        filename_array=basic_filename_array(image_filenames),
        file_count=len(image_filenames))


def show_sprites(filenames):
    '''
    Using the Commander X16 emulator, display all of the specified binary image
    files as sprites tiled on the screen.
    '''
    with tempfile.NamedTemporaryFile('w', delete=False) as basic_file:
        basic_program = ascii_basic_program(filenames)
        debug_print(basic_program)
        basic_file.write(basic_program)
        basic_file.close()
        os.system(f'x16emu -scale 2 -bas {basic_file.name} -echo iso')


def main(args):
    '''command line interface'''
    if len(args) < 2:
        print(USAGE)
        sys.exit(1)

    show_sprites(args[1:])


if __name__ == '__main__':
    main(sys.argv)
