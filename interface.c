#include <ncurses.h>
#include <stdlib.h>

void panic_null_win(WINDOW *);
void init_ncurses(void);
void exit_ncurses(WINDOW *);

WINDOW *create_input_box(int, int, int, int);
void destroy_input_box(WINDOW *, WINDOW **);
void handle_input(WINDOW *);

static const char *toolbar_txt = " <q: quit> <f1: input text> ";

int main(int argc, char *argv[]) {
  init_ncurses();

  int height  = getmaxy(stdscr) - 2;
  int width   = getmaxx(stdscr) - 2;
  int start_y = (LINES - height) / 2;
  int start_x = (COLS - width) / 2;

  WINDOW *w_master = newwin(height, width, start_y, start_x);
  panic_null_win(w_master);
  keypad(w_master, TRUE);

  box(w_master, 0, 0);
  mvwprintw(w_master, height - 1, 1, "%s", toolbar_txt);
  wrefresh(w_master);

  for (int i = 1; i < argc; ++i) {
    mvwprintw(w_master, i, 1, "%s", argv[i]);
  }

  int key;
  while ((key = wgetch(w_master)) != 'q') {
    if (key == KEY_F(1)) {
      WINDOW *w_input = create_input_box(height, width, start_y, start_x);
      handle_input(w_input);
      destroy_input_box(w_master, &w_input);
    }
  }

  // generalize to data structure which holds multiple windows
  exit_ncurses(w_master); 

  return 0; // technically unreachable
}

WINDOW *create_input_box(int height, int width, int start_y, int start_x)
{
    int input_height  = 8;
    int input_width   = 35;
    int input_start_y = start_y + (height - input_height) / 2;
    int input_start_x = start_x + (width - input_width) / 2;

    WINDOW *w_input = newwin(input_height, input_width,
                             input_start_y, input_start_x);
    panic_null_win(w_input);

    box(w_input, 0, 0);
    keypad(w_input, TRUE);
    curs_set(TRUE);
    wrefresh(w_input);

    return w_input;
}

void handle_input(WINDOW *w_input) {
  // Input handling loop
  char input_buffer[100]; // Adjust the buffer size as needed
  int i = 0; int key;
  while ((key = wgetch(w_input)) != KEY_F(1)) {
    if (key == '\n') break;  // Enter key
    input_buffer[i++] = (char) key;
    wprintw(w_input, "%s", input_buffer); // Display the input in real-time
    wrefresh(w_input);
  }
  input_buffer[i] = '\0'; // Null terminate the string 
}

// Double pointer to w_input necessary to prevent pointer from being passed
// by value. If this were not used, then setting w_input = NULL would never
// actually alter the value of w_input.
void destroy_input_box(WINDOW *w_master, WINDOW **w_input) {
  delwin(*w_input);
  *w_input = NULL; 
  touchwin(w_master);
  curs_set(FALSE);
  wrefresh(w_master);
}

void panic_null_win(WINDOW *w) {
  if (w == NULL) {
    endwin();
    fprintf(stderr, "PANIC: <panic_null_win()> NULL window returned.\n");
    exit(EXIT_FAILURE);
  }
}

void init_ncurses(void) {
  initscr();
  noecho();
  cbreak();
  curs_set(FALSE);
}

void exit_ncurses(WINDOW *w) {
  delwin(w);
  endwin();
  exit(EXIT_SUCCESS);
}
