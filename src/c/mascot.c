#include "mascot.h"

#include "theme.h"

// Sprite 14x10 : 'X' = corps corail, 'e' = œil, '.' = transparent.
// Clawd est un petit personnage 8-bit trapu : deux pinces au-dessus,
// deux yeux carrés, des petites pattes.
static const char *const MASCOT_ROWS[MASCOT_H] = {
  "..XX......XX..",
  ".XXX......XXX.",
  ".XX.XXXXXX.XX.",
  "..XXXXXXXXXX..",
  "..XXeeXXeeXX..",
  "..XXeeXXeeXX..",
  "..XXXXXXXXXX..",
  "..XXXXXXXXXX..",
  "...XXXXXXXX...",
  "..XX..XX..XX..",
};

void mascot_draw(GContext *ctx, GPoint origin) {
  for (int row = 0; row < MASCOT_H; row++) {
    for (int col = 0; col < MASCOT_W; col++) {
      char pixel = MASCOT_ROWS[row][col];
      if (pixel == '.') {
        continue;
      }
      graphics_context_set_fill_color(ctx, pixel == 'e' ? COLOR_MASCOT_EYE
                                                        : COLOR_ACCENT);
      graphics_fill_rect(ctx, GRect(origin.x + col, origin.y + row, 1, 1), 0,
                         GCornerNone);
    }
  }
}
