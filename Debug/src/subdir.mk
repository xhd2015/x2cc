################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/FAUtils.cpp \
../src/GrammaUtils.cpp \
../src/GrammarTranslateUtils.cpp \
../src/LexicalParser.cpp \
../src/SemanticParser.cpp 

OBJS += \
./src/FAUtils.o \
./src/GrammaUtils.o \
./src/GrammarTranslateUtils.o \
./src/LexicalParser.o \
./src/SemanticParser.o 

CPP_DEPS += \
./src/FAUtils.d \
./src/GrammaUtils.d \
./src/GrammarTranslateUtils.d \
./src/LexicalParser.d \
./src/SemanticParser.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -std=c++14 -DCODE64 -I"D:\Pool\eclipse-workspace\compiler-debug\include" -I"C:\Users\13774\Desktop\bochs\devel\x2 system\include" -I"D:\Pool\eclipse-workspace\leetcode\include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


