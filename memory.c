#include "memory.h"
#include <stdint.h>

uint8_t mem_read(uint8_t *memory, uint16_t addr){
    return memory[addr];
}