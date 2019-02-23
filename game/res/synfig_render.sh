#!/bin/bash

# $1 - to atlas
# $2 - sifz file
# $3 - action name
# $4 - start time
# $5 - end time
# $6 - fps
function RenderPart()
{
    rm -f ./$1/$2_$3*.png
    rm -f /tmp/sifz*.png
    synfig "./$2.sifz" -t png --start-time "$4" --end-time "$5" --fps "$6" -o "/tmp/sifz.png"
    NUM=0
    for FILE in /tmp/sifz*.png; do
        mv "$FILE" "./$1/$2_$3$NUM.png"
        NUM=$(expr $NUM + 1);
    done;
}

# $1 - to atlas
# $2 - sifz file
# $3 - start time
# $4 - end time
# $5 - fps
function RenderAll()
{
    rm -f ./$1/$2*.png
    rm -f /tmp/sifz*.png
    synfig "./$2.sifz" -t png --start-time "$3" --end-time "$4" --fps "$5" -o "/tmp/sifz.png"
    NUM=0
    for FILE in /tmp/sifz*.png; do
        mv "$FILE" "./$1/$2$NUM.png"
        NUM=$(expr $NUM + 1);
    done;
}

# $1 - to atlas
# $2 - sifz file
function RenderOne()
{
    rm -f ./$1/$2*.png
    rm -f /tmp/sifz*.png
    synfig "./$2.sifz" -t png --start-time "0f" --end-time "0f" --fps "0" -o "/tmp/sifz.png"
    for FILE in /tmp/sifz*.png; do
        mv "$FILE" "./$1/$2.png"
    done;
}

# shroom_cannon_red1
RenderPart common1 shroom_cannon_red1 attack "0f" "8f" "12"
RenderPart common1 shroom_cannon_red1 reload "9f" "14f" "12"
RenderOne common1 shroom_cannon_red1_missile

# shroom_guardian_red1
RenderOne common1 shroom_guardian_red1_head
RenderOne common1 shroom_guardian_red1_leg

# shroom_snake
RenderAll common1 shroom_snake_head "0f" "8f" "12"
RenderOne common1 shroom_snake_tail
