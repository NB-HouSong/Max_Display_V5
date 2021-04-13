#ifndef __TURNLIGHT_H
#define __TURNLIGHT_H

#include "main.h"

typedef enum LED_STATUS_enum
{
    LED_ALL_OFF         = 0x00,
    LED_ALL_ON          = 0x03,
    LED_LEFT_ON         = 0x01,
    LED_RIGHT_ON        = 0x02,
    LED_LEFT_BROKEN     = 0x04,
    LED_RIGHT_BROKEN    = 0x08
} LED_STATUS;

void turnLightProcess(void);

#endif
