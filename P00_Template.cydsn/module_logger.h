#ifndef MODULE_LOGGER_H_
#define MODULE_LOGGER_H_

/** \file
 * \author Ryan-David Reyes
 *
 * \brief This implements a request-reply paradigm between the PSOC and the
 * modules.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_WAIT_ATTEMPTS 5

typedef enum {
  MODULE_RIGHT_HIP_ID = 0,
  MODULE_RIGHT_KNEE_ID,
  MODULE_LEFT_HIP_ID,
  MODULE_LEFT_KNEE_ID,
  MODULE_ID_MAX
} module_id_t;

typedef struct module_logger_s {
  bool sent[MODULE_ID_MAX];
  bool received[MODULE_ID_MAX];
  size_t active_module;
  size_t wait_attempts;
  bool paused;
} module_logger_t;

/* function declarations */
/**
 * \brief Clears the tracking struct, and initializes everything to default.
 *
 * \param module_logger the module logging data structure
 */
void ml_clear(module_logger_t *module_logger);

/**
 * \brief Clears the tracking struct, and initializes everything to default.
 * Alias for clear
 *
 * \param module_logger the module logging data structure
 */
void ml_init(module_logger_t *module_logger);


/**
 * \brief Queries the module, then does nothing until it replies. Then queries
 * the next module.
 *
 * \param module_logger the module logging data structure
 */
void ml_queryModules(module_logger_t *module_logger);


/**
 * \brief Returns true if the messages from all the modules have been received.
 *
 * \param module_logger the module logging data structure
 */
bool ml_checkAllReceived(module_logger_t *module_logger);


/**
 * \brief Returns the number of the module we are waiting on.
 *
 * \param module_logger the module logging data structure
 */
uint8_t ml_getActiveModule(module_logger_t *module_logger);


/**
 * \brief Indicates to the logger that we received a message from module_id
 *
 */
void ml_moduleReceived(module_logger_t *module_logger, uint8_t module_id);


#ifdef __cplusplus
}
#endif
#endif
