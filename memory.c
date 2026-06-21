#include "memory.h"
#include "6502.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

uint8_t mem_read(uint8_t *memory, uint16_t addr){
    return memory[addr];
}

void load_program(uint8_t *memory,const uint8_t *rom){
    const uint8_t *prg = rom + 0x10;

    memcpy(&memory[0x8000], prg, 0x4000);
    memcpy(&memory[0xC000], prg, 0x4000);
}

uint16_t immediate(uint8_t *memory,uint16_t addr){
    return memory[addr];
}
uint16_t zeropage(uint8_t *memory,uint16_t addr){
    uint8_t zp_addr = memory[addr];
    return memory[zp_addr];
}
uint16_t zeropageXindex(uint8_t *memory,uint16_t addr, uint8_t x){
    uint8_t zp_addr = memory[addr];
    return memory[(zp_addr + x) & 0xFF];
}
uint16_t zeropageYindex(uint8_t *memory,uint16_t addr, uint8_t y){
    uint8_t zp_addr = memory[addr];
    return memory[(zp_addr + y) & 0xFF];
}
uint16_t indirectXindex(uint8_t *memory,uint16_t addr, uint8_t x){
    uint8_t zp_ptr = (memory[addr] + x) & 0xFF;

    uint8_t low  = memory[zp_ptr];
    uint8_t high = memory[(zp_ptr + 1) & 0xFF];

    uint16_t target = ((uint16_t)high << 8) | low;

    return memory[target];
}
uint16_t indirectYindex(uint8_t *memory,uint16_t addr, uint8_t y){
    uint8_t zp_ptr = memory[addr];

    uint8_t low  = memory[zp_ptr];
    uint8_t high = memory[(zp_ptr + 1) & 0xFF];

    uint16_t target = (((uint16_t)high << 8) | low) + y;

    return memory[target];
}
uint16_t absolute(uint8_t *memory,uint16_t addr){
    uint8_t low = memory[addr];
    uint8_t high = memory[addr+1];
    uint16_t target = (((uint16_t)high << 8) | low);

    return memory[target]; 
}
uint16_t absoluteX(uint8_t *memory,uint16_t addr, uint8_t x){
    uint8_t low = memory[addr];
    uint8_t high = memory[addr+1];
    uint16_t target = (((uint16_t)high << 8) | low) + x;
    return memory[target]; 
}
uint16_t absoluteY(uint8_t *memory,uint16_t addr, uint8_t y){
    uint8_t low = memory[addr];
    uint8_t high = memory[addr+1];
    uint16_t target = (((uint16_t)high << 8) | low) + y;
    return memory[target]; 
}