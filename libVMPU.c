#include "libVMPU.h"

/**
 * @brief writes one byte to a specific address. Needs to be changed if STM-HAL drivers are not used
 * 
 * @param data ptr to byte
 * @param adr address to write to
 * @return VMPU_STATUS 
 */
VMPU_STATUS MPUwriteByteTo(uint8_t* data,uint16_t adr)
{
   HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(&hi2c1,(uint16_t)VMPU_ADR,adr,I2C_MEMADD_SIZE_8BIT,data,I2C_MEMADD_SIZE_8BIT,HAL_MAX_DELAY);
   if (ret==HAL_OK) return VMPU_OK;
   else return VMPU_ERR;
}

/**
 * @brief reads one byte from a specific address. Needs to be changed if STM-HAL drivers are not used
 * 
 * @param adr address to read from
 * @return read byte
 */
uint8_t MPUreadFromAddress(uint16_t adr)
{
   uint8_t buf;
   HAL_StatusTypeDef ret =HAL_I2C_Mem_Read(&hi2c1,(uint16_t)VMPU_ADR,adr,1,&buf,1,HAL_MAX_DELAY);
   if (ret==HAL_OK) return buf;
   else return 0;
}

VMPU_STATUS VMPU_setCLK(VMPUclk clkSrc);
VMPU_STATUS VMPU_setScale(VMPUscale scal);
VMPU_STATUS VMPU_setRange(VMPUrange range);
VMPU_STATUS VMPU_resetSigPath();
VMPU_STATUS VMPU_wake();
VMPU_STATUS VMPU_setDHPF(VMPUaccelDHPF dhpf);
VMPU_STATUS VMPU_setIntPinCFG(VMPUintPinCfg intPinCfg);
VMPU_STATUS VMPU_setAccPwrOnDelay(VMPUaccelPwrOnDelay AccPwrOnDel);
VMPU_STATUS VMPU_setFF_thres(uint8_t thres);
VMPU_STATUS VMPU_setFF_dur(uint8_t dur);
VMPU_STATUS VMPU_setFF_dec(VMPUcountDecrement dec);
VMPU_STATUS VMPU_setMot_thres(uint8_t thres);
VMPU_STATUS VMPU_setMot_dur(uint8_t dur);
VMPU_STATUS VMPU_setMot_dec(VMPUcountDecrement dec);


