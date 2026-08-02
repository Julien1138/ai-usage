#pragma once

#include <pebble.h>

// Internationalisation : chaînes de l'interface en anglais et en français,
// choisies à l'exécution d'après la langue de la montre
// (i18n_get_system_locale). L'anglais est la langue par défaut.

typedef enum {
  STR_TITLE,             // En-tête « Claude usage »
  STR_LOADING,           // Statut initial avant la première réponse
  STR_REFRESHING,        // Statut pendant un rafraîchissement forcé
  STR_RESET_IMMINENT,    // Réinitialisation dans moins d'une minute
  STR_RESET_IN_D_H,      // « Réinit. dans %dj %dh »
  STR_RESET_IN_H_M,      // « Réinit. dans %dh%02d »
  STR_RESET_IN_MIN,      // « Réinit. dans %d min »
  STR_UPDATED_D_H,       // « Mis à jour il y a %dj %dh »
  STR_UPDATED_H_M,       // « Mis à jour il y a %dh%02d »
  STR_UPDATED_MIN,       // « Mis à jour il y a %d min »
  STR_UPDATED_SEC,       // « Mis à jour il y a %d s »
  STR_COUNT
} I18nStringId;

// Chaîne traduite pour l'identifiant donné. La langue est relue à chaque
// appel : la locale de la montre peut changer pendant que l'app tourne.
const char *i18n_get(I18nStringId id);
