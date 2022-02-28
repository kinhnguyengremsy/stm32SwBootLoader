################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../myLibrary/Src/hostUartBootLoader.c \
../myLibrary/Src/mavlinkControl.c \
../myLibrary/Src/mavlinkMsgHandle.c \
../myLibrary/Src/mavlinkProtocol.c \
../myLibrary/Src/ringBuffer.c \
../myLibrary/Src/serialPort.c \
../myLibrary/Src/sotfEmulations.c \
../myLibrary/Src/storageFlash.c \
../myLibrary/Src/swBootLoader.c \
../myLibrary/Src/uartBootLoader.c \
../myLibrary/Src/uartCLI.c \
../myLibrary/Src/uartCallback.c 

OBJS += \
./myLibrary/Src/hostUartBootLoader.o \
./myLibrary/Src/mavlinkControl.o \
./myLibrary/Src/mavlinkMsgHandle.o \
./myLibrary/Src/mavlinkProtocol.o \
./myLibrary/Src/ringBuffer.o \
./myLibrary/Src/serialPort.o \
./myLibrary/Src/sotfEmulations.o \
./myLibrary/Src/storageFlash.o \
./myLibrary/Src/swBootLoader.o \
./myLibrary/Src/uartBootLoader.o \
./myLibrary/Src/uartCLI.o \
./myLibrary/Src/uartCallback.o 

C_DEPS += \
./myLibrary/Src/hostUartBootLoader.d \
./myLibrary/Src/mavlinkControl.d \
./myLibrary/Src/mavlinkMsgHandle.d \
./myLibrary/Src/mavlinkProtocol.d \
./myLibrary/Src/ringBuffer.d \
./myLibrary/Src/serialPort.d \
./myLibrary/Src/sotfEmulations.d \
./myLibrary/Src/storageFlash.d \
./myLibrary/Src/swBootLoader.d \
./myLibrary/Src/uartBootLoader.d \
./myLibrary/Src/uartCLI.d \
./myLibrary/Src/uartCallback.d 


# Each subdirectory must supply rules for building sources it contributes
myLibrary/Src/%.o: ../myLibrary/Src/%.c myLibrary/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"E:/2022-work/firmware/home/stm32/stm32SwBootLoader/mavlink_v2" -I"E:/2022-work/firmware/home/stm32/stm32SwBootLoader/mavlink_v2/ardupilotmega" -I"E:/2022-work/firmware/home/stm32/stm32SwBootLoader/mavlink_v2/common" -I"E:/2022-work/firmware/home/stm32/stm32SwBootLoader/myLibrary/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-myLibrary-2f-Src

clean-myLibrary-2f-Src:
	-$(RM) ./myLibrary/Src/hostUartBootLoader.d ./myLibrary/Src/hostUartBootLoader.o ./myLibrary/Src/mavlinkControl.d ./myLibrary/Src/mavlinkControl.o ./myLibrary/Src/mavlinkMsgHandle.d ./myLibrary/Src/mavlinkMsgHandle.o ./myLibrary/Src/mavlinkProtocol.d ./myLibrary/Src/mavlinkProtocol.o ./myLibrary/Src/ringBuffer.d ./myLibrary/Src/ringBuffer.o ./myLibrary/Src/serialPort.d ./myLibrary/Src/serialPort.o ./myLibrary/Src/sotfEmulations.d ./myLibrary/Src/sotfEmulations.o ./myLibrary/Src/storageFlash.d ./myLibrary/Src/storageFlash.o ./myLibrary/Src/swBootLoader.d ./myLibrary/Src/swBootLoader.o ./myLibrary/Src/uartBootLoader.d ./myLibrary/Src/uartBootLoader.o ./myLibrary/Src/uartCLI.d ./myLibrary/Src/uartCLI.o ./myLibrary/Src/uartCallback.d ./myLibrary/Src/uartCallback.o

.PHONY: clean-myLibrary-2f-Src

