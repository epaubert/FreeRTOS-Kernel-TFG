
#ifndef __TMR_H__
#define __TMR_H__

#include <stdint.h>
typedef enum {
	no_error_timer,
	error_timer,
	timer_err_MAX,
} timer_err_t;

typedef enum {
	timer_0 = 0,
	timer_1,
	timer_2,
	timer_3,
	timer_MAX,

} timer_id_t;

void TimerInt_Init(timer_id_t tmr, int hz);

void clearInt(timer_id_t tmr);

timer_id_t getIntTmr(void);

#endif /* __TMR_H__ */

