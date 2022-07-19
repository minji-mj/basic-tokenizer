################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Test.cpp \
../src/TreeNode.cpp \
../src/makeDirectoryTree.cpp 

OBJS += \
./src/Test.o \
./src/TreeNode.o \
./src/makeDirectoryTree.o 

CPP_DEPS += \
./src/Test.d \
./src/TreeNode.d \
./src/makeDirectoryTree.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -DUSE_OSS -I"/usr/include" -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


