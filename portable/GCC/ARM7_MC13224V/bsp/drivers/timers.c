// MC1322xRM.pdf
// Base address for the TMR Module is 0x8000_7000
// All registers are 16 bits wide with 16-bit access only.
// Note register address offset is 0x2, not 0x4.

#include "system.h"
typedef struct {
  // Registro de comparación utilizando cuando cuenta hacia arriba
  uint16_t COMP1 : 16; // 0x00 | TMRX_MASK
  // Registro de comparación utilizando cuando cuenta hacia abajo
  uint16_t COMP2 : 16; // 0x02 | TMRX_MASK
  // Registro para guardar el valor actual del contador
  // 0x80007004
  uint16_t CAPT : 16; // 0x04 | TMRX_MASK
  // Registro para inicializar el contador
  uint16_t LOAD : 16; // 0x06 | TMRX_MASK
  // Registro que guarda el valor de este contador mientras otros
  // contadores se están leyendo
  uint16_t HOLD : 16; // 0x08 | TMRX_MASK
  // Registro que cuenta enventos internos y externos
  uint16_t CNTR : 16; // 0x0A | TMRX_MASK

  // Registros de control, Tabla 12-11 en MC1322xRM.pdf
  union { // CTRL : 0x0C | TMRX_MASK
    struct {
      uint16_t OUTPUT_MODE : 3;
      uint16_t CO_INIT : 1;
      uint16_t DIR : 1;
      uint16_t LENGHT : 1;
      uint16_t ONCE : 1;
      uint16_t SECONDARY_CNT_SRC : 2;
      uint16_t PRIMARY_CNT_SRC : 4;
      uint16_t COUNT_MODE : 3;
    };
    uint16_t CTRL : 16;
  };

  // Registros de estado y control, Tabla 12-15 en MC1322xRM.pdf
  union { // SCTRL : 0x0E | TMRX_MASK
    struct {
      uint16_t OEN : 1;
      uint16_t OPS : 1;
      uint16_t FORCE : 1;
      uint16_t VAL : 1;
      uint16_t EEOF : 1;
      uint16_t MSTR : 1;
      uint16_t CAPTURE_MODE : 2;
      uint16_t INPUT : 1;
      uint16_t IPS : 1;
      uint16_t IEFIE : 1;
      uint16_t IEF : 1;
      uint16_t TOFIE : 1;
      uint16_t TOF : 1;
      uint16_t TCFIE : 1;
      uint16_t TCF : 1;
    };
    uint16_t SCTRL : 16;
  };

  // Registros de carga del comparador, para
  // cargar en el comparador si se cumple una condición
  uint16_t CMPLD1 : 16; // 0x10 | TRMX_MASK
  uint16_t CMPLD2 : 16; // 0x12 | TRMX_MASK

  // Registros de estado y control de los comparadores
  union { // CSCTRL : 0x14 | TRMX_MASK
    struct {
      uint16_t CL1          : 2;
      uint16_t CL2          : 2;
      uint16_t TCF1         : 1;
      uint16_t TCF2         : 1;
      uint16_t TCF1_EN      : 1;
      uint16_t TCF2_EN      : 1;
      uint16_t /*RESERVED*/ : 5;
      uint16_t FILT_EN      : 1;
      uint16_t DBG_EN       : 2;
    };
    uint16_t CSCTRL         : 16;
  };
  /*PADDING 0x16 -> 0x1e */
  uint16_t                  : 8;
  // Regisro de Habilitación del canales
  union { // ENBL : 0x1e
    struct {
      uint16_t ENBL0        : 1;
      uint16_t ENBL1        : 1;
      uint16_t ENBL2        : 1;
      uint16_t ENBL3        : 1;
      uint16_t /*RESERVED*/ : 12;
    };
    uint16_t ENBL           : 16;
  };

} tmr_regs_t;

static volatile tmr_regs_t *const tmr_regs = TMR_BASE;

// Single-Edge Count Mode
// This example generates an interrupt every 100ms (10Hz),
// assuming the Peripheral Clock is operating at 24 MHz.
//
// It does this by using the Peripheral Clock divided by 128 as the counter
// clock source. The counter then counts to 18750 (dec) where it matches the
// COMP1 value. At that time an interrupt is generated, the counter is reloaded
// and the next COMP1 value is loaded from CMPLD1.
void TimerInt_Init(timer_id_t tmr, int hz){
  /*
   * En esta función utilizamos el reloj principal dividido por 128
   * 24000000 Hz / 128 = 187500 Hz
   * Si queremos una interrupción cada 100ms:
   * 1/100ms = 10 Hz
   * 187500 Hz / 10 Hz = 18750
   * */
  const uint32_t COUNT_TO = 187500 / hz;

  tmr_regs[tmr].CTRL = 0x20;

  tmr_regs[tmr].SCTRL = 0x00;

  tmr_regs[tmr].LOAD = 0x00;

  tmr_regs[tmr].COMP1 = COUNT_TO;
  tmr_regs[tmr].CMPLD1 = COUNT_TO;
  tmr_regs[tmr].CSCTRL = 0x41;

  tmr_regs[tmr].PRIMARY_CNT_SRC = 0xF;
  tmr_regs[tmr].CNTR = 0x00;
  tmr_regs[tmr].COUNT_MODE = 0x01;

  clearInt(tmr);
}

void clearInt(timer_id_t tmr){
  tmr_regs[tmr].TCF = 0;
  tmr_regs[tmr].TCF1 = 0;
  tmr_regs[tmr].TCF2 = 0; 
}

timer_id_t getIntTmr(void) {
  timer_id_t out = timer_MAX;

  if(tmr_regs[timer_0].TCF == 1) {
    out = timer_0; 
  }
  if(tmr_regs[timer_1].TCF == 1) {
    out = timer_1; 
  }
  if(tmr_regs[timer_2].TCF == 1) {
    out = timer_2; 
  }
  if(tmr_regs[timer_3].TCF == 1) {
    out = timer_3; 
  }

  return out;
}
