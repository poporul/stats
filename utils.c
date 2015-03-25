#include <ncurses.h>
#include <stdlib.h>
#include <pthread.h>

#include "utils.h"
#include "smc.h"

WINDOW *create_window(int height, int width, int y, int x);
extern WINDOW *info_canvas;
pthread_t thread;

void *update_cpu_info(void) {
  void *result;
  // TODO
  return result;
}

void *update_battery_info(void) {
  void *result;
  // TODO
  return result;
}

void *update_fan_info(void) {
  fan_rpm_t lfan_rpm_result;
  fan_rpm_t rfan_rpm_result;

  while (1) {
    lfan_rpm_result = get_fan_rpm(LEFT_FAN);
    rfan_rpm_result = get_fan_rpm(RIGHT_FAN);

    redraw_rpm_info(lfan_rpm_result, rfan_rpm_result);

    sleep(1);
  }
  return NULL;
}

void redraw_rpm_info(fan_rpm_t lfan_result, fan_rpm_t rfan_result) {
  mvwprintw(info_canvas, 4, 16, "%u", lfan_result.actualRpm);
  mvwprintw(info_canvas, 4, 26, "%u", rfan_result.actualRpm);

  mvwprintw(info_canvas, 5, 16, "%u", lfan_result.minimumRpm);
  mvwprintw(info_canvas, 5, 26, "%u", rfan_result.minimumRpm);

  mvwprintw(info_canvas, 6, 16, "%u", lfan_result.maximumRpm);
  mvwprintw(info_canvas, 6, 26, "%u", rfan_result.maximumRpm);

  mvwprintw(info_canvas, 7, 16, "%u", lfan_result.safeRpm);
  mvwprintw(info_canvas, 7, 26, "%u", rfan_result.safeRpm);

  /*wrefresh(info_canvas);*/
}

void show_fan_info(void) {
  static WINDOW *fan_info_window = NULL;

  if (fan_info_window == NULL) {
    int startx, starty, width, height;
    getbegyx(info_canvas, starty, startx);
    getmaxyx(info_canvas, height, width);
    fan_info_window = create_window(height - 4, width - 2, 3, startx);

    mvwprintw(fan_info_window, 3, 16, "Left fan");
    mvwprintw(fan_info_window, 3, 26, "Right fan");

    mvwprintw(fan_info_window, 4, 1, "Actual RPM");
    mvwprintw(fan_info_window, 5, 1, "Minimum RPM");
    mvwprintw(fan_info_window, 6, 1, "Maximum RPM");
    mvwprintw(fan_info_window, 7, 1, "Safe RPM");

    /*box(fan_info_window, 0, 0);*/
  }

  wrefresh(info_canvas);
  wrefresh(fan_info_window);

  if (pthread_create(&thread, NULL, (void *(*)(void *)) update_fan_info, NULL) != 0) {
    perror("Thread create error");
  }
}

void show_cpu_info(void) {
  if (pthread_create(&thread, NULL, (void *(*)(void *)) update_cpu_info, NULL) != 0) {
    perror("Thread create error");
  }
}

void show_battery_info(void) {
  if (pthread_create(&thread, NULL, (void *(*)(void *)) update_battery_info, NULL) != 0) {
    perror("Thread create error");
  }
}
