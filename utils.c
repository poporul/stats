#include <ncurses.h>
#include <stdlib.h>
#include <pthread.h>

#include "utils.h"
#include "smc.h"

extern WINDOW *menu;
extern WINDOW *info;

void *update_rpm_info() {
  fan_rpm_t lfan_rpm_result;
  fan_rpm_t rfan_rpm_result;

  while (1) {
    lfan_rpm_result = get_fan_rpm(LEFT_FAN);
    rfan_rpm_result = get_fan_rpm(RIGHT_FAN);

    mvwprintw(info, 4, 16, "%u", lfan_rpm_result.actualRpm);
    mvwprintw(info, 4, 26, "%u", rfan_rpm_result.actualRpm);

    mvwprintw(info, 5, 16, "%u", lfan_rpm_result.minimumRpm);
    mvwprintw(info, 5, 26, "%u", rfan_rpm_result.minimumRpm);

    mvwprintw(info, 6, 16, "%u", lfan_rpm_result.maximumRpm);
    mvwprintw(info, 6, 26, "%u", rfan_rpm_result.maximumRpm);

    mvwprintw(info, 7, 16, "%u", lfan_rpm_result.safeRpm);
    mvwprintw(info, 7, 26, "%u", rfan_rpm_result.safeRpm);

    wrefresh(info);
    sleep(1);
  }
  return NULL;
}

void show_fan_info(void) {
  mvwprintw(info, 3, 16, "Left fan");
  mvwprintw(info, 3, 26, "Right fan");

  mvwprintw(info, 4, 1, "Actual RPM");
  mvwprintw(info, 5, 1, "Minimum RPM");
  mvwprintw(info, 6, 1, "Maximum RPM");
  mvwprintw(info, 7, 1, "Safe RPM");
  

  pthread_t thread;
  int thread_create_result;

  thread_create_result = pthread_create(&thread, NULL, update_rpm_info, NULL);
  if (thread_create_result != 0) {
    perror("Error!!!");
  }
}

void show_cpu_info(void) {
}

void show_battery_info(void) {
}
