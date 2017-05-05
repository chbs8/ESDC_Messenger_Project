################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/dispatch.c \
../src/main.c \
../src/txperf.c \
../src/urxperf.c \
../src/utxperf.c 

OBJS += \
./src/dispatch.o \
./src/main.o \
./src/txperf.o \
./src/urxperf.o \
./src/utxperf.o 

C_DEPS += \
./src/dispatch.d \
./src/main.d \
./src/txperf.d \
./src/urxperf.d \
./src/utxperf.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -I../../socket_client_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


