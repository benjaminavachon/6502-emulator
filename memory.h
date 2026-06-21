#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include "6502.h"

uint8_t mem_read(uint8_t *memory, uint16_t addr);
void load_program(uint8_t *memory,const uint8_t *program,cpu_6502 *cpu, uint16_t start_addr, uint16_t size);

uint16_t immediate(uint8_t *memory,uint16_t addr);
uint16_t zeropage(uint8_t *memory,uint16_t addr);
uint16_t zeropageXindex(uint8_t *memory,uint16_t addr, uint8_t x);
uint16_t zeropageYindex(uint8_t *memory,uint16_t addr, uint8_t y);
uint16_t indirectXindex(uint8_t *memory,uint16_t addr, uint8_t x);
uint16_t indirectYindex(uint8_t *memory,uint16_t addr, uint8_t y);
uint16_t absolute(uint8_t *memory,uint16_t addr);
uint16_t absoluteX(uint8_t *memory,uint16_t addr, uint8_t x);
uint16_t absoluteY(uint8_t *memory,uint16_t addr, uint8_t y);




#endif