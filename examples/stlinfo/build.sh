#!/usr/bin/env bash

g++ main.cpp -std=c++23 -I../../src -lGL -lGLEW -lSDL3 ../../libkrill.a -o stlinfo
