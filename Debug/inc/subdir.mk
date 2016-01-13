################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../inc/iclust.c \
../inc/igraph_custom.c \
../inc/logger.c 

OBJS += \
./inc/iclust.o \
./inc/igraph_custom.o \
./inc/logger.o 

C_DEPS += \
./inc/iclust.d \
./inc/igraph_custom.d \
./inc/logger.d 


# Each subdirectory must supply rules for building sources it contributes
inc/%.o: ../inc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/igraph -I"/home/sensey/Projects/WurstNewman/vendor/cini/include" -I"/home/sensey/Projects/WurstNewman/inc" -I"/home/sensey/Projects/WurstNewman/vendor/zlog/include" -O3 -g3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


