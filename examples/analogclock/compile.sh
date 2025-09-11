#!/usr/bin/env bash

g++ main.cpp -I ../../src stb_image.o -lGL -lGLEW -lSDL3 -o analogclock ../../libkrill.a
