#ifndef CPU_6502_H
#define CPU_6502_H

#include <stdint.h>

typedef struct {
    uint8_t A;
    uint8_t X;
    uint8_t Y;
    uint16_t pc;
    uint8_t S;
    uint8_t P;
}cpu_6502;

void power_up(cpu_6502 *cpu);
void reset(cpu_6502 *cpu, uint8_t *memory);
void step(cpu_6502 *cpu, uint8_t *memory);

#endif
