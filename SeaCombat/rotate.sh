#!/bin/bash

COMPONENT=$1 # e.g., "hull", "sail"
PREREQUISITES=(${COMPONENT}00n.bin ${COMPONENT}01nne.bin ${COMPONENT}02ne.bin)
MIRROR=../mirror.py

for prereq in $PREREQUISITES; do
    if [ ! -f $prereq ]; then
        echo "ERROR: missing seed file ${prereq}"
        exit 1
        fi
    done

$MIRROR    diagonal    ${COMPONENT}01nne.bin   ${COMPONENT}03ene.bin
$MIRROR    diagonal    ${COMPONENT}00n.bin     ${COMPONENT}04e.bin
$MIRROR    vertical    ${COMPONENT}03ene.bin   ${COMPONENT}05ese.bin
$MIRROR    vertical    ${COMPONENT}02ne.bin    ${COMPONENT}06se.bin
$MIRROR    vertical    ${COMPONENT}01nne.bin   ${COMPONENT}07sse.bin
$MIRROR    vertical    ${COMPONENT}00n.bin     ${COMPONENT}08s.bin
$MIRROR    horizontal  ${COMPONENT}07sse.bin   ${COMPONENT}09ssw.bin
$MIRROR    horizontal  ${COMPONENT}06se.bin    ${COMPONENT}10sw.bin
$MIRROR    horizontal  ${COMPONENT}05ese.bin   ${COMPONENT}11wsw.bin
$MIRROR    horizontal  ${COMPONENT}04e.bin     ${COMPONENT}12w.bin
$MIRROR    horizontal  ${COMPONENT}03ene.bin   ${COMPONENT}13wnw.bin
$MIRROR    horizontal  ${COMPONENT}02ne.bin    ${COMPONENT}14nw.bin
$MIRROR    horizontal  ${COMPONENT}01nne.bin   ${COMPONENT}15nnw.bin
