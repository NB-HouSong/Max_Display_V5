#include <stdlib.h>

#include "Gpio.h"

typedef struct
{
    bool                status;
    GPIO_TypeDef       *type;
    GPIO_InitTypeDef    initType;
} GpioPin;

static GpioPin gpioPin[GpioMaxNumber] =
{
    //Output
    {false, GPIOB, {GPIO_Pin_14, GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},       // GpioOutRightLightCtrl
    {false, GPIOB, {GPIO_Pin_15, GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},       // GpioOutTurnSignalRgbPowCtrl
    {false, GPIOA, {GPIO_Pin_8,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},       // GpioOutLeftLightCtrl
    {false, GPIOA, {GPIO_Pin_11, GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},       // GpioOut5VSysPowCtrl
    
    {false, GPIOC, {GPIO_Pin_5,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},       // GpioOutPowCtrl
    {false, GPIOA, {GPIO_Pin_12, GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},       // GpioOutFrountLedCtrl
    {false, GPIOB, {GPIO_Pin_6,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},       // GpioOutWirelessChargePowCtrl
    {false, GPIOB, {GPIO_Pin_7,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},       // GpioOutGPSPowCtrl

    {false, GPIOC, {GPIO_Pin_9,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},       // GpioOutBeep
    
    //Input
    {false, GPIOC, {GPIO_Pin_14, GPIO_Mode_IN, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},        // GpioVsersionA
    {false, GPIOC, {GPIO_Pin_14, GPIO_Mode_IN, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},        // GpioVsersionB
    {false, GPIOF, {GPIO_Pin_0,  GPIO_Mode_IN, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},        // GpioInLeftButton
    {false, GPIOF, {GPIO_Pin_1,  GPIO_Mode_IN, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},        // GpioInRightButton
    
    {false, GPIOA, {GPIO_Pin_1,  GPIO_Mode_IN, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}},        // GpioInWirelessFB
};

/* Public function prototypes -----------------------------------------------*/
void gpioInitialize(void)
{
    for (uint32_t i = 0; i < GpioMaxNumber; ++i)
    {
        GPIO_Init(gpioPin[i].type, &gpioPin[i].initType);
    }
}

void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

void gpioSetOutput(GpioPinName pin, bool state)
{
    if(state)
    {
        GPIO_SetBits(gpioPin[pin].type, gpioPin[pin].initType.GPIO_Pin);
    }
    else
    {
        GPIO_ResetBits(gpioPin[pin].type, gpioPin[pin].initType.GPIO_Pin);
    }
}

void gpioSetToggle(GpioPinName pin)
{
    uint32_t odr;

    /* get current Output Data Register value */
    odr = gpioPin[pin].type->ODR;

    /* Set selected pins that were at low level, and reset ones that were high */
    gpioPin[pin].type->BSRR = ((odr & gpioPin[pin].initType.GPIO_Pin) << 16) | (~odr & gpioPin[pin].initType.GPIO_Pin);
}

bool gpioGetInput(GpioPinName pin)
{
    return GPIO_ReadInputDataBit(gpioPin[pin].type, gpioPin[pin].initType.GPIO_Pin) == 1;
}

bool gpioGetOutput(GpioPinName pin)
{
    return GPIO_ReadOutputDataBit(gpioPin[pin].type, gpioPin[pin].initType.GPIO_Pin) == 1;
}
