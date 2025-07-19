#!/bin/bash -ex

x16emu -scale 2 -bas <(printf 'LOAD"SEACOMBAT.BAS"\nRUN\n')
