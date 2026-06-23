# yin (音)

Small terminal music player written in C++17.
It uses [mpv](https://mpv.io/) for playback.

Work in progress! 

## keybinds

- j/k: cycle up/down
- l: play selected song / open selected directory
- L: loop selected song
- h: go up a directory
- g: go to the first item
- G: to to the last item
- s: start playing shuffled songs in the current directory

## rationale

Playlists are stupid, just use the filesystem as your database.

Most music players (terminal ones included) expect the user to use some sort of
playlist format or manually add files to their own playlist-like implementation. 

'yin' is different, it only acts as a tiny file picker for mpv.
