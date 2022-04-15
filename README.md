# libVMPU
Library for FF and MOT Interrupts on MPU6050.

By now this small lib should be able to configure the Freefall- and Motion- Interrupt on the MPU6050.
In the future this might be expanded to handle the other interrupt-sources aswell.

At the moment this program is based on STM-HAL drivers.
Should you need that functionality with different drivers simply change the MPUwriteByteTo(...) and MPUreadFromAddress(...) and remove the includes/externs in the .h file to suit your application.

The main files are only included as an example.

I do not care how you use this, just note that the code in this repo does NOT come with any warranty whatsoever. Use at own risk.
