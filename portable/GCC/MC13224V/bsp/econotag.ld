/*
 * Sistemas Empotrados
 * Linker script para la Redwire EconoTAG
 * Runtime de C para ser cargado por la BIOS de la placa
 */

/*
 * Punto de entrada
 */
ENTRY(_vector_table)

/*
 * Mapa de memoria de la placa
 */
MEMORY
{
        ram   : org = 0x00400000,       len = 0x00018000        /*  96 KB */
}

SECTIONS
{
	/* Imagen del firmware */
        .image :
        {
                *(.startup);
                *(.text);
                *(.rodata*);
                . = ALIGN(4) ;
                *(.data);
                . = ALIGN(4) ;
        } > ram

	/* Sección .bss */
        .bss :
        {
                _bss_start = . ;
                *(.bss);
                . = ALIGN(4) ;
                *(COMMON);
                . = ALIGN(4) ;
                _bss_end = . ;
        } > ram

        /* Gestión de las pilas */
        _ram_limit = ORIGIN(ram) + LENGTH(ram);
        _sys_stack_size = 1024 ;
        _irq_stack_size =  256 ;
        _fiq_stack_size =  256 ;
        _svc_stack_size =  256 ;
        _abt_stack_size =   16 ;
        _und_stack_size =   16 ;
        _stacks_size    = _stacks_top - _stacks_bottom ;
        .stacks _ram_limit - _stacks_size :
        {
                _stacks_bottom = . ;
                . += _sys_stack_size ;
                _sys_stack_top = . ;
                . += _svc_stack_size ;
                _svc_stack_top = . ;
                . += _abt_stack_size ;
                _abt_stack_top = . ;
                . += _und_stack_size ;
                _und_stack_top = . ;
                . += _irq_stack_size ;
                _irq_stack_top = . ;
                . += _fiq_stack_size ;
                _fiq_stack_top = . ;
                _stacks_top = . ;
        }

 	/* Gestión del heap */
        _heap_size = _stacks_bottom - _bss_end ;
        .heap _bss_end :
        {
                _heap_start = . ;
                . += _heap_size ;
                _heap_end = . ;                
        }
}
