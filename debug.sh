#!/bin/sh

make clean

make

export LD_LIBRARY_PATH=/usr/local/Cellar/sfml/2.5.1_1/lib

./run ./games/Super_mario_brothers.nes