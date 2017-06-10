################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/client.c \
../src/dispatch.c \
../src/iZybo.c \
../src/main.c \
../src/oneSocket.c \
../src/server.c \
../src/server_pro.c \
../src/udp_izybo.c 

OBJS += \
./src/client.o \
./src/dispatch.o \
./src/iZybo.o \
./src/main.o \
./src/oneSocket.o \
./src/server.o \
./src/server_pro.o \
./src/udp_izybo.o 

C_DEPS += \
./src/client.d \
./src/dispatch.d \
./src/iZybo.d \
./src/main.d \
./src/oneSocket.d \
./src/server.d \
./src/server_pro.d \
./src/udp_izybo.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -I../../FinalProject_ESDC_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


