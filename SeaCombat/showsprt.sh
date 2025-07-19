#!/bin/bash -e

function usage() {
    echo
    echo "Show a single sprite frame in the Commander X16 emulator."
    echo
    echo "usage:"
    echo
    echo "  showsprt.bash <sprite-path>"
    echo
    echo "where"
    echo
    echo "  sprite-path: path of a headerless, palette-indexed sprite file"
    echo
}

function show_sprite() {
    # upper-case the filename
    export SPRITE_FILENAME=$(echo $1 | tr a-z A-Z)

    # Inject the filename into the BASIC template.
    BAS_FILE=$(mktemp)
    cat showsprt.bas.txt | envsubst > $BAS_FILE

    # Run the (ASCII) BASIC program in the X16 emulator.
    x16emu -scale 2 -bas $BAS_FILE -echo iso
}

if [ "$#" -ne 1 ]; then
    usage
    exit 1
    fi

show_sprite $1
