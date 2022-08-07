#include "doom/statusbar/lib.h"

#include "doom/error.h"
#include "doom/misc.h"

static void l_stlib_draw_num(
    StatusNumberWidget* widget, StatusBarRefresh refresh);

void stlib_update_num(
    DoomState* state, StatusNumberWidget* widget, StatusBarRefresh refresh) {
  if (*widget->should_update) {
    l_stlib_draw_num(widget, refresh);
  }
}

static void l_stlib_draw_num(
    StatusNumberWidget* widget, StatusBarRefresh refresh) {
  int num_digits = widget->width;
  int num = *widget->value;
  int w = widget->patches[0]->width;
  int h = widget->patches[0]->height;
  widget->old_value = *widget->value;
  bool neg = num < 0;
  if (neg) {
    if (num_digits == 2 && num < -9) {
      num = -9;
    } else if (num_digits == 3 && num < -99) {
      num = -99;
    }

    num = -num;
  }

  int x = widget->x - num_digits * w;
  if (widget->y - ST_Y < 0) {
    doom_error("stlib_draw_num: widget->y - ST_Y < 0");
  }

  TODO("finish this function");
  // video_copy_rect(x, widget->y - ST_Y, BG, w * num_digits, h, x, widget->y,
  // FG);
}
