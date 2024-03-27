#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

void panic_null_win(WINDOW *);
void init_ncurses(void);
void exit_ncurses(WINDOW *);

WINDOW *create_input_box(int, int, int, int);
void destroy_input_box(WINDOW *, WINDOW **);
char *handle_input(WINDOW *);
void apply_border(int, int, int, int, const char *, const char *);

static const char *master_toolbar_txt = "<q: quit> <f1: new message>";
static const char *input_toolbar_txt = "<esc|f1: quit> <enter: send>";

// some missing char codes from ncurses
#define KEY_ESC 27
#define KEY_DEL 127

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
  mvwprintw(w_master, 0, width - 7, " %s ", "feed");
  mvwprintw(w_master, height - 1, 1, " %s ", master_toolbar_txt);
  wrefresh(w_master);

  for (int i = 1; i < argc; ++i) {
    mvwprintw(w_master, i, 1, "- %s", argv[i]);
  }

  while (true) {
    int key = wgetch(w_master);
    switch (key) {
    case 'q': case KEY_ESC: { // 27 is esc ASCII code
      exit_ncurses(w_master);
    } break;
    case KEY_F(1): {
        WINDOW *w_input = create_input_box(height, width, start_y, start_x);
        handle_input(w_input);
        destroy_input_box(w_master, &w_input);
    } break;
    default:
      break;
    }
  }

  return 0; // technically unreachable
}

/*
  Not sure how smart this is. We refresh once padding is created, then
  IMMEDIATELY delete, so that refreshes of embedded windows do not
  alter the deleted padding. subwin is no good because then I still
  need a reference to it, as it must be deleted BEFORE the originator.
  Probably best to just use a product type here, i.e.:

  typedef struct {
    WINDOW *w_input;
    WINDOW *w_border;
  } INPUT_WIN;

  Then ammend destroy function as needed.
*/

void apply_border(int height, int width,
                  int start_y, int start_x,
                  const char *title,
                  const char *tool_txt)
{
  WINDOW *w_border = newwin(height, width, start_y, start_x);
  box(w_border, 0, 0);

  int txt_end = (int) strlen(tool_txt) + 3;
  mvwprintw(w_border, height - 1, width - txt_end, " %s ", tool_txt);
  mvwprintw(w_border, 0, 1, " %s ", title);

  wrefresh(w_border);
  delwin(w_border);
}

WINDOW *create_input_box(int height, int width, int start_y, int start_x)
{
    int input_height  = 6;
    int input_width   = 50;
    int input_start_y = start_y + (height - input_height) / 2;
    int input_start_x = start_x + (width - input_width) / 2;

    apply_border(input_height + 2, input_width + 2,
                 input_start_y - 1, input_start_x - 1,
                 "message", input_toolbar_txt);

    WINDOW *w_input   = newwin(input_height, input_width,
                               input_start_y, input_start_x);
    panic_null_win(w_input);

    keypad(w_input, TRUE);
    curs_set(TRUE);
    wrefresh(w_input);

    return w_input;
}

char *handle_input(WINDOW *w_input) {
  char input_buffer[280];
  size_t i = 0;
  memset(input_buffer, 0, sizeof(input_buffer)); // zero out

  wmove(w_input, 0, 0); // relative to window position! :)
  wclrtoeol(w_input);   // clear to end of line
  wrefresh(w_input);

  while (TRUE) {
    int key = wgetch(w_input);
    switch (key) {
    case KEY_F(1): case KEY_ESC: return NULL;
    case '\n': {
      char *input_str = (char *) malloc(i + 1);
      if (input_str == NULL) return NULL;
      memcpy(input_str, input_buffer, i + 1);
      return input_str;
    }
    case KEY_BACKSPACE: case KEY_DEL: {
      if (i > 0) input_buffer[--i] = '\0';
    } break;
    default:  {
      if (i < sizeof(input_buffer) - 1) input_buffer[i++] = (char) key;
    } break;
    }
    wmove(w_input, 0, 0); // Move cursor back to the start of the input line
    wclrtoeol(w_input); // Clear the line
    wprintw(w_input, "%s", input_buffer); // Print the current buffer content
    wrefresh(w_input);
  }
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
  set_escdelay(0);
}

void exit_ncurses(WINDOW *w) {
  delwin(w);
  endwin();
  exit(EXIT_SUCCESS);
}
