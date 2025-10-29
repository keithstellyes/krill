#!/usr/bin/env bash

g++ main.cpp -std=c++23 -I../../src -lGL -lGLEW -lSDL3 -lglfw ../../libkrill.a -o rotatingcube
