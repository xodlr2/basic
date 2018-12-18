#include "platform_config.h"
#include "tmc5041_reg.h"

#define DEFAULT_CURRENT_IN_MA   1000

#define LIMIT(min, max, value)  ((value<min) ? (value=min) : ((value>max) ? (value=max) : (value=value)))


SPI_HandleTypeDef Handle_SPI;
u8 tmc5041_TxBuffer[5] = {0, };
u8 tmc5041_RxBuffer[5] = {0, };

reg_w_amax reg_amax[2];
reg_w_vmax reg_vmax[2];
reg_w_dmax reg_dmax[2];
reg_rw_xtarget reg_xtarget[2];


unsigned long driver_status_result;
uint32_t uStepSec = 150;
uint32_t uMinStepSec = 120;
int stepflag = 0;

int set_currents[2];
volatile bool v_sense_high[2] = {TRUE, TRUE};

uint8_t tmc5041_WriteRegister(uint8_t address, uint32_t data);
uint32_t tmc5041_ReadRegister(uint8_t address);


/**
* @brief SPI MSP Initialization
* This function configures the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hspi->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspInit 0 */

  /* USER CODE END SPI2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI2_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI2 GPIO Configuration    
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI2_MspInit 1 */

  /* USER CODE END SPI2_MspInit 1 */
  }

}

/**
* @brief SPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

  if(hspi->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspDeInit 0 */

  /* USER CODE END SPI2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI2_CLK_DISABLE();
  
    /**SPI2 GPIO Configuration    
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

  /* USER CODE BEGIN SPI2_MspDeInit 1 */

  /* USER CODE END SPI2_MspDeInit 1 */
  }

}

