#!/bin/bash -ex

x16emu -warp -echo iso -bas
  <(printf 'BASLOAD"SEACOMBAT.BASL"\nSAVE"@:SEACOMBAT.BAS"\nPOWEROFF\n')
