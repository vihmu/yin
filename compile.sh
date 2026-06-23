#!/bin/sh
if command -v c++; then
  c++ yin.cpp -o yin -Wall -Wextra -Wpedantic -O2
fi
