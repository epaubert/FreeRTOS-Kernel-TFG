@
@ Sistemas Empotrados
@ Gestión de excepciones del MC1322x
@

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@
@ Máscaras para acceder a los bits de control de los registros de estado
@
	.set _IRQ_DISABLE, 0x80 @ cuando el bit I está activo, IRQ está deshabilitado
	.set _FIQ_DISABLE, 0x40 @ cuando el bit F está activo, FIQ está deshabilitado
	.set	_SVC_MODE, 0x13
	.set	_MSK_MODE, 0x1F

@
@ De momento vamos a definir los manejadores en la sección .text, aunque se
@ puede poner otra sección que esté mapeada a una memoria más rápida,
@ previamente definida en el linker script (ej. fastcode)
@
@ Los manejadores deben estar escritos en código de 32 bits
@
	.code	32
	.section .text, "xa"

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@
@ Manejador en ensamblador para interrupciones normales no anidadas
@
	.align	4
	.globl	excep_nonnested_irq_handler_asm
	.type	excep_nonnested_irq_handler_asm, %function

excep_nonnested_irq_handler_asm:

	@ Entrada del manejador
	sub		lr, lr, #4					@ Ajustamos el registro de enlace
	stmfd	sp!, {a1-a4, fp, ip, lr}	@ Salvamos los registros corruptibles (según AAPCS)

	@ Servimos la interrupción
	ldr	ip, =itc_service_normal_interrupt
	mov	lr, pc
	bx	ip

	@ Salida del manejador
	ldmfd	sp!, {a1-a4, fp, ip, pc}^	@ Recuperamos los registros y retornamos

	.size	excep_nonnested_irq_handler_asm, .-excep_nonnested_irq_handler_asm

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ Manejador en ensamblador para interrupciones rápidas no anidadas
@
	.align	4
	.globl	excep_nonnested_fiq_handler_asm
	.type	excep_nonnested_fiq_handler_asm, %function

excep_nonnested_fiq_handler_asm:

	@ Entrada del manejador
	# sub		lr, lr, #4						@ Ajustamos el registro de enlace
	# add		lr, lr, #4						@ Ajustamos el registro de enlace
	stmfd	sp!, {a1-a4, lr}				@ Salvamos los registros corruptibles (según AAPCS)
	
	@ Servimos la interrupción
	ldr		ip, =itc_service_fast_interrupt
	mov		lr, pc
	bx		ip

	@ Salida del manejador
	ldmfd	sp!, {a1-a4, pc}^				@ Recuperamos los registros y retornamos

	.size	excep_nonnested_fiq_handler_asm, .-excep_nonnested_fiq_handler_asm

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ Constantes para facilitar la gestión del marco de pila
@
	.set	_FRAME_A1,	0
	.set	_FRAME_A2,	_FRAME_A1 + 4
	.set	_FRAME_A3,	_FRAME_A2 + 4
	.set	_FRAME_A4,	_FRAME_A3 + 4
	.set	_FRAME_V1,	_FRAME_A4 + 4
	.set	_FRAME_V2,	_FRAME_V1 + 4
	.set	_FRAME_V3,	_FRAME_V2 + 4
	.set	_FRAME_V4,	_FRAME_V3 + 4
	.set	_FRAME_V5,	_FRAME_V4 + 4
	.set	_FRAME_V6,	_FRAME_V5 + 4
	.set	_FRAME_V7,	_FRAME_V6 + 4
	.set	_FRAME_FP,	_FRAME_V7 + 4
	.set	_FRAME_IP,	_FRAME_FP + 4
	.set	_FRAME_PSR,	_FRAME_IP + 4
	.set	_FRAME_LR,	_FRAME_PSR + 4
	.set	_FRAME_PC,	_FRAME_LR + 4
	.set	_FRAME_SIZE,_FRAME_PC + 4

@
@ Manejador en ensamblador para interrupciones normales anidadas
@
	.align	4
	.globl	excep_nested_irq_handler
	.type	excep_nested_irq_handler, %function

excep_nested_irq_handler:

	@ Entrada del manejador
	sub		lr, lr, #4					@ Ajustamos el registro de enlace
	stmfd	sp!, {a1-a4, fp, ip, lr}	@ Salvamos los registros corruptibles (según AAPCS)

	@ Salvamos spsr_irq y sp_irq y vaciamos la pila del modo IRQ
	mrs		a1, spsr					@ a1 <- spsr_irq
	mov		a2, sp						@ a2 <- sp_irq
	add		sp, sp, #7*4				@ Sacamos 7 entradas, ya que más arriba se han metido 7 registros

	@ Cambiamos al modo SVC
	mrs		a3, cpsr
	bic		a3, a3, #_MSK_MODE
	orr		a3, a3, #_SVC_MODE
	msr		cpsr_c, a3

	@ Reservamos parte del marco de pila en la pila SVC (v1-v7,fp,ip,psr,lr,pc)
	sub		sp, sp, #_FRAME_SIZE-_FRAME_V1
	stmia	sp,{v1-v7}					@ Almacenamos v1-v7 en el marco de pila
	ldmia	a2,{v1-v7}					@ Copiamos a1-a4, fp, ip, lr de la pila IRQ en v1-v7

	@ Terminamos de crear el marco de pila
	stmfd	sp!,{v1-v4}					@ Almacenamos a1-a4 en el marco de pila

	@ Terminamos de rellenar el marco de pila
	str		v5,[sp, #_FRAME_FP]
	str		v6,[sp, #_FRAME_IP]
	str		v7,[sp, #_FRAME_PC]
	str		a1,[sp, #_FRAME_PSR]
	str		lr,[sp, #_FRAME_LR]

	@ Servimos la interrupción, la ISR podrá volver a habilitar las IRQ una vez reconocida la interrupción
	ldr	ip, =itc_service_normal_interrupt
	mov	lr, pc
	bx	ip

	@ Sección crítica para restaurar el contexto
	msr	cpsr_c, #(_SVC_MODE | _IRQ_DISABLE | _FIQ_DISABLE)

	@ Salida del manejador
	ldmfd	sp!, {a1-a4,v1-v7,fp,ip,lr}	@ Recuperamos los registros a1-a4,v1-v7, spsr
	msr		spsr_cxsf, lr				@ Restauramos spsr
	ldmfd	sp!,{lr,pc}^				@ Retornamos

	.size	excep_nested_irq_handler, .-excep_nested_irq_handler

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

