#include "i18n.h"

static const char *const s_english[STR_COUNT] = {
  [STR_TITLE] = "Claude usage",
  [STR_LOADING] = "Loading...",
  [STR_REFRESHING] = "Refreshing...",
  [STR_RESET_IMMINENT] = "Resets soon",
  [STR_RESET_IN_D_H] = "Resets in %dd %dh",
  [STR_RESET_IN_H_M] = "Resets in %dh%02d",
  [STR_RESET_IN_MIN] = "Resets in %d min",
  [STR_UPDATED_D_H] = "Updated %dd %dh ago",
  [STR_UPDATED_H_M] = "Updated %dh%02d ago",
  [STR_UPDATED_MIN] = "Updated %d min ago",
  [STR_UPDATED_SEC] = "Updated %d s ago",
};

static const char *const s_french[STR_COUNT] = {
  [STR_TITLE] = "Claude usage",
  [STR_LOADING] = "Chargement...",
  [STR_REFRESHING] = "Actualisation...",
  [STR_RESET_IMMINENT] = "Réinit. imminente",
  [STR_RESET_IN_D_H] = "Réinit. dans %dj %dh",
  [STR_RESET_IN_H_M] = "Réinit. dans %dh%02d",
  [STR_RESET_IN_MIN] = "Réinit. dans %d min",
  [STR_UPDATED_D_H] = "Mis à jour il y a %dj %dh",
  [STR_UPDATED_H_M] = "Mis à jour il y a %dh%02d",
  [STR_UPDATED_MIN] = "Mis à jour il y a %d min",
  [STR_UPDATED_SEC] = "Mis à jour il y a %d s",
};

const char *i18n_get(I18nStringId id) {
  const char *locale = i18n_get_system_locale();
  if (locale && strncmp(locale, "fr", 2) == 0) {
    return s_french[id];
  }
  return s_english[id];
}
