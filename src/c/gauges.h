#pragma once

#include <pebble.h>

// Modèle de données : jauges reçues du téléphone, message de statut,
// persistance des dernières valeurs connues.

#define MAX_GAUGES 8
#define MAX_LABEL_LEN 24
#define MAX_STATUS_LEN 96
#define MAX_JOINED_LEN 256

int gauges_count(void);
const char *gauges_label(int index);
int gauges_percent(int index);
// Epoch Unix de la prochaine réinitialisation, 0 si inconnue.
time_t gauges_reset(int index);

const char *gauges_status(void);
void gauges_set_status(const char *status);

// Epoch Unix de la dernière mise à jour des jauges reçue du téléphone,
// 0 si jamais reçue.
time_t gauges_last_update(void);

// Remplace les jauges depuis les chaînes "label|label|...", "pct|pct|..."
// et "epoch|epoch|..." puis les persiste sur la montre.
void gauges_set_from_joined(const char *labels_joined, const char *percents_joined,
                            const char *resets_joined);
void gauges_load_persisted(void);
