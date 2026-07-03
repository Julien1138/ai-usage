#include "ui.h"

#include "gauges.h"
#include "mascot.h"
#include "theme.h"

#define TEXT_H 18
#define BAR_H 8
#define BAR_RADIUS 4
#define RESET_H 14
#define ROW_H (TEXT_H + BAR_H + RESET_H)
#define ROW_GAP 8
#define SIDE_MARGIN 12
#define ROUND_INSET 6
#define SCROLL_STEP 42

#define TITLE_TEXT "Claude usage"
#define TITLE_H 24
#define TITLE_GAP 6
#define MASCOT_GAP 6

static Layer *s_canvas;
static int s_scroll_offset = 0;

#if defined(PBL_ROUND)
static uint32_t isqrt(uint32_t n) {
  uint32_t x = n, c = 0, d = 1u << 30;
  while (d > n) {
    d >>= 2;
  }
  while (d != 0) {
    if (x >= c + d) {
      x -= c + d;
      c = (c >> 1) + d;
    } else {
      c >>= 1;
    }
    d >>= 2;
  }
  return c;
}
#endif

// Zone disponible pour une rangée occupant [y_top, y_bottom] à l'écran.
// Sur écran rond, c'est la corde du cercle au niveau le plus contraint de la
// rangée : chaque rangée a donc sa propre largeur. Renvoie la largeur et
// écrit le bord gauche dans out_x.
static int16_t row_span(GRect bounds, int16_t y_top, int16_t y_bottom,
                        int16_t inset, int16_t *out_x) {
#if defined(PBL_ROUND)
  int16_t cy = bounds.size.h / 2;
  int16_t radius = bounds.size.w / 2 - inset;
  int16_t dy_top = y_top < cy ? cy - y_top : y_top - cy;
  int16_t dy_bottom = y_bottom < cy ? cy - y_bottom : y_bottom - cy;
  int16_t dy = dy_top > dy_bottom ? dy_top : dy_bottom;
  if (dy >= radius) {
    *out_x = bounds.size.w / 2;
    return 0;
  }
  int32_t half = isqrt((uint32_t)((int32_t)radius * radius - (int32_t)dy * dy));
  int16_t width = 2 * half - 2 * inset;
  if (width < 0) {
    width = 0;
  }
  *out_x = (bounds.size.w - width) / 2;
  return width;
#else
  (void)inset;
  *out_x = SIDE_MARGIN;
  return bounds.size.w - 2 * SIDE_MARGIN;
#endif
}

static int content_height(void) {
  if (gauges_count() == 0) {
    return 0;
  }
  return TITLE_H + TITLE_GAP + gauges_count() * ROW_H +
         (gauges_count() - 1) * ROW_GAP;
}

static int top_start(GRect bounds) {
  int content_h = content_height();
  int min_top = PBL_IF_ROUND_ELSE(24, 6);
  if (content_h < bounds.size.h - 2 * min_top) {
    return (bounds.size.h - content_h) / 2;
  }
  return min_top;
}

static void clamp_scroll(GRect bounds) {
  int top = top_start(bounds);
  int min_offset = bounds.size.h - 2 * top - content_height();
  if (min_offset > 0) {
    min_offset = 0;
  }
  if (s_scroll_offset < min_offset) {
    s_scroll_offset = min_offset;
  }
  if (s_scroll_offset > 0) {
    s_scroll_offset = 0;
  }
}

// « Réinit. dans 2j 5h » / « 2h05 » / « 12 min » à partir de l'epoch.
static void format_reset(time_t reset, char *buf, size_t len) {
  buf[0] = '\0';
  if (reset <= 0) {
    return;
  }
  int32_t delta = reset - time(NULL);
  if (delta <= 60) {
    snprintf(buf, len, "Réinit. imminente");
    return;
  }
  int days = delta / 86400;
  int hours = (delta % 86400) / 3600;
  int mins = (delta % 3600) / 60;
  if (days > 0) {
    snprintf(buf, len, "Réinit. dans %dj %dh", days, hours);
  } else if (hours > 0) {
    snprintf(buf, len, "Réinit. dans %dh%02d", hours, mins);
  } else {
    snprintf(buf, len, "Réinit. dans %d min", mins);
  }
}