void tmc5041_IO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    // __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = TMC5041_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TMC5041_CS_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = TMC5041_REFR1_PIN;
    HAL_GPIO_Init(TMC5041_REFR1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = TMC5041_REFL1_PIN;
    HAL_GPIO_Init(TMC5041_REFL1_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = TMC5041_REFR2_PIN;
    HAL_GPIO_Init(TMC5041_REFR2_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = TMC5041_REFL2_PIN;
    HAL_GPIO_Init(TMC5041_REFL2_PORT, &GPIO_InitStruct);

    tmc5041_SetRefPin(TMC5041_MOTOR_1, FALSE);
    tmc5041_SetRefPin(TMC5041_MOTOR_2, FALSE);

    GPIO_InitStruct.Pin = TMC5041_EN_PIN;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(TMC5041_EN_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(TMC5041_EN_PORT, TMC5041_EN_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TMC5041_CS_PORT, TMC5041_CS_PIN, GPIO_PIN_SET);

    Handle_SPI.Instance = TMC5041_SPI;
    Handle_SPI.Init.Mode = SPI_MODE_MASTER;
    Handle_SPI.Init.Direction = SPI_DIRECTION_2LINES;
    Handle_SPI.Init.DataSize = SPI_DATASIZE_8BIT;
    Handle_SPI.Init.CLKPolarity = SPI_POLARITY_HIGH;
    Handle_SPI.Init.CLKPhase = SPI_PHASE_2EDGE;
    Handle_SPI.Init.NSS = SPI_NSS_SOFT;
    Handle_SPI.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    Handle_SPI.Init.FirstBit = SPI_FIRSTBIT_MSB;
    Handle_SPI.Init.TIMode = SPI_TIMODE_DISABLE;
    Handle_SPI.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    Handle_SPI.Init.CRCPolynomial = 7;

    if (HAL_SPI_Init(&Handle_SPI) != HAL_OK)
    {
        // _Error_Handler(__FILE__, __LINE__);
        Error_Handler();
    }
}

void tmc5041_Init(void)
{
    reg_rw_gconf reg_gconf;
    reg_gconf.VALUE = 0;
    reg_gconf.bit.POSCMP_EN = 1;

    reg_rw_rampmode reg_ramp_mode;
    reg_ramp_mode.VALUE = 0;

    reg_rw_xtarget reg_xtarget = 0;

    reg_rw_xactual reg_xactual = 0;

    reg_chopconf chopper_control_register;
    chopper_control_register.VALUE = 0;
    chopper_control_register.bit.TOFF = 5;
    chopper_control_register.bit.HSTRT = 5;
    chopper_control_register.bit.HEND = 3;
    chopper_control_register.bit.RNDTF = 1;
    chopper_control_register.bit.TBL = 2;

    reg_rw_sw_mode reg_sw_mode;
    reg_sw_mode.VALUE = 0;
    reg_sw_mode.bit.STOP_L_ENABLE = 1;
    reg_sw_mode.bit.LATCH_L_ACTIVE = 1;
    reg_sw_mode.bit.STOP_R_ENABLE = 1;
    reg_sw_mode.bit.LATCH_R_ACTIVE = 1;

    //get rid of the 'something happened after reboot' warning
    tmc5041_ReadRegister(TMC5041_GENERAL_STATUS_REGISTER);
    tmc5041_WriteRegister(TMC5041_GENERAL_CONFIG_REGISTER, reg_gconf.VALUE); //int/PP are outputs
    // motor #1
    tmc5041_WriteRegister(TMC5041_RAMP_MODE_REGISTER_1, reg_ramp_mode.VALUE); //enforce positioing mode
    tmc5041_WriteRegister(TMC5041_X_TARGET_REGISTER_1, reg_xtarget);
    tmc5041_WriteRegister(TMC5041_X_ACTUAL_REGISTER_1, reg_xactual);
    tmc5041_SetCurrent(0, DEFAULT_CURRENT_IN_MA);
    chopper_control_register.VALUE |= tmc5041_ReadRegister(TMC5041_CHOPPER_CONFIGURATION_REGISTER_1);
    tmc5041_WriteRegister(TMC5041_CHOPPER_CONFIGURATION_REGISTER_1, chopper_control_register.VALUE);
    
    // motor #2
    //get rid of the 'something happened after reboot' warning
    tmc5041_WriteRegister(TMC5041_RAMP_MODE_REGISTER_2, reg_ramp_mode.VALUE); //enforce positioing mode
    tmc5041_WriteRegister(TMC5041_X_TARGET_REGISTER_2, reg_xtarget);
    tmc5041_WriteRegister(TMC5041_X_ACTUAL_REGISTER_2, reg_xactual);
    tmc5041_SetCurrent(1, DEFAULT_CURRENT_IN_MA);
    chopper_control_register.VALUE |= tmc5041_ReadRegister(TMC5041_CHOPPER_CONFIGURATION_REGISTER_2);
    tmc5041_WriteRegister(TMC5041_CHOPPER_CONFIGURATION_REGISTER_2, chopper_control_register.VALUE);
    
    //configure reference switches (to nothing)
    tmc5041_WriteRegister(TMC5041_REFERENCE_SWITCH_CONFIG_REGISTER_1, reg_sw_mode.VALUE);
    tmc5041_WriteRegister(TMC5041_REFERENCE_SWITCH_CONFIG_REGISTER_2, reg_sw_mode.VALUE);
    
    //those values are static and will anyway reside in the tmc5041 settings
    tmc5041_WriteRegister(TMC5041_A_1_REGISTER_1, 0);
    tmc5041_WriteRegister(TMC5041_D_1_REGISTER_1, 1); //the datahseet says it is needed
    tmc5041_WriteRegister(TMC5041_V_START_REGISTER_1, 0);
    tmc5041_WriteRegister(TMC5041_V_STOP_REGISTER_1, 1); //needed acc to the datasheet?
    tmc5041_WriteRegister(TMC5041_V_1_REGISTER_1, 0);
    tmc5041_WriteRegister(TMC5041_A_1_REGISTER_2, 0);
    tmc5041_WriteRegister(TMC5041_D_1_REGISTER_2, 1); //the datahseet says it is needed
    tmc5041_WriteRegister(TMC5041_V_START_REGISTER_2, 0);
    tmc5041_WriteRegister(TMC5041_V_STOP_REGISTER_2, 1); //needed acc to the datasheet?
    tmc5041_WriteRegister(TMC5041_V_1_REGISTER_2, 0);
    
    //and ensure that the event register are emtpy 
    tmc5041_ReadRegister(TMC5041_RAMP_STATUS_REGISTER_1);
    tmc5041_ReadRegister(TMC5041_RAMP_STATUS_REGISTER_2);

}

unsigned char calculateCurrentValue(char motor, int current)
{
    // I_RMS = ((CS+1)/32)*(V_FS/R_SENSE)*(1/SQRT(2))
    // CS = ((I_RMS*32*SQRT(2)*R_SENSE)/(V_FS))-1
    float i_rms = current/1000.0;
    float _cs;
    _cs = ((i_rms * 32 * SQRT_2 * TMC_5041_R_SENSE)/V_HIGH_SENSE)-0.5;
    v_sense_high[motor] = TRUE;
    if (_cs > 31)
    {
        _cs = ((i_rms * 32 * SQRT_2 * TMC_5041_R_SENSE)/V_LOW_SENSE)-0.5;
        v_sense_high[motor] = FALSE;
        if (_cs > 31)
        {
            return 31;
        }
        else if (_cs < 0)
        {
            return 0;
        }
        else
        {
            return (unsigned char)_cs;
        }
    }
    else if (_cs < 0)
    {
        return 0;
    }
    else
    {
        return (unsigned char)_cs;
    }
}

int tmc5041_GetCurrent(unsigned char motor_number)
{
    if (motor_number > 1)
    {
        return -1;
    }
    return set_currents[motor_number];
}


const char tmc5041_SetCurrent(unsigned char motor_number, int newCurrent)
{
    unsigned char run_current = calculateCurrentValue(motor_number, newCurrent);
    unsigned char hold_current = run_current;
    reg_w_ihold_irun current_register;
    current_register.VALUE = 0;

    printf("run_current : %d\r\n", run_current);

    //set the holding delay
    current_register.bit.IHOLDDELAY = 2;
    current_register.bit.IRUN = run_current;
    current_register.bit.IHOLD = hold_current;
    tmc5041_WriteRegister((motor_number == TMC5041_MOTOR_1) ? TMC5041_HOLD_RUN_CURRENT_REGISTER_1 : TMC5041_HOLD_RUN_CURRENT_REGISTER_2, current_register.VALUE);

    set_currents[motor_number] = (int)(((run_current + 1.0)/32.0)*((v_sense_high[motor_number]? V_HIGH_SENSE : V_LOW_SENSE) / TMC_5041_R_SENSE) * (1000.0 / SQRT_2));

    reg_chopconf chopper_control_register;
    chopper_control_register.VALUE = 0;
    chopper_control_register.bit.TOFF = 5;
    chopper_control_register.bit.HSTRT = 5;
    chopper_control_register.bit.HEND = 3;
    chopper_control_register.bit.RNDTF = 1;
    chopper_control_register.bit.TBL = 2;

    if (v_sense_high[motor_number])
    {
        chopper_control_register.bit.VSENSE = 1; // lower v_sense voltage
    }
    else
    {
        chopper_control_register.bit.VSENSE = 0; // higher v_sense voltage
    } 

    tmc5041_WriteRegister((motor_number == TMC5041_MOTOR_1) ? TMC5041_CHOPPER_CONFIGURATION_REGISTER_1 : TMC5041_CHOPPER_CONFIGURATION_REGISTER_2, chopper_control_register.VALUE);

    return 0;
}

uint32_t tmc5041_GetTargetPosition(char motor_nr)
{
    return tmc5041_ReadRegister((motor_nr==TMC5041_MOTOR_1) ? TMC5041_X_TARGET_REGISTER_1 : TMC5041_X_TARGET_REGISTER_2);
}

uint32_t tmc5041_GetActualPosition(char motor_nr)
{
    return tmc5041_ReadRegister((motor_nr==TMC5041_MOTOR_1) ? TMC5041_X_ACTUAL_REGISTER_1 : TMC5041_X_ACTUAL_REGISTER_2);
}

// vMax
void tmc5041_MotorPosition(bool bOnOff, unsigned char motor_nr, unsigned long vMax, unsigned long aMax, long position)
{
    if(motor_nr < 2)
    {
        long read_conf = 0;

        LIMIT(0, pow(2, 16)-1, aMax);
        LIMIT(0, pow(2, 23) - 512, vMax);
        reg_amax[motor_nr].VALUE = aMax;
        reg_dmax[motor_nr].VALUE = aMax;
        reg_vmax[motor_nr].VALUE = vMax;

        if(bOnOff)
        {
            if (reg_vmax[motor_nr].VALUE != 0)
            {
                tmc5041_WriteRegister((motor_nr == TMC5041_MOTOR_1) ? TMC5041_A_MAX_REGISTER_1 : TMC5041_A_MAX_REGISTER_2, reg_amax[motor_nr].VALUE);
                tmc5041_WriteRegister((motor_nr == TMC5041_MOTOR_1) ? TMC5041_D_MAX_REGISTER_1 : TMC5041_D_MAX_REGISTER_2, reg_dmax[motor_nr].VALUE);
                tmc5041_WriteRegister((motor_nr == TMC5041_MOTOR_1) ? TMC5041_V_MAX_REGISTER_1 : TMC5041_V_MAX_REGISTER_2, reg_vmax[motor_nr].VALUE);
                read_conf = tmc5041_ReadRegister((motor_nr==TMC5041_MOTOR_1) ? TMC5041_X_ACTUAL_REGISTER_1 : TMC5041_X_ACTUAL_REGISTER_2);
                tmc5041_WriteRegister((motor_nr == TMC5041_MOTOR_1) ? TMC5041_X_TARGET_REGISTER_1: TMC5041_X_TARGET_REGISTER_2, read_conf + position);
            }
        }
        else
        {
            tmc5041_SetRefPin(motor_nr, TRUE);
            read_conf = tmc5041_ReadRegister((motor_nr==TMC5041_MOTOR_1) ? TMC5041_X_ACTUAL_REGISTER_1 : TMC5041_X_ACTUAL_REGISTER_2);
            tmc5041_WriteRegister((motor_nr == TMC5041_MOTOR_1) ? TMC5041_X_TARGET_REGISTER_1: TMC5041_X_TARGET_REGISTER_2, read_conf);
            tmc5041_SetRefPin(motor_nr, FALSE);
        }
    }
}

void tmc5041_Chip_Select(bool set)
{
    HAL_GPIO_WritePin(TMC5041_CS_PORT, TMC5041_CS_PIN, (set ? GPIO_PIN_RESET : GPIO_PIN_SET));
    if(set)
    {
        Delay_us(100);
    }
}

void tmc5041_SetRefPin(unsigned char motor_nr, bool bOnOff)
{
    if(motor_nr == TMC5041_MOTOR_1)
    {
        HAL_GPIO_WritePin(TMC5041_REFR1_PORT, TMC5041_REFR1_PIN, bOnOff);
        HAL_GPIO_WritePin(TMC5041_REFL1_PORT, TMC5041_REFL1_PIN, bOnOff);
    }
    else
    {
        HAL_GPIO_WritePin(TMC5041_REFR2_PORT, TMC5041_REFR2_PIN, bOnOff);
        HAL_GPIO_WritePin(TMC5041_REFL2_PORT, TMC5041_REFL2_PIN, bOnOff);
    }

}

bool tmc5041_InvertMotor(char motor_nr, bool inverted)
{
    reg_rw_gconf reg_gconf;
    reg_gconf.VALUE = tmc5041_ReadRegister(TMC5041_GENERAL_CONFIG_REGISTER);

    if (motor_nr == 0)
    {
        reg_gconf.bit.SHAFT1 = inverted;
    } 
    else
    {
        reg_gconf.bit.SHAFT2 = inverted;
    }

    tmc5041_WriteRegister(TMC5041_GENERAL_CONFIG_REGISTER, reg_gconf.VALUE); 
    reg_gconf.VALUE = tmc5041_ReadRegister(TMC5041_GENERAL_CONFIG_REGISTER);
    //finally return if the bit iss set 
    return (motor_nr == 0) ? (reg_gconf.bit.SHAFT1 == inverted) : (reg_gconf.bit.SHAFT2 == inverted);
}

#define WRITE_ACCESS    0x80

uint8_t tmc5041_WriteRegister(uint8_t address, uint32_t data)
{
    uint8_t status;

    //PRINTF(LOG_DEBUG, "address : 0x%02x, data : 0x%08lx", address, data);

    tmc5041_TxBuffer[0] = address | WRITE_ACCESS;
    tmc5041_TxBuffer[1] = (data >> 24) & 0xff;
    tmc5041_TxBuffer[2] = (data >> 16) & 0xff;
    tmc5041_TxBuffer[3] = (data >> 8) & 0xff;
    tmc5041_TxBuffer[4] = data & 0xff;

    tmc5041_Chip_Select(TRUE);
    HAL_SPI_TransmitReceive(&Handle_SPI, tmc5041_TxBuffer, tmc5041_RxBuffer, 5, 100);
    tmc5041_Chip_Select(FALSE);

    status = tmc5041_RxBuffer[0];

    return status;
}

uint32_t tmc5041_ReadRegister(uint8_t address)
{
    uint32_t value;

    memset(tmc5041_TxBuffer, 0x00, 5);

    tmc5041_TxBuffer[0] = address;

    tmc5041_Chip_Select(TRUE);
    HAL_SPI_TransmitReceive(&Handle_SPI, tmc5041_TxBuffer, tmc5041_RxBuffer, 5, 100);
    tmc5041_Chip_Select(FALSE);

    Delay_us(1);

    tmc5041_Chip_Select(TRUE);
    HAL_SPI_TransmitReceive(&Handle_SPI, tmc5041_TxBuffer, tmc5041_RxBuffer, 5, 100);
    tmc5041_Chip_Select(FALSE);

	value = 0;
    value |= tmc5041_RxBuffer[1] << 24;
    value |= tmc5041_RxBuffer[2] << 16;
    value |= tmc5041_RxBuffer[3] << 8;
    value |= tmc5041_RxBuffer[4];

    // PRINTF(LOG_DEBUG, "value : 0x%08lx", value);
    printf("value : 0x%08lx\r\n", value);
    return value;
}


uint8_t tmc5041_Read_Status(void)
{
    uint8_t status;

    memset(tmc5041_TxBuffer, 0x00, 5);
    tmc5041_TxBuffer[0] = TMC5041_GENERAL_CONFIG_REGISTER;

    tmc5041_Chip_Select(TRUE);
    HAL_SPI_TransmitReceive(&Handle_SPI, tmc5041_TxBuffer, tmc5041_RxBuffer, 5, 100);
    tmc5041_Chip_Select(FALSE);

    status = tmc5041_RxBuffer[0];

    return status;
}

