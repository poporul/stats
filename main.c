#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "utils.h"
#include "smc.h"

#define MENU_WIDTH 30
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

WINDOW *menu_canvas;
WINDOW *info_canvas;
WINDOW *menu_window;

static int max_columns, max_rows;

static int selected_item = 1;
static int highlighted_item = 1;

WINDOW *create_window(int height, int width, int y, int x);

enum EMenuColors {
  HIGHLIGHTED_MENU_C = 1,
  SELECTED_MENU_C = 2,
  SELECTED_AND_HIGHLIGHTED_MENU_C = 3
};

static const char *menu_items[] = {
  "Fan speed",
  "Cpu temp",
  "Battery info",
  "Exit"
};

static void draw_menu(WINDOW *window);
static void draw_borders(void);
static void process_menu_enter(void);

int main (int argv, char *argc[]) {
  open_smc();

  initscr();
  cbreak();
  noecho();

  curs_set(0);
  start_color();

  init_pair(HIGHLIGHTED_MENU_C, COLOR_WHITE, COLOR_CYAN);
  init_pair(SELECTED_MENU_C, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(SELECTED_AND_HIGHLIGHTED_MENU_C, COLOR_MAGENTA, COLOR_CYAN);

  getmaxyx(stdscr, max_rows, max_columns);

  menu_canvas = create_window(max_rows, MENU_WIDTH, 0, 0);
  info_canvas = create_window(max_rows, max_columns - MENU_WIDTH, 0, MENU_WIDTH);
  menu_window = create_window(ARRAY_LENGTH(menu_items), MENU_WIDTH - 2, 3, 1);

  keypad(menu_window, TRUE);

  draw_borders();
  draw_menu(menu_window);

  mvwaddstr(menu_canvas, 1, 8, "Select action");

  show_fan_info();

  wrefresh(menu_canvas);
  wrefresh(info_canvas);

  wrefresh(menu_window);

  while(1) {
    int key = wgetch(menu_window);

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

    draw_menu(menu_window);
    wrefresh(menu_window);
  }

  endwin();
  return EXIT_SUCCESS;
}

WINDOW *create_window(int height, int width, int y, int x) {
  WINDOW *window = newwin(height, width, y, x);
  return window;
}

static void draw_menu(WINDOW *window) {
  for(size_t i = 0; i < ARRAY_LENGTH(menu_items); i++) {
    if (highlighted_item == i + 1) {
      wattron(window, COLOR_PAIR(HIGHLIGHTED_MENU_C));
    }

    if (selected_item == i + 1) {
      wattron(window, A_BOLD | COLOR_PAIR(SELECTED_MENU_C));
    }

    if (selected_item == i + 1 && highlighted_item == i + 1) {
      wattron(window, COLOR_PAIR(SELECTED_AND_HIGHLIGHTED_MENU_C));
    }

    mvwaddch(window, i, 0, ' ');
    mvwaddstr(window, i, 1, menu_items[i]);

    int title_length = strlen(menu_items[i]);
    int width, height;
    getmaxyx(window, height, width);
    int span = width - title_length + 2;

    for(size_t i = 0; i < span - 3; i++) {
      waddch(window, ' ');
    }

    if (highlighted_item == i + 1) {
      wattroff(window, COLOR_PAIR(1));
    }

    if (selected_item == i + 1) {
      wattroff(window, A_BOLD | COLOR_PAIR(2));
    }
  }
}

static void draw_borders(void) {
  box(menu_canvas, 0, 0);
  wborder(info_canvas, ' ', 0, 0, 0, ACS_HLINE, 0, ACS_HLINE, 0);

  mvwhline(menu_canvas, 0, 1, ACS_HLINE, MENU_WIDTH - 2);
  mvwhline(menu_canvas, 2, 1, ACS_HLINE, MENU_WIDTH - 2);

  mvwhline(info_canvas, 0, 1, ACS_HLINE, max_columns - MENU_WIDTH - 2);
  mvwhline(info_canvas, 2, 0, ACS_HLINE, max_columns - MENU_WIDTH - 1);
}

void set_info_header(void) {
  int width, height;
  const char *selected_name = menu_items[selected_item - 1];

  getmaxyx(info_canvas, height, width);

  wmove(info_canvas, 1, 0);
  wclrtoeol(info_canvas);
  mvwaddch(info_canvas, 1, width - 1, ACS_VLINE);
  mvwprintw(info_canvas, 1, width / 2 - strlen(selected_name) / 2, selected_name);
  wrefresh(info_canvas);
}

static void process_menu_enter(void) {
  extern pthread_t thread;

  set_info_header();

  if (pthread_kill(thread, 0) == 0) {
    pthread_cancel(thread);
  }

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
      delwin(menu_window);
      delwin(menu_canvas);
      delwin(info_canvas);

      endwin();
      close_smc();
      exit(EXIT_SUCCESS);
      break;
  }
}
