#ifndef VMCB_MPU
#define VMCB_MPU
#ifdef __cplusplus
extern "C"{
#endif



#include "stm32f0xx_hal.h"
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

typedef enum VMPU_STATUS{VMPU_OK,VMPU_ERR}VMPU_STATUS;


#define VMPU_ADDRESS_AD0
#ifdef VMPU_ADDRESS_AD0 
    #define VMPU_ADR (0x68 << 1)
#else 
    #define VMPU_ADR (0x69 << 1)
#endif

//registers of Interest and correspunding datasets
#define VMPU_reg_GYCFG 0x1B
    #define VMPU_offs_GYCFG_scale 3
    typedef enum VMPUscale {
        VMPUscale_250,
        VMPUscale_500,
        VMPUscale_1000,
        VMPUscale_2000
    }VMPUscale;
#define VMPU_reg_ACCFG 0x1C
    #define VMPU_offs_ACCFG_range 3
    typedef enum VMPUrange {
        VMPUrange_2g,
        VMPUrange_4g,
        VMPUrange_8g,
        VMPUrange_16g
    }VMPUrange;
    #define VMPU_offs_ACCFG_DHPF 0
    typedef enum VMPUaccelDHPF {
        VMPUaccDHPF_RESET,
        VMPUaccDHPF_5Hz,
        VMPUaccDHPF_2Hz5,
        VMPUaccDHPF_1Hz25,
        VMPUaccDHPF_0Hz63,
        VMPUaccDHPF_HOLD
    } VMPUaccelDHPF;


#define VMPU_reg_FF_THR 0x1D
#define VMPU_reg_FF_DUR 0x1E
#define VMPU_reg_MOT_THR 0x1F
#define VMPU_reg_MOT_DUR 0x20

#define VMPU_reg_INT_PIN_CFG 0x37
    typedef enum VMPUintPinCfg { 
        VMPUintPinCfg_rdClear = 0x10,
        VMPUintPinCfg_latch = 0x20,
        VMPUintPinCfg_openD = 0x40,
        VMPUintPinCfg_activeH = 0x80,
    }VMPUintPinCfg;


#define VMPU_reg_INT_ENABLE 0x38
typedef enum INTenable {
    VMPUINT_DATA_RDY=0x01,
    VMPUINT_DMP = 0x02,
    VMPUINT_PLL_RDY = 0x04,
    VMPUINT_I2C_MST = 0x08,
    VMPUINT_FIFO_OVERF = 0x10,
    VMPUINT_ZMOT = 0x20,
    VMPUINT_MOT = 0x40,
    VMPUINT_FF = 0x80
}INTenable;

#define VMPU_reg_SIG_PATH_RESET 0x68
#define VMPU_offs_SIG_PATH_RESET 0
    typedef enum VMPUresetSigPath{
        VMPUresetSigPath_Temp = 0x01,
        VMPUresetSigPath_Accel = 0x02,
        VMPUresetSigPath_Gyro = 0x04,
    }VMPUresetSigPath;

#define VMPU_reg_MOT_DECT_CFG 0x69
    #define VMPU_offs_MOT_DECT_CFG_APOD 4
    #define VMPU_offs_MOT_DECT_CFG_FFDEC 2
    #define VMPU_offs_MOT_DECT_CFG_MOTDEC 0
    typedef enum VMPUaccelPwrOnDelay {
        VMPUaccelPwrOnDelay_0ms,
        VMPUaccelPwrOnDelay_1ms,
        VMPUaccelPwrOnDelay_2ms,
        VMPUaccelPwrOnDelay_3ms
    }VMPUaccelPwrOnDelay;
    typedef enum VMPUcountDecrement {
        VMPUcountDec_FULL,
        VMPUcountDec_1,
        VMPUcountDec_2,
        VMPUcountDec_3
    }VMPUcountDecrement;

#define VMPU_reg_PWRMGMT1 0x6B
    #define VMPU_offs_PWRMGMT1_reset 7
    #define VMPU_offs_PWRMGMT1_sleep 6
    #define VMPU_offs_PWRMGMT1_clk 0
    typedef enum VMPUclk {
        VMPUclk_INTERNAL = 0x00,
        VMPUclk_PLLx,
        VMPUclk_PLLy,
        VMPUclk_PLLz,
        VMPUclk_PLLext32,
        VMPUclk_PLLext19,
        VMPUclk_RESERVED,
        VMPUclk_STOP
    }VMPUclk;


VMPU_STATUS VMPU_init(VMPUclk clkSrc, VMPUscale scal, VMPUrange range);

VMPU_STATUS VMPU_initINT(VMPUaccelDHPF dhpf, VMPUintPinCfg intPin, VMPUaccelPwrOnDelay AccPwrOnDel);

VMPU_STATUS VMPU_initINT_FF(uint8_t thres, uint8_t dur, VMPUcountDecrement dec);

VMPU_STATUS VMPU_initINT_MOT(uint8_t thres, uint8_t dur, VMPUcountDecrement dec);

VMPU_STATUS VMPUsetIntEnable(uint8_t interruptMask);

#ifdef __cplusplus
}
#endif
#endif