SET PATH=d:\Keil_v5\ARM\ARMCC\bin\;d:\Program Files (x86)\STMicroelectronics\st_toolset\asm;C:\Program Files (x86)\STMicroelectronics\st_toolset\asm;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files (x86)\Intel\iCLS Client\;C:\Program Files\Intel\iCLS Client\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files\Intel\Intel(R) Management Engine Components\IPT;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\IPT;D:\Program Files (x86)\Altium Designer Summer 09\System;C:\Program Files (x86)\Microsoft SQL Server\80\Tools\Binn\;C:\Program Files (x86)\ATMEL Corporation\AT91-ISP v1.13\Library\;C:\Program Files (x86)\ATMEL Corporation\AT91-ISP v1.13\sam-ba 2.9;d:\Program Files (x86)\ATMEL Corporation\AT91-ISP v1.13\Library\;d:\Program Files (x86)\ATMEL Corporation\AT91-ISP v1.13\sam-ba 2.9;d:\Cadence\SPB_16.6\openaccess\bin\win32\opt;d:\Cadence\SPB_16.6\tools\capture;d:\Cadence\SPB_16.6\tools\pspice;d:\Cadence\SPB_16.6\tools\specctra\bin;d:\Cadence\SPB_16.6\tools\fet\bin;d:\Cadence\SPB_16.6\tools\libutil\bin;d:\Cadence\SPB_16.6\tools\bin;d:\Cadence\SPB_16.6\tools\pcb\bin;d:\Program Files (x86)\COSMIC\CXSTM8
SET CPU_TYPE=STM32F103RB
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=Target 1
SET CPU_CLOCK=0x00B71B00
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\main.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\test.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\gui.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\delay.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\system_stm32f10x.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\key.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\lcd.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\myiic.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\24cxx.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\touch.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\spi.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\core_cm3.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmAsm" --Via "..\obj\startup_stm32f10x_md._ia"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\misc.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\stm32f10x_gpio.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\stm32f10x_rcc.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\stm32f10x_spi.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via "..\obj\stm32f10x_usart.__i"
"d:\Keil_v5\ARM\ARMCC\bin\ArmLink" --Via "..\OBJ\TOUCH.lnp"
"d:\Keil_v5\ARM\ARMCC\bin\fromelf.exe" "..\OBJ\TOUCH.axf" --i32combined --output "..\OBJ\TOUCH.hex"
