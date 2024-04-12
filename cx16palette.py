'''
As a program, prints the default Commander X16 pallete in a format suitable for
the "Colors" > "Import Colors" feature of the https://www.pixilart.com/ sprite
editor.

As a library, import 
'''

## from https://github.com/fvdhoef/vera-module/blob/rev4/doc/VERA%20Programmer's%20Reference.md#palette
three_byte_color_strings = '''
000,fff,800,afe,c4c,0c5,00a,ee7,d85,640,f77,333,777,af6,08f,bbb
000,111,222,333,444,555,666,777,888,999,aaa,bbb,ccc,ddd,eee,fff
211,433,644,866,a88,c99,fbb,211,422,633,844,a55,c66,f77,200,411
611,822,a22,c33,f33,200,400,600,800,a00,c00,f00,221,443,664,886
aa8,cc9,feb,211,432,653,874,a95,cb6,fd7,210,431,651,862,a82,ca3
fc3,210,430,640,860,a80,c90,fb0,121,343,564,786,9a8,bc9,dfb,121
342,463,684,8a5,9c6,bf7,120,241,461,582,6a2,8c3,9f3,120,240,360
480,5a0,6c0,7f0,121,343,465,686,8a8,9ca,bfc,121,242,364,485,5a6
6c8,7f9,020,141,162,283,2a4,3c5,3f6,020,041,061,082,0a2,0c3,0f3
122,344,466,688,8aa,9cc,bff,122,244,366,488,5aa,6cc,7ff,022,144
166,288,2aa,3cc,3ff,022,044,066,088,0aa,0cc,0ff,112,334,456,668
88a,9ac,bcf,112,224,346,458,56a,68c,79f,002,114,126,238,24a,35c
36f,002,014,016,028,02a,03c,03f,112,334,546,768,98a,b9c,dbf,112
324,436,648,85a,96c,b7f,102,214,416,528,62a,83c,93f,102,204,306
408,50a,60c,70f,212,434,646,868,a8a,c9c,fbe,211,423,635,847,a59
c6b,f7d,201,413,615,826,a28,c3a,f3c,201,403,604,806,a08,c09,f0b
'''


def flatten(arrays):
    result = []
    for a in arrays:
        result.extend(a)
    return result


def color_12bit_to_24bit(twelve_bit_triple: str) -> str:
    '''
    Given a 12-bit hexidecimal RGB triple (e.g., "07f"),
    returns the equivalent 24-bit RGB triple (e.g., "0077ff").
    '''
    return ''.join( c+c for c in twelve_bit_triple ) # example: '07f' => '0077ff'


def color_12bit_to_argb(twelve_bit_triple: str) -> tuple[int, int, int, int]:
    '''
    Given a 12-bit hexidecimal RGB triple (e.g., "18f"),
    returns the equivalent 24-bit RGB triple (e.g., `(0, 17, 136, 255)`).
    '''
    r, g, b = ( int(c+c, base=16) for c in twelve_bit_triple )

    if r == g == b == 0:
        return (0, 0, 0, 0)

    return (r, g, b, 255)


## Replace line breaks with ','.
comma_separated_color_triples = ','.join( 
    l 
    for l 
    in three_byte_color_strings.splitlines() 
    if l != '' 
    )

comma_separated_color_triple_strs = comma_separated_color_triples.split(',')     

## Convert colors from 12-bit to 24-bit hexidecimal notation.
palette = [
    color_12bit_to_24bit(t) 
    for t 
    in comma_separated_color_triple_strs
    ]

rgba_palette: list[tuple[int, int, int]] = [ 
    color_12bit_to_argb(color_triple_str)
    for color_triple_str
    in comma_separated_color_triple_strs
    ]


def hexify(rgba: tuple[int, int, int, int]) -> str:
    '''
    Given a tuple of 8-bit integers representing alpha, red, green, and blue
    pixel values, return the (somewhat) human-friendly string of hexidecimal
    values.

    Example: given (8, 9, 10, 11) returns "08090a0b"
    '''
    return ''.join(
        f'{hex(i).replace('0x', ''):0>2}'
        for i in rgba
        )


def rgba_to_index(rgba: tuple[int, int, int, int]) -> int:
    '''
    Given a tuple of 8-bit integers representing alpha, red, green, and blue
    pixel values, either return the corresponding index into the Commander X16's
    default palette or throw `ValueError`.
    '''
    try:
        return rgba_palette.index(rgba)
    except ValueError:
        raise ValueError(
            f'pixel ARGB value {hexify(rgba)} / {rgba} is not part of '
            f'the default Commander X16 color palette')


def print_pixelart_palette():
    '''
    prints the default Commander X16 pallete in a format suitable for the
    "Colors" > "Import Colors" feature of the https://www.pixilart.com/ sprite
    editor.
    '''    
    print(','.join(palette))


if __name__ == '__main__':
    print_pixelart_palette()
