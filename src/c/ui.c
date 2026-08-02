#include "ui.h"

#include "gauges.h"
#include "i18n.h"
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

#define TITLE_H 24
#define TITLE_GAP 6
#define MASCOT_GAP 6

// Heure (haut) et « mis à jour il y a » (bas) : même hauteur de ligne et
// même marge par rapport au bord de l'écran, pour rester symétriques.
#define TIME_H 18
#define TIME_Y PBL_IF_ROUND_ELSE(8, 2)

static Layer *s_canvas;
static int s_scroll_offset = 0;
static TimeUnits s_tick_units = MINUTE_UNIT;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  ui_mark_dirty();
}

// Cadence minimale pour garder « mis à jour il y a » et les comptes à
// rebours de réinitialisation à jour, sans réveiller la montre plus que
// nécessaire : secondes tant que le message affiche des secondes, minutes
// tant qu'une jauge est visible (son compte à rebours descend en minutes)
// ou que le message affiche encore des minutes, heures au-delà (le message
// n'affiche alors plus que jours + heures).
static TimeUnits desired_tick_units(void) {
  time_t last = gauges_last_update();
  int32_t since_update = last > 0 ? time(NULL) - last : -1;
  if (since_update >= 0 && since_update < 60) {
    return SECOND_UNIT;
  }
  if (gauges_count() > 0) {
    return MINUTE_UNIT;
  }
  if (since_update >= 86400) {
    return HOUR_UNIT;
  }
  return MINUTE_UNIT;
}

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

// Bande réservée en haut/bas de l'écran pour l'heure et le texte de mise à
// jour ; symétrique pour que le contenu (titre + jauges) reste centrable
// avec la même formule qu'avant leur ajout.
static int chrome_margin(void) {
  return PBL_IF_ROUND_ELSE(24, 6) + TIME_H;
}

static int top_start(GRect bounds) {
  int content_h = content_height();
  int min_top = chrome_margin();
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
    snprintf(buf, len, "%s", i18n_get(STR_RESET_IMMINENT));
    return;
  }
  int days = delta / 86400;
  int hours = (delta % 86400) / 3600;
  int mins = (delta % 3600) / 60;
  if (days > 0) {
    snprintf(buf, len, i18n_get(STR_RESET_IN_D_H), days, hours);
  } else if (hours > 0) {
    snprintf(buf, len, i18n_get(STR_RESET_IN_H_M), hours, mins);
  } else {
    snprintf(buf, len, i18n_get(STR_RESET_IN_MIN), mins);
  }
}

// Dessine « mascotte + Claude usage » alignés à gauche ; sur écran rond, le
// titre s'appuie sur une corde presque pleine largeur (marge réduite) et la
// police diminue si elle est encore trop étroite.
static void draw_title(GContext *ctx, GRect bounds, int16_t y) {
  const char *title = i18n_get(STR_TITLE);
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  int16_t text_center_offset = 11;
  GRect measure_rect = GRect(0, 0, bounds.size.w, TITLE_H);
  GSize text_size = graphics_text_layout_get_content_size(
      title, font, measure_rect, GTextOverflowModeTrailingEllipsis,
      GTextAlignmentLeft);

  int16_t x0;
  int16_t avail = row_span(bounds, y, y + TITLE_H, 2, &x0);
  if (text_size.w + MASCOT_W + MASCOT_GAP > avail) {
    font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
    text_center_offset = 9;
    text_size = graphics_text_layout_get_content_size(
        title, font, measure_rect, GTextOverflowModeTrailingEllipsis,
        GTextAlignmentLeft);
  }

  mascot_draw(ctx, GPoint(x0, y + text_center_offset - MASCOT_H / 2));

  graphics_context_set_text_color(ctx, COLOR_TEXT);
  graphics_draw_text(ctx, title, font,
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
  int margin = chrome_margin();
  text_rect.origin.y = margin + (bounds.size.h - 2 * margin - size.h) / 2;
  graphics_draw_text(ctx, gauges_status(), font, text_rect,
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

// Heure courante centrée en haut de l'écran, taille et graisse identiques
// aux autres textes secondaires (compte à rebours de réinitialisation).
static void draw_time(GContext *ctx, GRect bounds) {
  time_t now = time(NULL);
  struct tm *local = localtime(&now);
  static char buf[8];
  strftime(buf, sizeof(buf), clock_is_24h_style() ? "%H:%M" : "%I:%M", local);

  int16_t x0;
  int16_t avail = row_span(bounds, TIME_Y, TIME_Y + TIME_H, 2, &x0);
  graphics_context_set_text_color(ctx, COLOR_TEXT);
  graphics_draw_text(ctx, buf, fonts_get_system_font(FONT_KEY_GOTHIC_14),
                     GRect(x0, TIME_Y, avail, TIME_H),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

// « mis à jour il y a Xj Yh » / « il y a Xh05 » / « il y a X min » / « il y a
// X s » à partir de l'epoch de la dernière mise à jour reçue du téléphone.
static void format_last_update(time_t last_update, char *buf, size_t len) {
  buf[0] = '\0';
  if (last_update <= 0) {
    return;
  }
  int32_t delta = time(NULL) - last_update;
  if (delta < 0) {
    delta = 0;
  }
  int days = delta / 86400;
  int hours = (delta % 86400) / 3600;
  int mins = (delta % 3600) / 60;
  int secs = delta % 60;
  if (days > 0) {
    snprintf(buf, len, i18n_get(STR_UPDATED_D_H), days, hours);
  } else if (hours > 0) {
    snprintf(buf, len, i18n_get(STR_UPDATED_H_M), hours, mins);
  } else if (mins > 0) {
    snprintf(buf, len, i18n_get(STR_UPDATED_MIN), mins);
  } else {
    snprintf(buf, len, i18n_get(STR_UPDATED_SEC), secs);
  }
}

static void draw_last_update(GContext *ctx, GRect bounds) {
  char buf[40];
  format_last_update(gauges_last_update(), buf, sizeof(buf));
  if (buf[0] == '\0') {
    return;
  }

  int16_t y = bounds.size.h - TIME_Y - TIME_H;
  int16_t x0;
  int16_t avail = row_span(bounds, y, y + TIME_H, 2, &x0);
  graphics_context_set_text_color(ctx, COLOR_RESET_TEXT);
  graphics_draw_text(ctx, buf, fonts_get_system_font(FONT_KEY_GOTHIC_14),
                     GRect(x0, y, avail, TIME_H),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
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

  draw_time(ctx, bounds);
  draw_last_update(ctx, bounds);

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
  TimeUnits desired = desired_tick_units();
  if (desired != s_tick_units) {
    tick_timer_service_unsubscribe();
    tick_timer_service_subscribe(desired, tick_handler);
    s_tick_units = desired;
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

void ui_start_clock(void) {
  s_tick_units = desired_tick_units();
  tick_timer_service_subscribe(s_tick_units, tick_handler);
}

void ui_stop_clock(void) {
  tick_timer_service_unsubscribe();
}
