cmake_minimum_required(VERSION "3.27.7")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BIN "/run/current-system/sw/bin/")
set(SHELL "${BIN}sh")

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_SYSTEM_PROCESSOR "arm7tdmi")

set(CROSS_COMPILE "${BIN}arm-none-eabi-")

set(CMAKE_C_COMPILER "${CROSS_COMPILE}gcc")
set(CMAKE_CXX_COMPILER "${CROSS_COMPILE}g++")
set(CMAKE_C_LINKER "${CROSS_COMPILE}ld")
set(CMAKE_C_ASSEMBLER "${CROSS_COMPILE}as")
set(CMAKE_AR "${CROSS_COMPILE}ar")

# set(CC ${CMAKE_C_COMPILER})
# set(LD ${CMAKE_C_LINKER})
# set(AS ${CMAKE_C_ASSEMBLER})
# set(AR ${CMAKE_AR})

set(CMAKE_EXE_LINKER_FLAGS "-specs=nosys.specs" CACHE INTERNAL "")

# Flags
set(CFLAGS "-c -g -O2 -Wall -mcpu=arm7tdmi") # -mthumb -mthumb-interwork")
set(ASFLAGS "-gstabs -mcpu=arm7tdmi -mfpu=softfpa") # -mthumb")
set(ARFLAGS "-src")

project(bsp_econotag C ASM)

# set(BSP "bsp")
# set(BSP_LIB "lib${BSP}.a") set(BSP_LINKER_SCRIPT "${BSP_ROOT_DIR}/econotag.ld")
# set(BSP_INCLUDE_DIRS "$(sort $(dir $(shell find ${BSP_ROOT_DIR} -name '*.h' -print)))")
# set(BSP_CFLAGS "$(addprefix -I, ${BSP_INCLUDE_DIRS})")
# set(BSP_ASFLAGS "$(addprefix -I, ${BSP_INCLUDE_DIRS})")
# set(BSP_LDFLAGS "-T${BSP_LINKER_SCRIPT} -L${BSP_ROOT_DIR}")
# set(BSP_LIBS "-l${BSP}")
# set(BSP_LDFLAGS "-L$(subst /libc.a,,$(shell echo `${CC} --print-file-name=libc.a`))")
# set(BSP_LIBS "-lc -lm")
# set(BSP_LDFLAGS "-L$(subst /libgcc.a,,$(shell echo `${CC} --print-file-name=libgcc.a`))")
# set(BSP_LIBS "-lgcc")
# set(BSP_LIBS "-l${BSP}")
# set(BSP_ROOT_DIR ".")
# set(OBJ_DIR "BSP_ROOT_DIR/obj")
# set(DIST_DIR "BSP_ROOT_DIR/dist")

add_library(bsp
    util/circular_buffer.c
    hal/bsp_init.c
    hal/crt0.s
    hal/dev.c
    hal/excep.c
    hal/syscalls.c
    drivers/gpio.c
    drivers/itc.c
    drivers/uart.c
    drivers/timers.c
) 
set_target_properties(bsp PROPERTIES LINKER_LANGUAGE C)
target_include_directories( bsp PUBLIC "include")
target_compile_options(bsp PRIVATE
    -O2
    -g
    -Wall
    -mcpu=arm7tdmi
    # -mthumb
    # -mthumb-interwork
)
