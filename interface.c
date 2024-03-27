#include <ncurses.h>
#include <stdlib.h>

void panic_null_win(WINDOW *);
void init_ncurses(void);
void exit_ncurses(WINDOW *);

WINDOW *create_input_box(int, int, int, int);
void destroy_input_box(WINDOW *, WINDOW **);

static const char *toolbar_txt = " <q: quit> <f1: input text> ";

int main(int argc, char *argv[]) {
  init_ncurses();

  int height  = getmaxy(stdscr) - 2;
  int width   = getmaxx(stdscr) - 2;
  int start_y = (LINES - height) / 2;
  int start_x = (COLS - width) / 2;

  WINDOW *w_master = newwin(height, width, start_y, start_x);
  panic_null_win(w_master);

  box(w_master, 0, 0);
  mvwprintw(w_master, height - 1, 1, "%s", toolbar_txt);
  wrefresh(w_master);

  for (int i = 1; i < argc; ++i) {
    mvwprintw(w_master, i, 1, "%s", argv[i]);
  }

  int key;
  WINDOW *w_input = NULL;
  const int KEY_F1 = 27; // TODO: find cross-platform way
  while ((key = wgetch(w_master)) != 'q') {
    if (key == KEY_F1 && w_input == NULL) {
      w_input = create_input_box(height, width, start_y, start_x);
    } else if (key == KEY_F1 && w_input != NULL) {
      destroy_input_box(w_master, &w_input);
    }
  }

  // generalize to data structure which holds multiple windows
  exit_ncurses(w_master); 

  return 0; // technically unreachable
}

WINDOW *create_input_box(int height, int width, int start_y, int start_x)
{
    int input_height  = 3;
    int input_width   = 30;
    int input_start_y = start_y + height / 2 - input_height / 2;
    int input_start_x = start_x + width / 2 - input_width / 2;

    WINDOW *w_input = newwin(input_height, input_width,
                             input_start_y, input_start_x);
    panic_null_win(w_input);

    box(w_input, 0, 0);
    curs_set(TRUE);
    wrefresh(w_input);
    return w_input;
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
