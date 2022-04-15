#include "libVMPU.h"

#pragma region ReadWrite_Func
VMPU_STATUS MPUwriteByteTo(uint8_t* data,uint16_t adr)
{
   HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(&hi2c1,(uint16_t)VMPU_ADR,adr,I2C_MEMADD_SIZE_8BIT,data,I2C_MEMADD_SIZE_8BIT,HAL_MAX_DELAY);
   if (ret==HAL_OK) return VMPU_OK;
   else return VMPU_ERR;
}
uint8_t MPUreadFromAddress(uint16_t adr)
{
   uint8_t buf;
   HAL_StatusTypeDef ret =HAL_I2C_Mem_Read(&hi2c1,(uint16_t)VMPU_ADR,adr,1,&buf,1,HAL_MAX_DELAY);
   if (ret==HAL_OK) return buf;
   else return 0;
}
#pragma endregion 
#pragma region Private_Declarations
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
#pragma endregion
#pragma region Public_Definitions
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


VMPU_STATUS VMPUsetIntEnable(uint8_t interruptMask)
{
   uint8_t regBuf = interruptMask;
   return MPUwriteByteTo(&regBuf,VMPU_reg_INT_ENABLE);
}
#pragma endregion
#pragma region Private_Definitions
VMPU_STATUS VMPU_setCLK(VMPUclk clkSrc)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_PWRMGMT1);
   regBuf &= ~(0b111 << VMPU_offs_PWRMGMT1_clk);
   regBuf |= ((clkSrc & 0b111) << VMPU_offs_PWRMGMT1_clk);
   return MPUwriteByteTo(&regBuf,VMPU_reg_PWRMGMT1);
}
VMPU_STATUS VMPU_setScale(VMPUscale scal)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_GYCFG);
   regBuf &= ~(0b11 << VMPU_offs_GYCFG_scale);
   regBuf |= ((scal & 0b11) << VMPU_offs_GYCFG_scale);
   return MPUwriteByteTo(&regBuf,VMPU_reg_GYCFG);
}
VMPU_STATUS VMPU_setRange(VMPUrange range)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_ACCFG);
   regBuf &= ~(0b11 << VMPU_offs_ACCFG_range);
   regBuf |= ((range & 0b11)<< VMPU_offs_ACCFG_range);
   return MPUwriteByteTo(&regBuf,VMPU_reg_ACCFG);
}
VMPU_STATUS VMPU_resetSigPath()
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_SIG_PATH_RESET);
   regBuf &= ~(0b111 << VMPU_offs_SIG_PATH_RESET);
   regBuf |= (((VMPUresetSigPath_Accel | VMPUresetSigPath_Gyro | VMPUresetSigPath_Temp) & 0b111)<<VMPU_offs_SIG_PATH_RESET);
   return MPUwriteByteTo(&regBuf,VMPU_reg_SIG_PATH_RESET);
}
VMPU_STATUS VMPU_wake()
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_PWRMGMT1);
   regBuf &= ~(0b1 << VMPU_offs_PWRMGMT1_sleep);
   return MPUwriteByteTo(&regBuf,VMPU_reg_PWRMGMT1);
}
VMPU_STATUS VMPU_setDHPF(VMPUaccelDHPF dhpf)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_ACCFG);
   regBuf &= ~(0b111 << VMPU_offs_ACCFG_DHPF);
   regBuf |= ((dhpf & 0b111) << VMPU_offs_ACCFG_DHPF);
   return MPUwriteByteTo(&regBuf,VMPU_reg_ACCFG);
}
VMPU_STATUS VMPU_setIntPinCFG(VMPUintPinCfg intPinCfg)
{
   uint8_t regBuf = intPinCfg;
   return MPUwriteByteTo(&regBuf,VMPU_reg_INT_PIN_CFG);
}
VMPU_STATUS VMPU_setAccPwrOnDelay(VMPUaccelPwrOnDelay AccPwrOnDel)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_MOT_DECT_CFG);
   regBuf &= ~(0b11 << VMPU_offs_MOT_DECT_CFG_APOD);
   regBuf |= ((AccPwrOnDel & 0b11) << VMPU_offs_MOT_DECT_CFG_APOD);
   return MPUwriteByteTo(&regBuf,VMPU_reg_MOT_DECT_CFG);
}
VMPU_STATUS VMPU_setFF_thres(uint8_t thres)
{
   uint8_t regBuf = thres;
   return MPUwriteByteTo(&regBuf,VMPU_reg_FF_THR);
}
VMPU_STATUS VMPU_setFF_dur(uint8_t dur)
{
   uint8_t regBuf = dur;
   return MPUwriteByteTo(&regBuf,VMPU_reg_FF_DUR);
}
VMPU_STATUS VMPU_setFF_dec(VMPUcountDecrement dec)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_MOT_DECT_CFG);
   regBuf &= ~(0b11 << VMPU_offs_MOT_DECT_CFG_FFDEC);
   regBuf |= ((dec & 0b11) << VMPU_offs_MOT_DECT_CFG_FFDEC);
   return MPUwriteByteTo(&regBuf,VMPU_reg_MOT_DECT_CFG);
}
VMPU_STATUS VMPU_setMot_thres(uint8_t thres)
{
   uint8_t regBuf = thres;
   return MPUwriteByteTo(&regBuf,VMPU_reg_MOT_THR);
}
VMPU_STATUS VMPU_setMot_dur(uint8_t dur)
{
   uint8_t regBuf = dur;
   return MPUwriteByteTo(&regBuf,VMPU_reg_MOT_DUR);
}
VMPU_STATUS VMPU_setMot_dec(VMPUcountDecrement dec)
{
   uint8_t regBuf = MPUreadFromAddress(VMPU_reg_MOT_DECT_CFG);
   regBuf &= ~(0b11 << VMPU_offs_MOT_DECT_CFG_MOTDEC);
   regBuf |= ((dec & 0b11) << VMPU_offs_MOT_DECT_CFG_MOTDEC);
   return MPUwriteByteTo(&regBuf,VMPU_reg_MOT_DECT_CFG);
}
#pragma endregion
