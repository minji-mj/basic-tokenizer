################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TestPart4.cpp \
../src/TreeNode.cpp \
../src/tokenClasses.cpp \
../src/tokenClassifier.cpp 

OBJS += \
./src/TestPart4.o \
./src/TreeNode.o \
./src/tokenClasses.o \
./src/tokenClassifier.o 

CPP_DEPS += \
./src/TestPart4.d \
./src/TreeNode.d \
./src/tokenClasses.d \
./src/tokenClassifier.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -I"/usr/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


