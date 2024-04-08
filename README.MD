Work-in-progress games for the [Commander X16]() in the spirit of
[Spacewar!](https://en.wikipedia.org/wiki/Spacewar!), written in
[BASIC](https://github.com/X16Community/x16-docs/blob/master/X16%20Reference%20-%2004%20-%20BASIC.md)
and C/assembly (using [CC65](https://cc65.github.io/doc/) ), along with
experiments and examples used to understand the hardware and compiler/assembler
behavior.

- [BASIC](./BASIC/) - a self-contained Spacewar! game written entirely in BASIC
- [KernalCall](./KernalCall/) - experiments with CC65 to call [Commander X16 Kernal routines](https://github.com/X16Community/x16-docs/blob/master/X16%20Reference%20-%2005%20-%20KERNAL.md)
- [SpriteLoad](./SpriteLoad/) - C program to load sprite data from disk and display it
- [SpriteSave](./SpriteSave/) - BASIC program to create a sprite and and save it to disk
- [cx16palette.py](./cx16palette.py) - Python 3 program that prints the default
Commander X16 pallete in a format suitable for the "Colors" > "Import Colors"
feature of the https://www.pixilart.com/ sprite editor.