// Dessine « mascotte + Claude usage » alignés à gauche ; sur écran rond, le
// titre s'appuie sur une corde presque pleine largeur (marge réduite) et la
// police diminue si elle est encore trop étroite.
static void draw_title(GContext *ctx, GRect bounds, int16_t y) {
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  int16_t text_center_offset = 11;
  GRect measure_rect = GRect(0, 0, bounds.size.w, TITLE_H);
  GSize text_size = graphics_text_layout_get_content_size(
      TITLE_TEXT, font, measure_rect, GTextOverflowModeTrailingEllipsis,
      GTextAlignmentLeft);

  int16_t x0;
  int16_t avail = row_span(bounds, y, y + TITLE_H, 2, &x0);
  if (text_size.w + MASCOT_W + MASCOT_GAP > avail) {
    font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
    text_center_offset = 9;
    text_size = graphics_text_layout_get_content_size(
        TITLE_TEXT, font, measure_rect, GTextOverflowModeTrailingEllipsis,
        GTextAlignmentLeft);
  }

  mascot_draw(ctx, GPoint(x0, y + text_center_offset - MASCOT_H / 2));

  graphics_context_set_text_color(ctx, COLOR_TEXT);
  graphics_draw_text(ctx, TITLE_TEXT, font,
                     GRect(x0 + MASCOT_W + MASCOT_GAP, y,
                           avail - MASCOT_W - MASCOT_GAP, TITLE_H),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

static void draw_status(GContext *ctx, GRect bounds) {
  graphics_context_set_text_color(ctx, COLOR_TEXT);
  GRect text_rect = grect_inset(bounds, GEdgeInsets(0, PBL_IF_ROUND_ELSE(26, 14)));
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  GSize size = graphics_text_layout_get_content_size(
      gauges_status(), font, text_rect, GTextOverflowModeWordWrap,
      GTextAlignmentCenter);
  text_rect.origin.y = (bounds.size.h - size.h) / 2 - 4;
  graphics_draw_text(ctx, gauges_status(), font, text_rect,
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

static void draw_gauge_row(GContext *ctx, int index, int16_t x, int16_t y,
                           int16_t width) {
  GFont label_font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);

  graphics_context_set_text_color(ctx, COLOR_TEXT);
  graphics_draw_text(ctx, gauges_label(index), label_font,
                     GRect(x, y - 3, width - 36, TEXT_H),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);

  char pct_buf[8];
  snprintf(pct_buf, sizeof(pct_buf), "%d%%", gauges_percent(index));
  graphics_draw_text(ctx, pct_buf, label_font, GRect(x, y - 3, width, TEXT_H),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);

  GRect track = GRect(x, y + TEXT_H, width, BAR_H);
  graphics_context_set_fill_color(ctx, COLOR_TRACK);
  graphics_fill_rect(ctx, track, BAR_RADIUS, GCornersAll);
#if !defined(PBL_COLOR)
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_round_rect(ctx, track, BAR_RADIUS);
#endif

  int pct = gauges_percent(index);
  if (pct < 0) {
    pct = 0;
  }
  if (pct > 100) {
    pct = 100;
  }
  int16_t fill_w = width * pct / 100;
  if (pct > 0 && fill_w < BAR_H) {
    fill_w = BAR_H;
  }
  if (fill_w > 0) {
    graphics_context_set_fill_color(ctx, COLOR_ACCENT);
    graphics_fill_rect(ctx, GRect(x, y + TEXT_H, fill_w, BAR_H), BAR_RADIUS,
                       GCornersAll);
  }

  char reset_buf[32];
  format_reset(gauges_reset(index), reset_buf, sizeof(reset_buf));
  if (reset_buf[0] != '\0') {
    graphics_context_set_text_color(ctx, COLOR_RESET_TEXT);
    graphics_draw_text(ctx, reset_buf,
                       fonts_get_system_font(FONT_KEY_GOTHIC_14),
                       GRect(x, y + TEXT_H + BAR_H - 1, width, RESET_H + 4),
                       GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
  }
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  if (gauges_count() == 0) {
    draw_status(ctx, bounds);
    return;
  }

  int16_t y = top_start(bounds) + s_scroll_offset;

  draw_title(ctx, bounds, y);
  y += TITLE_H + TITLE_GAP;

  for (int i = 0; i < gauges_count(); i++) {
    int16_t x;
    int16_t width = row_span(bounds, y, y + ROW_H, ROUND_INSET, &x);
    if (width >= 44) {
      draw_gauge_row(ctx, i, x, y, width);
    }
    y += ROW_H + ROW_GAP;
  }
}

void ui_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  s_canvas = layer_create(layer_get_bounds(window_layer));
  layer_set_update_proc(s_canvas, canvas_update_proc);
  layer_add_child(window_layer, s_canvas);
}

void ui_window_unload(Window *window) {
  layer_destroy(s_canvas);
  s_canvas = NULL;
}

void ui_mark_dirty(void) {
  if (s_canvas) {
    layer_mark_dirty(s_canvas);
  }
}

void ui_reset_scroll(void) {
  s_scroll_offset = 0;
}

void ui_scroll(int direction) {
  if (!s_canvas) {
    return;
  }
  s_scroll_offset += direction * SCROLL_STEP;
  clamp_scroll(layer_get_bounds(s_canvas));
  layer_mark_dirty(s_canvas);
}
