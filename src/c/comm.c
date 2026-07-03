#include <pebble.h>

#include "comm.h"
#include "gauges.h"
#include "ui.h"

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *labels_t = dict_find(iter, MESSAGE_KEY_GaugeLabels);
  Tuple *percents_t = dict_find(iter, MESSAGE_KEY_GaugePercents);
  Tuple *resets_t = dict_find(iter, MESSAGE_KEY_GaugeResets);
  Tuple *status_t = dict_find(iter, MESSAGE_KEY_Status);

  if (labels_t && percents_t) {
    gauges_set_from_joined(labels_t->value->cstring, percents_t->value->cstring,
                           resets_t ? resets_t->value->cstring : "");
    ui_reset_scroll();
  } else if (status_t && gauges_count() == 0) {
    gauges_set_status(status_t->value->cstring);
  }

  ui_mark_dirty();
}

void comm_init(void) {
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(512, 64);
}

void comm_request_refresh(void) {
  DictionaryIterator *iter;
  if (app_message_outbox_begin(&iter) == APP_MSG_OK) {
    dict_write_uint8(iter, MESSAGE_KEY_Refresh, 1);
    app_message_outbox_send();
  }
}