/**
 * @brief initializes the MPU6050 according to parameters
 * 
 * @param clkSrc Clock source 
 * @param scal Gyro sensitivity
 * @param range Accel sensitivity
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_init(VMPUclk clkSrc, VMPUscale scal, VMPUrange range)
{
   VMPU_STATUS ret;
   ret = VMPU_setCLK(clkSrc);
   if (ret != VMPU_OK) return VMPU_ERR;
   ret = VMPU_setScale(scal);
   if (ret != VMPU_OK) return VMPU_ERR;
   ret = VMPU_setRange(range);
   if (ret != VMPU_OK) return VMPU_ERR;
   ret = VMPU_resetSigPath();
   if (ret != VMPU_OK) return VMPU_ERR;
   ret = VMPU_wake();
   if (ret != VMPU_OK) return VMPU_ERR;
   return VMPU_OK;
   
}

/**
 * @brief initializes extra registers needed for FF and MOT interrupts
 * 
 * @param dhpf sets the digital highpass filter [defined parameters]
 * @param intPin configures properties of the INT-Pin [defined parameters]
 * @param AccPwrOnDel Sets the Power on delay of the accelerometer. Internally also used for the decrement of MOT [defined parameters]
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_initINT(VMPUaccelDHPF dhpf, VMPUintPinCfg intPin, VMPUaccelPwrOnDelay AccPwrOnDel)
{
   VMPU_STATUS ret;
   ret = VMPU_setDHPF(dhpf);
   if (ret != VMPU_OK) return VMPU_ERR;
   ret = VMPU_setIntPinCFG(intPin);
   if (ret != VMPU_OK) return VMPU_ERR;
   ret = VMPU_setAccPwrOnDelay(AccPwrOnDel);
   if (ret != VMPU_OK) return VMPU_ERR;
   return VMPU_OK;
}

/**
 * @brief initializes the FREE FALL interrupt
 * 
 * @param thres Threshold to trigger
 * @param dur Duration after witch a freefall is valid
 * @param dec decrement over time
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_initINT_FF(uint8_t thres, uint8_t dur, VMPUcountDecrement dec)
{
   VMPU_STATUS ret;
   ret = VMPU_setFF_thres(thres);
   if (ret != VMPU_OK) return VMPU_ERR;
   ret = VMPU_setFF_dur(dur);
   if (ret != VMPU_OK) return VMPU_ERR;
   ret = VMPU_setFF_dec(dec);
   if (ret != VMPU_OK) return VMPU_ERR;
   return VMPU_OK;
}

/**
 * @brief initializes the MOTION DETECTION interrupt
 * 
 * @param thres Threshold to trigger
 * @param dur Duration after witch a freefall is valid
 * @param dec decrement over time [defined parameters]
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_initINT_MOT(uint8_t thres, uint8_t dur, VMPUcountDecrement dec)
{
   VMPU_STATUS ret;
   ret = VMPU_setMot_thres(thres);
   if (ret != VMPU_OK) return VMPU_ERR;
   ret = VMPU_setMot_dur(dur);
   if (ret != VMPU_OK) return VMPU_ERR;
   ret = VMPU_setMot_dec(dec);
   if (ret != VMPU_OK) return VMPU_ERR;
   return VMPU_OK;
   }

/**
 * @brief enables interrupts according to bitmask
 * 
 * @param interruptMask bitmask [defined parameters] [FF,MOT,ZMOT,FIFO,PLL,DMP,DATRDY,I2CMe] 
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPUsetIntEnable(uint8_t interruptMask)
{
   uint8_t regBuf = interruptMask;
   return MPUwriteByteTo(&regBuf,VMPU_reg_INT_ENABLE);
}

/**
 * @brief configures clock source
 * 
 * @param clkSrc [defined parameters]
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_setCLK(VMPUclk clkSrc)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_PWRMGMT1);
   regBuf &= ~(0b111 << VMPU_offs_PWRMGMT1_clk);
   regBuf |= ((clkSrc & 0b111) << VMPU_offs_PWRMGMT1_clk);
   return MPUwriteByteTo(&regBuf,VMPU_reg_PWRMGMT1);
}
/**
 * @brief sets the scale for the gyroscope
 * 
 * @param scal [defined parameters]
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_setScale(VMPUscale scal)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_GYCFG);
   regBuf &= ~(0b11 << VMPU_offs_GYCFG_scale);
   regBuf |= ((scal & 0b11) << VMPU_offs_GYCFG_scale);
   return MPUwriteByteTo(&regBuf,VMPU_reg_GYCFG);
}

/**
 * @brief sets range of accelerometer
 * 
 * @param range [defined parameters]
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_setRange(VMPUrange range)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_ACCFG);
   regBuf &= ~(0b11 << VMPU_offs_ACCFG_range);
   regBuf |= ((range & 0b11)<< VMPU_offs_ACCFG_range);
   return MPUwriteByteTo(&regBuf,VMPU_reg_ACCFG);
}

/**
 * @brief resets the internal signal paths
 * 
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_resetSigPath()
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_SIG_PATH_RESET);
   regBuf &= ~(0b111 << VMPU_offs_SIG_PATH_RESET);
   regBuf |= (((VMPUresetSigPath_Accel | VMPUresetSigPath_Gyro | VMPUresetSigPath_Temp) & 0b111)<<VMPU_offs_SIG_PATH_RESET);
   return MPUwriteByteTo(&regBuf,VMPU_reg_SIG_PATH_RESET);
}

/**
 * @brief wakes the mpu up
 * 
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_wake()
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_PWRMGMT1);
   regBuf &= ~(0b1 << VMPU_offs_PWRMGMT1_sleep);
   return MPUwriteByteTo(&regBuf,VMPU_reg_PWRMGMT1);
}

/**
 * @brief sets the digital high pass filter
 * 
 * @param dhpf [defined parameters]
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_setDHPF(VMPUaccelDHPF dhpf)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_ACCFG);
   regBuf &= ~(0b111 << VMPU_offs_ACCFG_DHPF);
   regBuf |= ((dhpf & 0b111) << VMPU_offs_ACCFG_DHPF);
   return MPUwriteByteTo(&regBuf,VMPU_reg_ACCFG);
}

/**
 * @brief configures the properties of the INT-Pin
 * 
 * @param intPinCfg [defined parameters]
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_setIntPinCFG(VMPUintPinCfg intPinCfg)
{
   uint8_t regBuf = intPinCfg;
   return MPUwriteByteTo(&regBuf,VMPU_reg_INT_PIN_CFG);
}

/**
 * @brief sets the delay of the internal initialisation of the accelerometer on power-on
 * 
 * @param AccPwrOnDel [defined parameters]
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_setAccPwrOnDelay(VMPUaccelPwrOnDelay AccPwrOnDel)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_MOT_DECT_CFG);
   regBuf &= ~(0b11 << VMPU_offs_MOT_DECT_CFG_APOD);
   regBuf |= ((AccPwrOnDel & 0b11) << VMPU_offs_MOT_DECT_CFG_APOD);
   return MPUwriteByteTo(&regBuf,VMPU_reg_MOT_DECT_CFG);
}

/**
 * @brief sets the threshold of the FF interrupt
 * 
 * @param thres 8 bit value
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_setFF_thres(uint8_t thres)
{
   uint8_t regBuf = thres;
   return MPUwriteByteTo(&regBuf,VMPU_reg_FF_THR);
}
/**
 * @brief sets the duration for the FF interrupt
 * 
 * @param dur 8 bit value
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_setFF_dur(uint8_t dur)
{
   uint8_t regBuf = dur;
   return MPUwriteByteTo(&regBuf,VMPU_reg_FF_DUR);
}

/**
 * @brief sets the decrement over time for FF int
 * 
 * @param dec [defined parameters]
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_setFF_dec(VMPUcountDecrement dec)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_MOT_DECT_CFG);
   regBuf &= ~(0b11 << VMPU_offs_MOT_DECT_CFG_FFDEC);
   regBuf |= ((dec & 0b11) << VMPU_offs_MOT_DECT_CFG_FFDEC);
   return MPUwriteByteTo(&regBuf,VMPU_reg_MOT_DECT_CFG);
}

/**
 * @brief sets the threshold of the MOT interrupt
 * 
 * @param thres 8 bit value
 * @return VMPU_STATUS 
*/
VMPU_STATUS VMPU_setMot_thres(uint8_t thres)
{
   uint8_t regBuf = thres;
   return MPUwriteByteTo(&regBuf,VMPU_reg_MOT_THR);
}

/**
 * @brief sets the duration for the MOT interrupt
 * 
 * @param dur 8 bit value
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_setMot_dur(uint8_t dur)
{
   uint8_t regBuf = dur;
   return MPUwriteByteTo(&regBuf,VMPU_reg_MOT_DUR);
}

/**
 * @brief sets the decrement over time for MOT int
 * 
 * @param dec [defined parameters]
 * @return VMPU_STATUS 
 */
VMPU_STATUS VMPU_setMot_dec(VMPUcountDecrement dec)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_MOT_DECT_CFG);
   regBuf &= ~(0b11 << VMPU_offs_MOT_DECT_CFG_MOTDEC);
   regBuf |= ((dec & 0b11) << VMPU_offs_MOT_DECT_CFG_MOTDEC);
   return MPUwriteByteTo(&regBuf,VMPU_reg_MOT_DECT_CFG);
}

