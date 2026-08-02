#include "gauges.h"

#include "i18n.h"

#define PERSIST_KEY_LABELS 1
#define PERSIST_KEY_PERCENTS 2
#define PERSIST_KEY_RESETS 3
#define PERSIST_KEY_LAST_UPDATE 4

static int s_count = 0;
static char s_labels[MAX_GAUGES][MAX_LABEL_LEN];
static int s_percents[MAX_GAUGES];
static time_t s_resets[MAX_GAUGES];
// Vide tant qu'aucun statut n'a été reçu : gauges_status() retombe alors
// sur la chaîne traduite « Chargement... ».
static char s_status[MAX_STATUS_LEN] = "";
static time_t s_last_update = 0;

int gauges_count(void) {
  return s_count;
}

const char *gauges_label(int index) {
  return s_labels[index];
}

int gauges_percent(int index) {
  return s_percents[index];
}

time_t gauges_reset(int index) {
  return s_resets[index];
}

const char *gauges_status(void) {
  return s_status[0] != '\0' ? s_status : i18n_get(STR_LOADING);
}

void gauges_set_status(const char *status) {
  strncpy(s_status, status, sizeof(s_status) - 1);
  s_status[sizeof(s_status) - 1] = '\0';
}

time_t gauges_last_update(void) {
  return s_last_update;
}

// Découpe la chaîne sur '|' en place et renvoie le segment courant,
// ou NULL en fin de chaîne.
static char *next_token(char **cursor) {
  char *start = *cursor;
  if (!start || *start == '\0') {
    return NULL;
  }
  char *sep = strchr(start, '|');
  if (sep) {
    *sep = '\0';
    *cursor = sep + 1;
  } else {
    *cursor = start + strlen(start);
  }
  return start;
}

static void parse_joined(const char *labels_joined, const char *percents_joined,
                         const char *resets_joined) {
  char labels_buf[MAX_JOINED_LEN];
  char percents_buf[MAX_JOINED_LEN];
  char resets_buf[MAX_JOINED_LEN];
  strncpy(labels_buf, labels_joined, sizeof(labels_buf) - 1);
  labels_buf[sizeof(labels_buf) - 1] = '\0';
  strncpy(percents_buf, percents_joined, sizeof(percents_buf) - 1);
  percents_buf[sizeof(percents_buf) - 1] = '\0';
  strncpy(resets_buf, resets_joined, sizeof(resets_buf) - 1);
  resets_buf[sizeof(resets_buf) - 1] = '\0';

  s_count = 0;

  char *labels_cursor = labels_buf;
  char *percents_cursor = percents_buf;
  char *resets_cursor = resets_buf;
  char *label = next_token(&labels_cursor);
  char *pct = next_token(&percents_cursor);

  while (label && pct && s_count < MAX_GAUGES) {
    strncpy(s_labels[s_count], label, MAX_LABEL_LEN - 1);
    s_labels[s_count][MAX_LABEL_LEN - 1] = '\0';
    s_percents[s_count] = atoi(pct);
    char *reset = next_token(&resets_cursor);
    s_resets[s_count] = reset ? (time_t)atoi(reset) : 0;
    s_count++;
    label = next_token(&labels_cursor);
    pct = next_token(&percents_cursor);
  }
}

void gauges_set_from_joined(const char *labels_joined, const char *percents_joined,
                            const char *resets_joined) {
  parse_joined(labels_joined, percents_joined, resets_joined);
  persist_write_string(PERSIST_KEY_LABELS, labels_joined);
  persist_write_string(PERSIST_KEY_PERCENTS, percents_joined);
  persist_write_string(PERSIST_KEY_RESETS, resets_joined);
  s_last_update = time(NULL);
  persist_write_int(PERSIST_KEY_LAST_UPDATE, (int32_t)s_last_update);
}

void gauges_load_persisted(void) {
  char labels_buf[MAX_JOINED_LEN];
  char percents_buf[MAX_JOINED_LEN];
  char resets_buf[MAX_JOINED_LEN] = "";
  if (persist_exists(PERSIST_KEY_LABELS) && persist_exists(PERSIST_KEY_PERCENTS)) {
    persist_read_string(PERSIST_KEY_LABELS, labels_buf, sizeof(labels_buf));
    persist_read_string(PERSIST_KEY_PERCENTS, percents_buf, sizeof(percents_buf));
    if (persist_exists(PERSIST_KEY_RESETS)) {
      persist_read_string(PERSIST_KEY_RESETS, resets_buf, sizeof(resets_buf));
    }
    parse_joined(labels_buf, percents_buf, resets_buf);
  }
  if (persist_exists(PERSIST_KEY_LAST_UPDATE)) {
    s_last_update = (time_t)persist_read_int(PERSIST_KEY_LAST_UPDATE);
  }
}
