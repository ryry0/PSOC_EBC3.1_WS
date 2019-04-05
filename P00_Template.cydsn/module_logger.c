#include <module_logger.h>
#include <board_can.h>
#include <packet_hybrid.h>

#define FIRST_MODULE_ID MODULE_RIGHT_HIP_ID
  //should start from 0 if psoc

void ml_clear(module_logger_t *module_logger) {
  for (size_t i = 0; i < MODULE_ID_MAX; ++i) {
    module_logger->sent[i] = false;
    module_logger->received[i] = false;
  }
  module_logger->paused = false;
  module_logger->wait_attempts = 0;
  module_logger->active_module = FIRST_MODULE_ID;
}

inline void ml_init(module_logger_t *module_logger) {
  ml_clear(module_logger);
}


void ml_queryModules(module_logger_t *module_logger) {
  pkt_generic_t query_message;
  pkt_init(&query_message);


  pkt_setHeader(&query_message, PKT_QUERY_MODULE, sizeof(pkt_query_module_t));
  pkt_query_module_t *query_message_payload = pkt_interp(pkt_query_module_t,
      query_message);

  query_message_payload->module_id = module_logger->active_module;

  //if the active module is still a valid id
  if (module_logger->active_module < MODULE_ID_MAX) {
    //if we haven't sent a thing to the active module yet;
    if (!module_logger->sent[ module_logger->active_module ]) {

      bd_writeCAN(pkt_encodeBuffer(&query_message),
          pkt_getTotalLength(&query_message));
      module_logger->sent[ module_logger->active_module ] = true;
    } else { //else wait till MAX_WAIT_ATTEMPTS and try again

      module_logger->wait_attempts++;
      if (module_logger->wait_attempts > MAX_WAIT_ATTEMPTS) {

        bd_writeCAN(pkt_encodeBuffer(&query_message),
            pkt_getTotalLength(&query_message));
        module_logger->wait_attempts = 0;
      }

    } //end else
  }
}

void ml_moduleReceived(module_logger_t *module_logger, uint8_t module_id) {
  /*
   * if we received a module reply from the active module
   * increment the active module
   */

  if (module_logger->active_module == module_id) {

    module_logger->received[module_logger->active_module] = true;
    module_logger->active_module ++;

  }
}

bool ml_checkAllReceived(module_logger_t *module_logger) {
  bool ret_val = true;
  for (size_t i = FIRST_MODULE_ID; i < MODULE_ID_MAX; ++i) {
    //if we haven't received one of the messages
    if (!module_logger->received[i])
      ret_val = false;
  }
  return ret_val;
}

uint8_t ml_getActiveModule(module_logger_t *module_logger) {
  return module_logger->active_module;
}
