#
# Makefile publico para la Redwire EconoTAG  (común para BSP y aplicaciones)
#

#
# Información sobre la biblioteca proporcionada por el BSP
#

# Nombre de la biblioteca
BSP            = bsp

# Nombre del archivo biblioteca que proporciona el BSP
BSP_LIB        = lib$(BSP).a

#
# Paths
#

# Path al script de enlazado
BSP_LINKER_SCRIPT = $(BSP_ROOT_DIR)/econotag.ld

# Ruta a la raiz de todas las cabeceras que el BSP proporciona a la aplicación.
# Las siguientes rutas se añaden a la lista de cabeceras que la aplicación o
# cualquier componente del BSP usen.
BSP_INCLUDE_DIRS = $(sort $(dir $(shell find $(BSP_ROOT_DIR) -name '*.h' -print)))


# Añadimos los directorios a las flags
BSP_CFLAGS     = $(addprefix -I, $(BSP_INCLUDE_DIRS))
BSP_ASFLAGS    = $(addprefix -I, $(BSP_INCLUDE_DIRS))

# Añadimos la biblioteca generada por el BSP a la lista de bibliotecas
BSP_LDFLAGS    = -T$(BSP_LINKER_SCRIPT) -L$(BSP_ROOT_DIR) #--print-map
BSP_LIBS       = -l$(BSP)

# Añadimos las bibliotecas libc y libm de newlib
BSP_LDFLAGS    += -L$(subst /libc.a,,$(shell echo `$(CC) --print-file-name=libc.a`))
BSP_LIBS       += -lc -lm

# Añadimos libgcc a la lista de bibliotecas
BSP_LDFLAGS    += -L$(subst /libgcc.a,,$(shell echo `$(CC) --print-file-name=libgcc.a`))
BSP_LIBS       += -lgcc

# Como la implementación de las llamadas al sistema está en el BSP, es necesario
# añadir -l$(BSP) tras -lc
BSP_LIBS       += -l$(BSP)

