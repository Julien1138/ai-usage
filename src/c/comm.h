#pragma once

// Communication AppMessage avec le téléphone (PebbleKit JS).

void comm_init(void);
// Demande au téléphone de rafraîchir les jauges immédiatement.
void comm_request_refresh(void);
