#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "smc.h"

#define MENU_WIDTH 30
#define MARGIN_TOP 3
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

WINDOW *menu;
WINDOW *info;

static int max_columns, max_rows;

static int selected_item = 1;
static int highlighted_item = 1;

static WINDOW *create_window(int height, int width, int y, int x);
static const char *menu_items[] = {
  "Fan speed",
  "Cpu temp",
  "Battery info",
  "Exit"
};

static void draw_menu(WINDOW *menu, int margin_top);
static void draw_borders(void);
static void process_menu_enter(void);

int main (int argv, char *argc[]) {
  open_smc();

  initscr();
  cbreak();
  noecho();

  curs_set(0);
  start_color();

  init_pair(1, COLOR_WHITE, COLOR_CYAN);    // highlighted
  init_pair(2, COLOR_MAGENTA, COLOR_BLACK); // selected
  init_pair(3, COLOR_MAGENTA, COLOR_CYAN);  // selected && highlighted

  getmaxyx(stdscr, max_rows, max_columns);

  menu = create_window(max_rows, MENU_WIDTH, 0, 0);
  info = create_window(max_rows, max_columns - MENU_WIDTH, 0, MENU_WIDTH);

  keypad(menu, TRUE);

  draw_borders();
  draw_menu(menu, MARGIN_TOP);

  mvwaddstr(menu, 1, 8, "Select action");

  wrefresh(menu);
  wrefresh(info);

  while(1) {
    int key = wgetch(menu);

    switch(key) {
      case KEY_UP:
        if (highlighted_item > 1)
          highlighted_item--;
        break;

      case KEY_DOWN:
        if (highlighted_item < ARRAY_LENGTH(menu_items))
          highlighted_item++;
        break;

      case 10:
        selected_item = highlighted_item;
        process_menu_enter();
        break;
    }

    draw_menu(menu, MARGIN_TOP);
    wrefresh(menu);
  }

  endwin();
  return EXIT_SUCCESS;
}

static WINDOW *create_window(int height, int width, int y, int x) {
  WINDOW *window = newwin(height, width, y, x);
  return window;
}

static void draw_menu(WINDOW *menu, int margin_top) {
  for(size_t i = 0; i < ARRAY_LENGTH(menu_items); i++) {
    if (highlighted_item == i + 1) {
      wattron(menu, COLOR_PAIR(1));
    }

    if (selected_item == i + 1) {
      wattron(menu, A_BOLD | COLOR_PAIR(2));
    }

    if (selected_item == i + 1 && highlighted_item == i + 1) {
      wattron(menu, COLOR_PAIR(3));
    }

    mvwaddch(menu, margin_top + i, 1, ' ');
    mvwaddstr(menu, margin_top + i, 2, menu_items[i]);

    int title_length = strlen(menu_items[i]);
    int span = MENU_WIDTH - title_length;

    for(size_t i = 0; i < span - 3; i++) {
      waddch(menu, ' ');
    }

    if (highlighted_item == i + 1) {
      wattroff(menu, COLOR_PAIR(1));
    }

    if (selected_item == i + 1) {
      wattroff(menu, A_BOLD | COLOR_PAIR(2));
    }
  }
}

static void draw_borders(void) {
  box(menu, 0, 0);
  wborder(info, ' ', 0, 0, 0, ACS_HLINE, 0, ACS_HLINE, 0);

  mvwhline(menu, 0, 1, ACS_HLINE, MENU_WIDTH - 2);
  mvwhline(menu, 2, 1, ACS_HLINE, MENU_WIDTH - 2);

  mvwhline(info, 0, 1, ACS_HLINE, max_columns - MENU_WIDTH - 2);
  mvwhline(info, 2, 0, ACS_HLINE, max_columns - MENU_WIDTH - 1);
}

static void process_menu_enter(void) {
  wmove(info, 1, 0);
  wclrtoeol(info);
  mvwprintw(info, 1, (max_columns - MENU_WIDTH) / 2 - strlen(menu_items[selected_item - 1]) / 2, menu_items[selected_item - 1]);
  wrefresh(info);

  switch(selected_item) {
    case 1:
      show_fan_info();
      break;

    case 2:
      show_cpu_info();
      break;

    case 3:
      show_battery_info();
      break;

    case 4:
      delwin(menu);
      delwin(info);

      endwin();
      close_smc();
      exit(EXIT_SUCCESS);
      break;
  }
}
