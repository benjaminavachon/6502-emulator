#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

uint8_t mem_read(uint8_t *memory, uint16_t addr);
void mem_write(uint8_t *memory, uint16_t addr, uint8_t value);

#endif