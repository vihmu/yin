#!/usr/bin/env bash
# yin
# Small terminal music player based on mpv
# https://github.com/vihmu/yin
# Copyright (c) 2026 vihmu
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# “Software”), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
# NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
# THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# Global variables
WIDTH="$(tput cols)"
HEIGHT="$(tput lines)"
INFO="welcome!"
files=()
pages=$((1))
sel=$((0))
nr=$((1))
total=$((0))

# Colors
BLK="\x1b[30m"
RED="\x1b[31m"
GRN="\x1b[32m"
YLL="\x1b[33m"
BLU="\x1b[34m"
MAG="\x1b[35m"
CYN="\x1b[36m"
WHT="\x1b[38m"
REV="\x1b[7m"
RES="\x1b[m"


move() {
  printf "\x1b[%s;%sH" "$2" "$1"
}

draw_bar() {
  move 1 $HEIGHT
  printf "\x1b[J$REV [${nr}/${pages}] $(($sel + 1))/${total} ${INFO} $(dirs +0) $RES"
}

list_dir() {
  # Reset values
  f=()
  d=()
  files=()
  total=$((0))
  # Get total count of files/dirs and store them in an array
  while IFS= read -r -d '' f; do
    if [ -d "$f" ]; then
      d+=("$f")
    else
      f+=("$f")
    fi
    total=$(( $total + 1 ))
  done < <(find . -maxdepth 1 -not -path '.' -print0)
  # Directories go first (but only if array is not empty)
  if [ "${#d[@]}" -gt 0 ]; then
    files=("${d[@]}""${f[@]}")
  else
    files=(${f[@]})
  fi
  pages=$(( $total / $(( $HEIGHT - 1 )) ))
}

draw_dir() {
  move 1 1
  for i in $(seq 0 ${#files[@]}); do
    # Color based on type (selected/file/directory)
    if [ $i -eq $sel ]; then
      printf "\x1b[$((i+1));3H$RED${files[i]}$RES"
    elif [ -d "${files[i]}" ]; then
      printf "\x1b[$((i+1));3H$BLU${files[i]}$RES"
    else
      printf "\x1b[$((i+1));3H${files[i]}"
    fi
  done
}

shuffle_songs() {
  loop=true
  trap loop=false SIGINT SIGTERM EXIT
  while $loop; do
    track="$(find . -type f -name '*.mp3' -o -name '*.m4a' | shuf -n 1)"
    if [ -z "$track" ]; then
      loop=false
      INFO="No files found."
    else
      printf "\n\n\t$RED${track}$RES\n\n"
      mpv --no-video "$track"
    fi
  done
}

enter() {
  trap '' SIGINT SIGTERM EXIT
  clear
  printf "\x1b[?25l"
}

leave() {
  printf "\x1b[?25h"
  exit 0
}

enter
list_dir
while true; do
  draw_dir
  draw_bar
  move 1 $HEIGHT

  read -n 1 -s key
  case "$key" in
    'q')
      clear
      leave
      ;;
    'k')
      if [ $sel -gt 0 ]; then 
        sel=$(( $sel - 1 ))
      fi
      ;;
    'j')
      if [ $sel -lt $(( $total - 1 )) ]; then 
        sel=$(( $sel + 1 ))
      fi
      ;;
    'l'|'L')
      if [ -d "${files[$sel]}" ]; then
        clear
        cd "${files[$sel]}"
        sel=$((0))
        list_dir
      else
        clear
        if [ "$key" = 'l' ]; then
          mpv --no-video "${files[$sel]}" || INFO="File format unknown."
        else
          mpv --no-video --loop-file=inf "${files[$sel]}" || INFO="File format unknown."
        fi
        clear
      fi
      ;;
    'h')
      clear
      cd ..
      sel=$((0))
      list_dir
      ;;
    'g')
      sel=$((0))
      ;;
    'G')
      sel=$(($total - 1))
      ;;
    's')
      shuffle_songs
      enter
    ;;
    *) ;;
  esac
done

