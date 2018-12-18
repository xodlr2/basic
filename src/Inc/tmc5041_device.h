#ifndef __TMC5041_DEVICE_H__
#define __TMC5041_DEVICE_H__

#include "platform_config.h"
#include "tmc5041_reg.h"

#define TMC5041_CS_PORT        GPIOB
#define TMC5041_CS_PIN         GPIO_PIN_12
#define TMC5041_EN_PORT        GPIOB
#define TMC5041_EN_PIN         GPIO_PIN_11

#define TMC5041_REFR1_PORT     GPIOA
#define TMC5041_REFR1_PIN      GPIO_PIN_3
#define TMC5041_REFL1_PORT     GPIOA
#define TMC5041_REFL1_PIN      GPIO_PIN_1

#define TMC5041_REFR2_PORT     GPIOA
#define TMC5041_REFR2_PIN      GPIO_PIN_2
#define TMC5041_REFL2_PORT     GPIOA
#define TMC5041_REFL2_PIN      GPIO_PIN_0

#define TMC5041_SPI            SPI2

#define TMC5041_MOTOR_1        0
#define TMC5041_MOTOR_2        1

#define MOTOR_STATUS_NONE       0x00
#define MOTOR_STATUS_LEFT       (0x01 << 0)
#define MOTOR_STATUS_RIGHT      (0x01 << 1)
#define MOTOR_STATUS_UP         (0x01 << 2)
#define MOTOR_STATUS_DOWN       (0x01 << 3)
#define MOTOR_STATUS_DIR        (0x03)
#define MOTOR_STATUS_SPEED      (0x0C)

#define MotorDIR(a)             (a & MOTOR_STATUS_DIR)
#define MotorSPEED(a)           (a & MOTOR_STATUS_SPEED)

#define STEP_MOTOR_MAX_SPEED    300
#define STEP_MOTOR_MIN_SPEED    35
#define STEP_MOTOR_OFFSET_VALUE 5000

void tmc5041_IO_Init(void);
void tmc5041_Init(void);
void tmc5041_SetRefPin(unsigned char motor_nr, bool bOnOff);
const char tmc5041_SetCurrent(unsigned char motor_number, int newCurrent);
void tmc5041_MotorPosition(bool bOnOff, unsigned char motor_nr, unsigned long vMax, unsigned long aMax, long position);
uint32_t tmc5041_GetTargetPosition(char motor_nr);
uint32_t tmc5041_GetActualPosition(char motor_nr);


#endif  //__TMC5041_DEVICE_H__
