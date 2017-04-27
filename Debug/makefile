################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include test/subdir.mk
-include src/subdir.mk
-include gen/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: compiler-debug.exe

dependents:
	-cd D:/Pool/eclipse-workspace/leetcode/Debug && $(MAKE) clean all

# Tool invocations
compiler-debug.exe: $(OBJS) $(USER_OBJS) D:/Pool/eclipse-workspace/leetcode/Debug/leetcode.exe
	@echo 'Building target: $@'
	@echo 'Invoking: Cygwin C++ Linker'
	g++  -o "compiler-debug.exe" $(OBJS) $(USER_OBJS) $(LIBS) ../../leetcode/Debug/include/StringUtils.o
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(CPP_DEPS)$(C_DEPS) compiler-debug.exe
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
D:/Pool/eclipse-workspace/leetcode/Debug/leetcode.exe:

-include ../makefile.targets
