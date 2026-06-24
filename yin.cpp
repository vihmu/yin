/*
 yin
   terminal music player based on mpv
   https://github.com/vihmu/yin

 Copyright (c) 2026 vihmu

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 “Software”), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <unistd.h>
#include <signal.h>
#include <termios.h>

#define VERSION "0.1.1"
#define MPV     "mpv "
// Keys
#define K_QUIT  'q'
#define K_UP    'k'
#define K_DOWN  'j'
#define K_LEFT  'h'
#define K_RIGHT 'l'
#define K_LOOP  'L'
#define K_TOP   'g'
#define K_BOT   'G'
#define K_SHUF  's'
// Colors
#define ESC "\x1b["
#define RES "\x1b[m"
#define RED "31m"
#define BLU "34m"
#define REV "7m"

using namespace std;

vector<string> d;
vector<string> f;
vector<string> dir;
string info = "welcome";
bool quit = false;
bool shuf = true;
size_t sel = 0;

/*
 * Terminal
 */
char getch(void)
{
  // Get one character from stdin
  char buf = 0;
  struct termios default_term;
  if (tcgetattr(0, &default_term) < 0)
    perror("Failed tcgetattr");

  // Disable canonical mode and echoing
  default_term.c_lflag &= ~ICANON;
  default_term.c_lflag &= ~ECHO;
  default_term.c_cc[VMIN] = 1;
  default_term.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &default_term) < 0)
    perror("Failed canonical");
  // Read from stdin (0)
  if (read(0, &buf, 1) < 0)
    perror("Failed to read from stdin");
  // Restore terminal properties
  default_term.c_lflag |= ICANON;
  default_term.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &default_term) < 0)
    perror("Failed restore");
  return buf;
}

void move(int x, int y)
{
  // Move cursor to x/y coords.
  cout << ESC << y << ';' << x << 'H';
}

void clear(void)
{
  // Clear the screen
  cout << ESC << "H" << ESC << "2J";
}

void cursor(bool t)
{
  // Hide or show cursor
  if (t)
    cout << ESC << "?25h";
  else
    cout << ESC << "?25l";
}

void draw_bar(void)
{
  // Display the information bar
  move(1,1);
  cout << ESC << 'J' << ESC << REV << sel << " " << info << RES << endl;
}

/*
 * File IO
 */
void get_files(void)
{
  // Make sure all vectors are cleared before running
  d.clear();
  f.clear();
  dir.clear();
  // Store files and directories in separate arrays
  for (const auto &entry : filesystem::directory_iterator(".")) {
    if (filesystem::is_directory(entry.path()))
      d.push_back(entry.path());
    else
      f.push_back(entry.path());
  }
  // Combine the two arrays so that directories come first
  for (size_t i = 0; i < size(d); ++i) {
    dir.push_back(d[i]);
  }
  for (size_t i = 0; i < size(f); ++i) {
    dir.push_back(f[i]);
  }
}

// TODO: See play()
//void exec(const string cmd)
//{
//  FILE* cpipe = popen(cmd.c_str(), "r");
//  if (cpipe == NULL) {
//    perror("Failed exec");
//  }
//
//  char buffer[256];
//  string output;
//  while (fgets(buffer, sizeof(buffer), cpipe) != NULL) {
//    output += buffer;
//  }
//}

/*
 * Other
 */
void play(const char *song, bool loop)
{
  char cmd[256];
  // TODO: Only run if file extension matches common audio formats?
  // TODO: system() seems to override the sig handler
  //       sleeping for 1 sec as a fix for now
  cout << "\n\n\t" << ESC << RED << song << RES << "\n" << endl;
  if (loop)
    snprintf(cmd, sizeof(cmd), "%s --no-video --loop-file=inf \"%s\"", MPV, song);
  else
    snprintf(cmd, sizeof(cmd), "%s --no-video \"%s\"", MPV, song);
  system(cmd);
}

void shuffle_quit(int sig)
{
  // Ignore signal (is always SIGINT)
  (void)sig;
  shuf = false;
}

void shuffle(void)
{
  // Loop and play random songs in folder
  // Handle Ctrl+C (SIGINT)
  struct sigaction handler;

  handler.sa_handler = shuffle_quit;
  sigemptyset(&handler.sa_mask);
  handler.sa_flags = 0;
  
  sigaction(SIGINT, &handler, NULL);

  while (shuf) {
    const char *song = dir[ rand()%size(dir)+size(d) ].c_str();
    play(song, false);
    sleep(1);
  }
}


int main(int argc, char **argv)
{
  // Handle terminal args
  if (argc > 1) {
    if (string(argv[1]) == "-h") {
      cout << "yin (" << VERSION << ") - tiny terminal music player based on mpv" << endl;
      cout << "usage: yin [directory]" << endl;
      exit(2);
    }
    error_code ec;
    filesystem::current_path(argv[1], ec);
    // Ignore given path if not available
    if (ec) {
      filesystem::current_path(".");
      info = "Unknown path.";
    }
  }
  // Initialize files array
  get_files();

  // Prepare screen (clear and hide cursor)
  clear();
  cursor(false);

  while (!quit) {
    draw_bar();
    // Display files
    for (size_t i =  0; i < size(dir); ++i) {
      if (i == sel)
        cout << ESC << RED << dir[i] << RES << endl;
      else if (i < size(d))
        cout << ESC << BLU << dir[i] << RES << endl;
      else
        cout << dir[i] << endl;
    }

    char key = getch();
    switch (key) {
    case K_QUIT:
      quit = true;
      break;
    case K_UP:
      sel = (sel > 0) ? sel - 1 : sel;
      break;
    case K_DOWN:
      sel = (sel < size(dir) - 1) ? sel + 1 : sel;
      break;
    case K_LEFT:
      filesystem::current_path("..");
      clear();
      get_files();
      sel = size(dir) / 2;
      break;
    case K_RIGHT:
    case K_LOOP:
      if (sel < size(d)) {
        filesystem::current_path(d[sel]);
        clear();
        get_files();
        sel = 0;
      } else {
        info = dir[sel];
        const char *song = dir[sel].c_str();
        // Looping
        if (key == K_LOOP)
          play(song, true);
        else
          play(song, false);
        // Hide cursor again and clear the screen
        cursor(false);
        clear();
      }
      break;
    case K_SHUF:
      shuffle();
      shuf = true;
      cursor(false);
      break;
    case K_TOP:
      sel = 0;
      break;
    case K_BOT:
      sel = size(dir) - 1;
      break;
    default:
      break;
    }
  }

  // Restore cursor
  cursor(true);
}
