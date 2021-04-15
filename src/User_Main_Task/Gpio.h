/**
 * @file Gpio.h
 *
 * @brief This file contains the basic GPIO functionality.
 */

#ifndef GPIO_H
#define GPIO_H
#include <stdbool.h>

#include "main.h"

typedef enum
{
    //Output
    GpioOutRightLightCtrl,          // ��ת�ƿ�������
    GpioOutTurnSignalRgbPowCtrl,    // ת��Ʒ�Χ�Ƶ�Դ��������
    GpioOutLeftLightCtrl,           // ��ת�ƿ�������
    GpioOut5VSysPowCtrl,            // 5V��Դ��������
    
    GpioOutPowCtrl,                 // ����Դ��������
    GpioOutFrontLedCtrl,            // ǰ�ƿ�������
    GpioOutWirelessChargePowCtrl,   // ���߳��Դ��������
    GpioOutGPSPowCtrl,              // Gps��Դ��������
    
    GpioOutBeep,                    // �������������� 
    
    //Input
    GpioVsersionA,                  // Check the hardware version A
    GpioVsersionB,                  // Check the hardware version B
    GpioInLeftButton,               // Check the turn light left button
    GpioInRightButton,              // Check the turn light right button

    GpioInWirelessFB,               // ���߳�������
    
    GpioMaxNumber,
} GpioPinName;

/**
 * @brief Initializes the Gpio, this function should be executed before any other function.
 * @param void.
 */
void gpioInitialize(void);

/**
 * @brief Set the output state of a pin.
 * @param pin The pin to set.
 * @param state The state to set, true is ON, false is OFF.
 */
void gpioSetOutput(GpioPinName pin, bool state);

/**
  * @brief  Toggle the specified GPIO pin.
  * @param  GPIO_Pin specifies the pin to be toggled.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void gpioSetToggle(GpioPinName pin);

/**
 * @brief Get the state of an input pin.
 * @param pin The pin for which to get the state.
 * @return The state of the pin, true is ON, false is OFF.
 */
bool gpioGetInput(GpioPinName pin);

/**
 * @brief Get the state of an output pin.
 * @param pin The pin for which to get the state.
 * @return The state of the pin, true is ON, false is OFF.
 */
bool gpioGetOutput(GpioPinName pin);

#endif // GPIO_H
