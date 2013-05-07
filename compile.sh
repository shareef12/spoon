#!/bin/sh

gcc gui.c -o gui `pkg-config --cflags --libs gtk+-3.0`
gcc spoon.c -o spoon -lcrypto
gcc exif.c -o exif
