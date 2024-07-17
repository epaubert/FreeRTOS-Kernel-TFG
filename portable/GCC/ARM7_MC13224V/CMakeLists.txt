cmake_minimum_required(VERSION 3.27.7)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(BIN "/run/current-system/sw/bin/")
set(SHELL "${BIN}sh")
set(BSP_DIR "bsp/")
set(DEMO_DIR "../../../../Demo/ARM7_MC13224V_GCC/")

set(LINKER_SCRIPT "${BSP_DIR}econotag.ld")

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_SYSTEM_PROCESSOR "arm7tdmi")

set(CROSS_COMPILE "${BIN}arm-none-eabi-")
set(CMAKE_C_COMPILER "${CROSS_COMPILE}gcc")
set(CMAKE_CXX_COMPILER "${CROSS_COMPILE}g++")
set(CMAKE_C_LINKER "${CROSS_COMPILE}ld")
set(CMAKE_C_ASSEMBLER "${CROSS_COMPILE}as") 
set(CMAKE_AR "${CROSS_COMPILE}ar")

set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")

# Flags
set(CFLAGS "-c -g -Og -Wall -mcpu=arm7tdmi") # -g -mthumb -mthumb-interwork 
set(ASFLAGS "-gstabs -mcpu=arm7tdmi -mfpu=softfpa") # -mthumb -mthumb-interwork
set(ARFLAGS "-src")
set(LDFLAGS -T${LINKER_SCRIPT})

project(freertos_kernel_port C ASM)

# BSP
add_subdirectory("${BSP_DIR}")

# PORT
add_library(freertos_kernel_port OBJECT)
target_sources(freertos_kernel_port
       PRIVATE
         port.c
         portISR.c
         # portmacro.h
)
add_library(freertos_kernel_port_headers INTERFACE)
target_include_directories(freertos_kernel_port_headers INTERFACE
        .
        ../../../include/
        ${BSP_DIR}/include/
        ${DEMO_DIR}
)
target_link_libraries(freertos_kernel_port
       PRIVATE
         bsp
         freertos_kernel_port_headers
         freertos_kernel_include 
)
target_compile_options(freertos_kernel_port PRIVATE
    -g
    -Og
    -Wall
    -mcpu=arm7tdmi
    # -mthumb
    # -mthumb-interwork 
)
