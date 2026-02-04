#ifndef CPOST_CFG_H
#define CPOST_CFG_H

#include <stddef.h>
#include <stdint.h>

#include "main.h"

#define CPOST_GET_TICK()       HAL_GetTick()

#define CPOST_ENTER_CRITICAL() __disable_irq()
#define CPOST_EXIT_CRITICAL() __enable_irq()

#endif /* CPOST_CFG_H */
