#!/bin/bash

g++ -g tris.cpp -o tris.exe \
  -IC:/msys64/ucrt64/include/SDL2 \
  -LC:/msys64/ucrt64/lib \
  -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -mwindows

if [ $? -eq 0 ]; then
  echo "Succesfull."
else
  echo "Error."
fi