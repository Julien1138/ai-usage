#pragma once

#include <pebble.h>

// Clawd, la mascotte de Claude Code, en pixel art.

#define MASCOT_W 14
#define MASCOT_H 10

void mascot_draw(GContext *ctx, GPoint origin);
