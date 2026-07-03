#pragma once

#include <pebble.h>

// Couleurs claude.ai : fond ivoire, texte anthracite, barre corail.
// Sur les écrans noir & blanc, on retombe sur du noir sur blanc.
#define COLOR_BG PBL_IF_COLOR_ELSE(GColorFromHEX(0xF0EEE5), GColorWhite)
#define COLOR_TEXT PBL_IF_COLOR_ELSE(GColorFromHEX(0x262624), GColorBlack)
#define COLOR_ACCENT PBL_IF_COLOR_ELSE(GColorFromHEX(0xD97757), GColorBlack)
// Attention : les gris chauds de claude.ai s'arrondissent en blanc sur la
// palette 64 couleurs de Pebble, d'où le GColorLightGray.
#define COLOR_TRACK PBL_IF_COLOR_ELSE(GColorLightGray, GColorWhite)
// Yeux de la mascotte : anthracite sur corps corail, blancs sur corps noir.
#define COLOR_MASCOT_EYE PBL_IF_COLOR_ELSE(GColorFromHEX(0x262624), GColorWhite)
// Texte secondaire (compte à rebours de réinitialisation).
#define COLOR_RESET_TEXT PBL_IF_COLOR_ELSE(GColorDarkGray, GColorBlack)
