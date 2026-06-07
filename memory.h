#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

uint8_t mem_read(uint8_t *memory, uint16_t addr);
void load_program(uint8_t *memory,const uint8_t *program, uint16_t start_addr, uint16_t size);

#endif