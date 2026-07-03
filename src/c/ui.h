#pragma once

#include <pebble.h>

// Rendu : en-tête « Claude usage », jauges adaptées à la forme de l'écran
// (largeur par corde du cercle sur les écrans ronds), défilement.

void ui_window_load(Window *window);
void ui_window_unload(Window *window);
void ui_mark_dirty(void);
// direction : +1 = remonter dans le contenu, -1 = descendre.
void ui_scroll(int direction);
// Revient en haut du contenu (à l'arrivée de nouvelles données).
void ui_reset_scroll(void);
