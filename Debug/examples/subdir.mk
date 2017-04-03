################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../examples/define_test1.cpp \
../examples/normal_test1.cpp \
../examples/preprocessors.cpp \
../examples/transferAll.cpp 

OBJS += \
./examples/define_test1.o \
./examples/normal_test1.o \
./examples/preprocessors.o \
./examples/transferAll.o 

CPP_DEPS += \
./examples/define_test1.d \
./examples/normal_test1.d \
./examples/preprocessors.d \
./examples/transferAll.d 


# Each subdirectory must supply rules for building sources it contributes
examples/%.o: ../examples/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -DCODE64 -I"D:\Pool\eclipse-workspace\compiler-debug\include" -I"C:\Users\13774\Desktop\bochs\devel\x2 system\include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


