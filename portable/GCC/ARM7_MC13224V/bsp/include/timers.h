
#ifndef __TMR_H__
#define __TMR_H__

// #include <stdint.h>

#define TMR_OFFSET      (0x20)
#define TMR_BASE        (0x80007000)
#define TMR0_BASE       (TMR_BASE)
#define TMR1_BASE       (TMR_BASE + TMR_OFFSET*1)
#define TMR2_BASE       (TMR_BASE + TMR_OFFSET*2)
#define TMR3_BASE       (TMR_BASE + TMR_OFFSET*3)

#define TMR0_PIN GPIO_08
#define TMR1_PIN GPIO_09
#define TMR2_PIN GPIO_10
#define TMR3_PIN GPIO_11

typedef enum {
	timer_0 = 0,
	timer_1,
	timer_2,
	timer_3,
	timer_MAX,
} timer_id_t;

#define GET_TMR(tmr) ((volatile struct tmr_regs_t *) (TMR_BASE + (timer_id_t)(tmr)*TMR_OFFSET))

void timer_init();

// void TimerInt_Init(timer_id_t tmr, int hz);

uint32_t timer_setup_irq(timer_id_t timer_id, uint32_t hz);

void clearInt(timer_id_t tmr);

timer_id_t getIntTmr(void);

void timerEnable(timer_id_t tmr);

void timerDisable(timer_id_t tmr);

#endif /* __TMR_H__ */

