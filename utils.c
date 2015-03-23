#include <ncurses.h>
#include <stdlib.h>
#include <pthread.h>

#include "utils.h"
#include "smc.h"

extern WINDOW *info_canvas;

void *update_rpm_info(WINDOW *window) {
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

  wrefresh(info_canvas);
}

void show_fan_info(void) {
  mvwprintw(info_canvas, 3, 16, "Left fan");
  mvwprintw(info_canvas, 3, 26, "Right fan");

  mvwprintw(info_canvas, 4, 1, "Actual RPM");
  mvwprintw(info_canvas, 5, 1, "Minimum RPM");
  mvwprintw(info_canvas, 6, 1, "Maximum RPM");
  mvwprintw(info_canvas, 7, 1, "Safe RPM");

  pthread_t thread;
  int thread_create_result;

  thread_create_result = pthread_create(&thread, NULL, (void *(*)(void *)) update_rpm_info, NULL);
  if (thread_create_result != 0) {
    perror("Error!!!");
  }
}

void show_cpu_info(void) {
  // TODO
}

void show_battery_info(void) {
  // TODO
}
