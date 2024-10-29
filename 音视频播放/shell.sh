#!/bin/sh
arm-linux-gnueabihf-gcc -o main main.c touch.c screen.c video.c -I /home/langzx/linux/tools/tslib/include/ ./include/screen.h -L /home/langzx/linux/tools/tslib/lib/  -lts -lpthread
