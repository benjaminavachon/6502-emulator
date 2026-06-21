#include <stdio.h>
#include "6502.h"
#include "memory.h"

void power_up(cpu_6502 *cpu) {
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->pc = 0;
    cpu->S = 0xFD;
    cpu->P = 0x34;
}

void reset(cpu_6502 *cpu, uint8_t *memory) {
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->pc = memory[0xFFFC] | (memory[0xFFFD] << 8);
    cpu->S = 0xFD;
    cpu->P = 0x24;
}

void step(cpu_6502 *cpu,uint8_t* memory) {
    uint8_t opcode = mem_read(memory,cpu->pc++);
    uint8_t value;
    uint8_t low;
    uint8_t high;
    uint16_t target;
    uint16_t return_addr;
    switch(opcode){
        case 0x00:{
            cpu->P |= 0x10;
            cpu->P |= 0x04;
            break;
        }
        case 0x01:{
            cpu->A |= indirectXindex(memory, cpu->pc++, cpu->X);
            break;
        }
        case 0x05:{
            cpu->A |= zeropage(memory, cpu->pc++);
            break;
        }
        case 0x06:{
            uint8_t zp = zeropage(memory, cpu->pc++);
            uint8_t val = memory[zp];

            
            cpu->P = (val & 0x80) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            uint8_t result = val << 1;

            memory[zp] = result;

            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            

            break;
        }
        case 0x08:{
            cpu->S--;
            memory[0x0100 + cpu->S] = cpu->P;

            break;
        }
        case 0x09:{
            cpu->A |= immediate(memory, cpu->pc++);
            break;
        }
        case 0x0A:{
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            cpu->A <<= 1;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x0D:{
            cpu->A |= absolute(memory, cpu->pc++);
            break;
        }
        case 0x0E:{
            uint16_t addr = memory[cpu->pc + 1] | (memory[cpu->pc + 2] << 8);
            uint8_t val = memory[addr];

            cpu->P = (val & 0x80) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            uint8_t result = val << 1;

            memory[addr] = result;

            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 3;
        }
        case 0x10:{
            int8_t offset = (int8_t)memory[cpu->pc++];

            uint16_t old_pc = cpu->pc;

            if ((cpu->P & 0x80) == 0) {
                cpu->pc = old_pc + offset;
            }

            break;
        }
        case 0x11:{
            cpu->A |= indirectYindex(memory, cpu->pc++, cpu->Y);
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            break;
        }
        case 0x15:{
            cpu->A |= zeropageXindex(memory, cpu->pc++, cpu->X);
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            break;
        }
        case 0x16:{
            uint8_t zp = (memory[cpu->pc++] + cpu->X) & 0xFF;
            uint8_t val = memory[zp];

            
            cpu->P = (val & 0x80) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            uint8_t result = val << 1;

            memory[zp] = result;

            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            

            break;
        }
        case 0x18:{
            cpu->P &= ~0x01;
            break;
        }
        case 0x19:{
            cpu->A |= absoluteY(memory, cpu->pc+1, cpu->Y);
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->pc += 2;

            break;
        }
        case 0x1D:{
            cpu->A |= absoluteX(memory, cpu->pc+1, cpu->Y);
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->pc += 2;

            break;
        }
        case 0x1E:{
            uint16_t addr = (memory[cpu->pc + 1] | (memory[cpu->pc + 2] << 8)) + cpu->X;
            uint8_t val = memory[addr];

            cpu->P = (val & 0x80) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            uint8_t result = val << 1;

            memory[addr] = result;

            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 3;
            break;
        }
        case 0x20:{
            low = mem_read(memory, cpu->pc++);
            high = mem_read(memory, cpu->pc++);

            target = ((uint16_t)high << 8) | low;

            return_addr = cpu->pc - 1;

            cpu->S--;
            memory[0x0100 + cpu->S] = (return_addr >> 8) & 0xFF;
            
            cpu->S--;
            memory[0x0100 + cpu->S] = return_addr & 0xFF;
            

            cpu->pc = target;

            break;
        }
        case 0x21:{
            uint8_t zp = memory[cpu->pc++];
            uint8_t ptr = (zp + cpu->X) & 0xFF;

            uint16_t addr = memory[ptr] | (memory[(ptr + 1) & 0xFF] << 8);

            cpu->A &= memory[addr];

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x24:{
            uint8_t zp = zeropage(memory, cpu->pc++);
            (cpu->A & zp) == 0 ? (cpu->P |= 0x02) : (cpu->P &= ~0x02);
            
            cpu->P = (zp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (zp & 0x40) ? (cpu->P | 0x40) : (cpu->P & ~0x40);
            break;
        }
        case 0x25:{

            uint8_t zp = zeropage(memory, cpu->pc++);
            cpu->A &= zp;
            
            (cpu->A == 0) ? (cpu->P |= 0x02) : (cpu->P &= ~0x02);
            cpu->P = (zp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x26:{
            
            uint8_t addr = memory[cpu->pc];
            uint8_t zp = memory[addr];

            uint8_t old = cpu->P & 0x01;

            cpu->P = (zp & 0x80) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            uint8_t result = (zp << 1) | old;

            memory[addr] = result;

            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            cpu->pc += 1;

            break;
        }
        case 0x28:{
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x29:{
            uint8_t val = immediate(memory,cpu->pc++);
            cpu->A &= val;
            
            (cpu->A == 0) ? (cpu->P |= 0x02) : (cpu->P &= ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x2A:{
            uint8_t zp = cpu->A;

            uint8_t old = cpu->P & 0x01;

            cpu->P = (zp & 0x80) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            uint8_t result = (zp << 1) | old;

            cpu->A = result;

            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0x2C:{
            uint8_t val = absolute(memory, cpu->pc++);
            (cpu->A & val) == 0 ? (cpu->P |= 0x02) : (cpu->P &= ~0x02);
            
            cpu->P = (val & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (val & 0x40) ? (cpu->P | 0x40) : (cpu->P & ~0x40);
            break;
        }
        case 0x2D:{
            uint8_t val = absolute(memory, cpu->pc++);
            cpu->A &= val;
            
            cpu->P = (cpu->A == 0) ? cpu->P | 0x02 : cpu->P & ~0x02;
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x2E:{
            uint16_t addr = memory[cpu->pc] | (memory[cpu->pc + 1] << 8);
            uint8_t val = memory[addr];

            uint8_t old = cpu->P & 0x01;

            cpu->P = (val & 0x80) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            uint8_t result = (val << 1) | old;
            memory[addr] = result;

            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            cpu->pc+=2;

            break;
        }
        case 0x30:{
            int8_t offset = (int8_t)memory[cpu->pc++];

            if(cpu->P & 0x80) {
                cpu->pc += offset;
            }

            break;
        }
        case 0x31:{
            uint8_t zp = memory[cpu->pc++];
            uint16_t base = memory[zp] | (memory[(uint8_t)(zp + 1)] << 8);
            uint16_t addr = base + cpu->Y;

            cpu->A &= memory[addr];

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x35:{
            uint8_t zp = zeropageXindex(memory, cpu->pc++,cpu->X);
            cpu->A &= zp;
            
            (cpu->A == 0) ? (cpu->P |= 0x02) : (cpu->P &= ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x36:{
            uint8_t addr = memory[cpu->pc];
            uint8_t x_addr = (addr + cpu->X) & 0xFF;
            uint8_t zp = memory[x_addr];

            uint8_t old = cpu->P & 0x01;

            cpu->P = (zp & 0x80) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            uint8_t result = (zp << 1) | old;

            memory[x_addr] = result;

            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            cpu->pc += 1;

            break;
        }
        case 0x38:{
            cpu->P |= 0x01;

            break;
        }
        case 0x39:{
            uint8_t val = absoluteY(memory,cpu->pc++,cpu->Y);
            cpu->A &= val;
            
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x3D:{
            uint8_t val = absoluteX(memory,cpu->pc++,cpu->X);
            cpu->A &= val;
            
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x3E:{
            uint16_t addr = (memory[cpu->pc] | (memory[cpu->pc + 1] << 8)) + cpu->X;
            uint8_t val = memory[addr];

            uint8_t old = cpu->P & 0x01;

            cpu->P = (val & 0x80) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            uint8_t result = (val << 1) | old;
            memory[addr] = result;

            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            cpu->pc+=2;

            break;
        }
        case 0x40:{
            cpu->S++;

            cpu->P = memory[0x0100 + cpu->S];
            cpu->S++;

            uint8_t pcl = memory[0x0100 + cpu->S];
            cpu->S++;

            uint8_t pch = memory[0x0100 + cpu->S];

            cpu->pc = ((uint16_t)pch << 8) | pcl;

            break;
        }
        case 0x41:{
            uint8_t val = indirectXindex(memory,cpu->pc++,cpu->X);
            cpu->A ^= val;
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x45:{
            uint8_t addr = memory[cpu->pc];
            uint8_t val = zeropage(memory,cpu->pc++);
            cpu->A ^= val;
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x46:{
            uint8_t addr = memory[cpu->pc];
            uint8_t val = memory[addr];
            if((val & 0x01) == 0) {
                cpu->P &= ~0x01;
            }else{
                cpu->P |= 0x01;
            }
            memory[cpu->pc] = val>>1;

            cpu->P = (memory[cpu->pc] == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P &= ~0x80;

            cpu->pc+=1;

            break;
        }
        case 0x48:{
            cpu->S--;
            memory[0x0100 + cpu->S] = cpu->A;
            

            break;
        }
        case 0x49:{
            uint8_t val = immediate(memory,cpu->pc++);
            cpu->A ^= val;
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        
        case 0x4A:{
            if((cpu->A & 0x01) == 0) {
                cpu->P &= ~0x01;
            }else{
                cpu->P |= 0x01;
            }
            cpu->A >>= 1;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P &= ~0x80;
            break;
        }
        case 0x4C: {
            low = mem_read(memory, cpu->pc++);
            high = mem_read(memory, cpu->pc++);

            cpu->pc = ((uint16_t)high << 8) | low;
            break;
        }
        case 0x4D:{
            uint8_t val = absolute(memory,cpu->pc);
            cpu->A ^= val;
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->pc+=1;
            break;
        }
        case 0x4E:{
            uint8_t low = memory[cpu->pc];
            uint8_t high = memory[cpu->pc+1];
            uint16_t target = (((uint16_t)high << 8) | low); //memory address

            uint8_t val = memory[target];

            if((val & 0x01) == 0) {
                cpu->P &= ~0x01;
            }else{
                cpu->P |= 0x01;
            }
            memory[target] = val>>1;

            cpu->P = (memory[target] == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P &= ~0x80;

            cpu->pc+=2;

            break;
        }
        case 0x50:{
            int8_t offset = (int8_t)memory[cpu->pc++];

            if ((cpu->P & 0x40) == 0) { // V flag clear
                cpu->pc += offset;
            }

            break;
        }
        case 0x51:{
            uint8_t val = indirectYindex(memory,cpu->pc++,cpu->Y);
            cpu->A ^= val;
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x55:{
            uint8_t val = zeropageXindex(memory,cpu->pc++,cpu->X);
            cpu->A ^= val;
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x56:{
            uint8_t zp = memory[cpu->pc++];
            uint8_t addr = (zp + cpu->X) & 0xFF;

            uint8_t val = memory[addr];
            if((val & 0x01) == 0) {
                cpu->P &= ~0x01;
            }else{
                cpu->P |= 0x01;
            }
            memory[addr] = val>>1;

            cpu->P = (memory[addr] == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P &= ~0x80;

            cpu->pc+=1;

            break;
        }
        case 0x58:{
            cpu->P &= ~0x04;

            break;
        }
        case 0x59:{
            uint8_t val = absoluteY(memory,cpu->pc,cpu->Y);
            cpu->A ^= val;
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->pc+=2;
            break;
        }
        case 0x5D:{
            uint8_t val = absoluteX(memory,cpu->pc,cpu->X);
            cpu->A ^= val;
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->pc+=2;
            break;
        }
        case 0x5E:{
            uint8_t low = memory[cpu->pc];
            uint8_t high = memory[cpu->pc+1];
            uint16_t target = (((uint16_t)high << 8) | low) + cpu->X;

            uint8_t val = memory[target];

            if((val & 0x01) == 0) {
                cpu->P &= ~0x01;
            }else{
                cpu->P |= 0x01;
            }
            memory[target] = val>>1;

            cpu->P = (memory[target] == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P &= ~0x80;

            cpu->pc+=2;

            break;
        }
        case 0x60:{
            cpu->S++;
            low = memory[0x0100 + cpu->S];
            cpu->S++; 
            high = memory[0x0100 + cpu->S];

            target = ((uint16_t)high << 8) | low;

            cpu->pc = target+1;

            break;
        }
        case 0x61:{
            uint8_t val = indirectXindex(memory,cpu->pc++,cpu->X);
            uint16_t sum = cpu->A + val + (cpu->P & 0x01);
            uint8_t result = (uint8_t)sum;
            
            cpu->P = (sum > 0xFF) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x65:{
            uint8_t val = zeropage(memory,cpu->pc++);
            uint16_t sum = cpu->A + val + (cpu->P & 0x01);
            uint8_t result = (uint8_t)sum;
            
            cpu->P = (sum > 0xFF) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x66:{
            uint8_t addr = memory[cpu->pc];
            uint8_t zp = memory[addr];

            uint8_t old = (cpu->P & 0x01) ? 0x80 : 0x00;
            uint8_t result = (zp >> 1) | old;

            cpu->P = (zp & 0x01) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            memory[addr] = result;

            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            cpu->pc += 1;

            break;
        }
        case 0x68:{
            cpu->S++;
            cpu->A = memory[0x0100 + cpu->S];

            break;
        }
        case 0x69:{
            uint8_t val = memory[cpu->pc++];
            uint16_t sum = cpu->A + val + (cpu->P & 0x01);
            uint8_t result = (uint8_t)sum;
            
            cpu->P = (sum > 0xFF) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x6A:{
            uint8_t zp = cpu->A;

            uint8_t old = (cpu->P & 0x01) ? 0x80 : 0x00;
            uint8_t result = (zp >> 1) | old;

            cpu->P = (zp & 0x01) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            cpu->A = result;

            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0x6C:{
            low = mem_read(memory, cpu->pc++);
            high = mem_read(memory, cpu->pc++);

            uint16_t ptr = ((uint16_t)high << 8) | low;

            uint8_t targetLow = memory[ptr];
            uint8_t targetHigh = memory[ptr+1];

            cpu->pc = ((uint16_t)targetHigh << 8) | targetLow;
            break;
        }
        case 0x6D:{
            uint8_t val = absolute(memory,cpu->pc);
            uint16_t sum = cpu->A + val + (cpu->P & 0x01);
            uint8_t result = (uint8_t)sum;
            
            cpu->P = (sum > 0xFF) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 2;

            break;
        }
        case 0x6E:{
            uint8_t addr = memory[cpu->pc];
            uint8_t zp = memory[addr];

            uint8_t old = (cpu->P & 0x01) ? 0x80 : 0x00;
            uint8_t result = (zp >> 1) | old;

            cpu->P = (zp & 0x01) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            memory[addr] = result;

            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            cpu->pc += 1;

            break;
        }
        case 0x70:{
            int8_t offset = (int8_t)memory[cpu->pc++];

            if (cpu->P & 0x40) {
                cpu->pc += offset;
            }

            break;
        }
        case 0x71:{
            uint8_t val = indirectYindex(memory,cpu->pc++,cpu->Y);
            uint16_t sum = cpu->A + val + (cpu->P & 0x01);
            uint8_t result = (uint8_t)sum;
            
            cpu->P = (sum > 0xFF) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x75:{
            uint8_t val = zeropageXindex(memory,cpu->pc++,cpu->X);
            uint16_t sum = cpu->A + val + (cpu->P & 0x01);
            uint8_t result = (uint8_t)sum;
            
            cpu->P = (sum > 0xFF) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x76:{
            uint8_t zp = memory[cpu->pc++];
            uint8_t addr = (zp+cpu->X) & 0xFF;
            uint8_t val = memory[addr];
            

            uint8_t old = (cpu->P & 0x01) ? 0x80 : 0x00;
            uint8_t result = (val >> 1) | old;

            cpu->P = (val & 0x01) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            memory[addr] = result;

            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0x78:{
            cpu->P |= 0x04;

            break;
        }
        case 0x79:{
            uint8_t val = absoluteY(memory,cpu->pc, cpu->Y);
            uint16_t sum = cpu->A + val + (cpu->P & 0x01);
            uint8_t result = (uint8_t)sum;
            
            cpu->P = (sum > 0xFF) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 2;

            break;
        }
        case 0x7D:{
            uint8_t val = absoluteX(memory,cpu->pc,cpu->X);
            uint16_t sum = cpu->A + val + (cpu->P & 0x01);
            uint8_t result = (uint8_t)sum;
            
            cpu->P = (sum > 0xFF) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 2;

            break;
        }
        case 0x7E:{
            uint16_t addr = memory[cpu->pc] | (memory[cpu->pc + 1] << 8);
            addr += cpu->X;
            uint8_t zp = memory[addr];

            uint8_t old = (cpu->P & 0x01) ? 0x80 : 0x00;
            uint8_t result = (zp >> 1) | old;

            cpu->P = (zp & 0x01) ? (cpu->P | 0x01) : (cpu->P & ~0x01);

            memory[addr] = result;

            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (result == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            cpu->pc += 2;

            break;
        }
        case 0x81:{
            uint8_t zp_ptr = (memory[cpu->pc++] + cpu->X) & 0xFF;
            uint8_t low  = memory[zp_ptr];
            uint8_t high = memory[(zp_ptr + 1) & 0xFF];
            uint16_t target = ((uint16_t)high << 8) | low;
            memory[target] = cpu->A; 
            

            break;
        }
        case 0x84:{
            uint8_t zp_addr = memory[cpu->pc++];
            memory[zp_addr] = cpu->Y;
            

            break;
        }
        case 0x85:{
            uint8_t zp_addr = memory[cpu->pc++];
            memory[zp_addr] = cpu->A;
            

            break;
        }
        case 0x86:{
            uint8_t zp_addr = memory[cpu->pc++];
            memory[zp_addr] = cpu->X;
            

            break;
        }
        case 0x88:{
            cpu->Y--;
            break;
        }
        case 0x8A:{
            cpu->A = cpu->X;
            break;
        }
        case 0x8C:{
            uint8_t low = memory[cpu->pc];
            uint8_t high = memory[cpu->pc+1];
            uint16_t target = (((uint16_t)high << 8) | low);

            memory[target] = cpu->Y;

            cpu->pc += 2;

            break;
        }
        case 0x8D:{
            uint8_t low = memory[cpu->pc];
            uint8_t high = memory[cpu->pc+1];
            uint16_t target = (((uint16_t)high << 8) | low);

            memory[target] = cpu->A;

            cpu->pc += 2;

            break;
        }
        case 0x8E:{
            uint8_t low = memory[cpu->pc];
            uint8_t high = memory[cpu->pc+1];
            uint16_t target = (((uint16_t)high << 8) | low);

            memory[target] = cpu->X;

            cpu->pc += 2;

            break;
        }
        case 0x90:{
            int8_t offset = (int8_t)memory[cpu->pc++];

            if ((cpu->P & 0x01) == 0) {
                cpu->pc += offset;
            }

            break;
        }
        case 0x91:{
            uint8_t zp_ptr = memory[cpu->pc++];

            uint8_t low  = memory[zp_ptr];
            uint8_t high = memory[(uint8_t)(zp_ptr + 1)];

            uint16_t base = ((uint16_t)high << 8) | low;

            uint16_t addr = base + cpu->Y;

            memory[addr] = cpu->A;

            break;
        }
        case 0x94:{
            uint8_t zp_addr = memory[cpu->pc++];
            uint16_t target = (zp_addr + cpu->X) & 0xFF;

            memory[target] = cpu->Y;
            
            break;
        }
        case 0x95:{
            uint8_t zp_addr = memory[cpu->pc++];
            uint16_t target = (zp_addr + cpu->X) & 0xFF;

            memory[target] = cpu->A;
            
            break;
        }
        case 0x96:{
            uint8_t zp = memory[cpu->pc++];
            uint8_t target = (zp + cpu->Y) & 0xFF;

            memory[target] = cpu->X;
            
            break;
        }
        case 0x98:{
            cpu->A = cpu->Y;

            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0x99:{
            uint8_t low = memory[cpu->pc++];
            uint8_t high = memory[cpu->pc++];
            uint16_t target = (((uint16_t)high << 8) | low) + cpu->Y;

            memory[target] = cpu->A;

            break;
        }
        case 0x9A:{
            cpu->S = cpu->X;
            break;
        }
        case 0x9D:{
            uint8_t low = memory[cpu->pc++];
            uint8_t high = memory[cpu->pc++];
            uint16_t target = (((uint16_t)high << 8) | low) + cpu->X;

            memory[target] = cpu->A;

            break;
        }
        case 0xA0:{
            value = mem_read(memory, cpu->pc++);
            cpu->Y = value;

            cpu->P = (cpu->Y & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->Y == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0xA1:{
            cpu->A = indirectXindex(memory, cpu->pc++, cpu->X);

            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0xA2:{
            value = mem_read(memory, cpu->pc++);
            cpu->X = value;

            cpu->P = (cpu->X & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->X == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0xA4:{
            cpu->Y = zeropage(memory, cpu->pc++);

            cpu->P = (cpu->Y & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->Y == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0xA5:{
            cpu->A = zeropage(memory, cpu->pc++);

            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0xA6:{
            cpu->X = zeropage(memory, cpu->pc++);

            cpu->P = (cpu->X & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->X == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0xA8:{
            cpu->Y = cpu->A;

            cpu->P = (cpu->Y & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->Y == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0xA9:{
            cpu->A = immediate(memory, cpu->pc++);

            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0xAA:{
            cpu->X = cpu->A;

            cpu->P = (cpu->X & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->X == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0xAC:{
            cpu->Y = absolute(memory, cpu->pc++);

            cpu->P = (cpu->Y & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->Y == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0xAD:{
            cpu->A = absolute(memory, cpu->pc+1);

            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            cpu->pc += 2;
            break;
        }
        case 0xAE:{
            cpu->X = absolute(memory,cpu->pc+1);
            
            cpu->P = (cpu->X & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->X == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            cpu->pc += 2;

            break;
        }
        case 0xB0:{
            int8_t offset = (int8_t)memory[cpu->pc++];

            if (cpu->P & 0x01) { // Carry set
                cpu->pc += offset;
            }

            break;
        }
        case 0xB1:{
            cpu->A = indirectYindex(memory, cpu->pc++, cpu->X);

            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0xB4:{
            cpu->Y = zeropageXindex(memory,cpu->pc++,cpu->X);

            cpu->P = (cpu->Y & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->Y == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            
            break;
        }
        case 0xB5:{
            cpu->A = zeropageXindex(memory, cpu->pc++, cpu->X);

            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        case 0xB6:{
            cpu->X = zeropageYindex(memory, cpu->pc++, cpu->Y);

            cpu->P = (cpu->X & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (cpu->X == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);

            break;
        }
        
        case 0xB8:{
            cpu->P &= ~0x40;

            break;
        }
        case 0xB9:{
            cpu->A = absoluteY(memory, cpu->pc+1, cpu->Y);

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 2;
            break;
        }
        case 0xBA:{
            cpu->X = cpu->S;

            cpu->P = (cpu->X == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->X & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xBC:{
            cpu->Y = absoluteX(memory, cpu->pc+1,cpu->X);

            cpu->P = (cpu->Y == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->Y & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 2;
            break;
        }
        case 0xBD:{
            cpu->A = absoluteX(memory, cpu->pc++, cpu->X);

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xBE:{
            cpu->X = absoluteY(memory, cpu->pc++, cpu->Y);

            cpu->P = (cpu->X == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->X & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xC0:{
            uint8_t val = memory[cpu->pc++];

            uint8_t comp = cpu->Y - val;

            cpu->P = (cpu->Y >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xC1:{
            uint8_t val = indirectXindex(memory,cpu->pc++,cpu->X);

            uint8_t comp = cpu->A - val;

            cpu->P = (cpu->A >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xC4:{
            uint8_t val = zeropage(memory,cpu->pc);

            uint8_t comp = cpu->Y - val;

            cpu->P = (cpu->Y >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xC5:{
            uint8_t val = zeropage(memory,cpu->pc++);

            uint8_t comp = cpu->A - val;

            cpu->P = (cpu->A >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xC6:{
            uint8_t zp_addr = memory[cpu->pc];
            uint8_t val = memory[zp_addr];

            val--;

            memory[zp_addr] = val;

            cpu->P = (val == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (val & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 1;

            break;
        }
        case 0xC8:{
            cpu->Y++;

            cpu->P = (cpu->Y == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->Y & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        } 
        case 0xC9:{
            uint8_t val = memory[cpu->pc++];

            uint8_t comp = cpu->A - val;

            cpu->P = (cpu->A >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xCA:{
            cpu->X--;

            cpu->P = (cpu->X == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->X & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xCC:{
            uint8_t val = absolute(memory,cpu->pc);

            uint8_t comp = cpu->Y - val;

            cpu->P = (cpu->Y >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 2;

            break;
        }
        case 0xCD:{
            uint8_t val = absolute(memory,cpu->pc);

            uint8_t comp = cpu->A - val;

            cpu->P = (cpu->A >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 2;

            break;
        }
        case 0xCE:{
            uint8_t low = memory[cpu->pc];
            uint8_t high = memory[cpu->pc+1];
            uint16_t target = (((uint16_t)high << 8) | low);
            uint8_t val = memory[target];

            val--;

            memory[target] = val;

            cpu->P = (val == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (val & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 2;

            break;
        }
        case 0xD0:{
            int8_t offset = (int8_t)memory[cpu->pc++];

            if ((cpu->P & 0x02) == 0) {
                cpu->pc += offset;
            }

            break;
        }
        case 0xD1:{
            uint8_t val = indirectYindex(memory,cpu->pc++,cpu->Y);

            uint8_t comp = cpu->A - val;

            cpu->P = (cpu->A >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xD5:{
            uint8_t val = zeropageXindex(memory,cpu->pc++,cpu->X);

            uint8_t comp = cpu->A - val;

            cpu->P = (cpu->A >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xD6:{
            uint8_t zp_addr = memory[cpu->pc++];
            uint8_t addr = (zp_addr + cpu->X) & 0xFF;

            uint8_t val = memory[addr];

            val--;

            memory[addr] = val;

            cpu->P = (val == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (val & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xD8:{
            cpu->P &= ~0x08;

            break;
        }
        case 0xD9:{
            uint8_t val = absoluteY(memory,cpu->pc, cpu->Y);

            uint8_t comp = cpu->A - val;

            cpu->P = (cpu->A >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 2;

            break;
        }
        case 0xDD:{
            uint8_t val = absoluteX(memory,cpu->pc, cpu->X);

            uint8_t comp = cpu->A - val;

            cpu->P = (cpu->A >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 2;

            break;
        }
        case 0xDE:{
            uint8_t low = memory[cpu->pc];
            uint8_t high = memory[cpu->pc+1];
            uint16_t target = (((uint16_t)high << 8) | low) + cpu->X;
            uint8_t val = memory[target];

            val--;

            memory[target] = val;

            cpu->P = (val == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (val & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            cpu->pc += 2;

            break;
        }
        case 0xE0:{
            uint8_t val = memory[cpu->pc++];

            uint8_t comp = cpu->X - val;

            cpu->P = (cpu->X >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xE1:{
            uint8_t val = indirectXindex(memory,cpu->pc++,cpu->X);
            uint16_t temp = cpu->A - val - (1 - (cpu->P & 0x01));
            uint8_t result = (uint8_t)temp;
            
            cpu->P = (temp < 0x100) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xE4:{
            uint8_t val = zeropage(memory,cpu->pc++);

            uint8_t comp = cpu->X - val;

            cpu->P = (cpu->X >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xE5:{
            uint8_t val = zeropage(memory,cpu->pc++);
            uint16_t temp = cpu->A - val - (1 - (cpu->P & 0x01));
            uint8_t result = (uint8_t)temp;
            
            cpu->P = (temp < 0x100) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xE6:{
            uint8_t zp_addr = memory[cpu->pc++];

            uint8_t val = memory[zp_addr];

            val++;

            memory[zp_addr] = val;

            cpu->P = (val == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (val & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xE8:{
            cpu->X++;

            cpu->P = (cpu->X == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (cpu->X & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xE9:{
            uint8_t val = immediate(memory,cpu->pc++);
            uint16_t temp = cpu->A - val - (1 - (cpu->P & 0x01));
            uint8_t result = (uint8_t)temp;
            
            cpu->P = (temp < 0x100) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xEA:{

            break;
        }
        case 0xEC:{
            uint8_t val = absolute(memory,cpu->pc++);

            uint8_t comp = cpu->X - val;

            cpu->P = (cpu->X >= val) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = (comp == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (comp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xED:{
            uint8_t val = absolute(memory,cpu->pc++);
            uint16_t temp = cpu->A - val - (1 - (cpu->P & 0x01));
            uint8_t result = (uint8_t)temp;
            
            cpu->P = (temp < 0x100) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xEE:{
            uint8_t low = memory[cpu->pc++];
            uint8_t high = memory[cpu->pc++];
            uint16_t target = (((uint16_t)high << 8) | low);

            uint8_t val = memory[target];

            val++;

            memory[target] = val;

            cpu->P = (val == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (val & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xF0:{
            int8_t offset = (int8_t)memory[cpu->pc++];

            if (cpu->P & 0x02) { // Z flag set
                cpu->pc += offset;
            }

            break;
        }
        case 0xF1:{
            uint8_t val = indirectYindex(memory,cpu->pc++,cpu->Y);
            uint16_t temp = cpu->A - val - (1 - (cpu->P & 0x01));
            uint8_t result = (uint8_t)temp;
            
            cpu->P = (temp < 0x100) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xF5:{
            uint8_t val = zeropageXindex(memory,cpu->pc++,cpu->X);
            uint16_t temp = cpu->A - val - (1 - (cpu->P & 0x01));
            uint8_t result = (uint8_t)temp;
            
            cpu->P = (temp < 0x100) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xF6:{
            uint8_t zp_addr = memory[cpu->pc++];
            uint8_t addr = (zp_addr + cpu->X) & 0xFF;

            uint8_t val = memory[addr];

            val++;

            memory[addr] = val;

            cpu->P = (val == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (val & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xF8:{
            cpu->P |= 0x08;

            break;
        }  
        case 0xF9:{
            uint8_t val = absoluteY(memory,cpu->pc++,cpu->Y);
            uint16_t temp = cpu->A - val - (1 - (cpu->P & 0x01));
            uint8_t result = (uint8_t)temp;
            
            cpu->P = (temp < 0x100) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xFD:{
            uint8_t val = absoluteX(memory,cpu->pc++,cpu->X);
            uint16_t temp = cpu->A - val - (1 - (cpu->P & 0x01));
            uint8_t result = (uint8_t)temp;
            
            cpu->P = (temp < 0x100) ? (cpu->P | 0x01) : (cpu->P & ~0x01);
            cpu->P = ((~(cpu->A ^ val) & (cpu->A ^ result) & 0x80)) ? (cpu->P | 0x40) : (cpu->P & ~0x40);

            cpu->A = result;

            cpu->P = (cpu->A == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (result & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0xFE:{
            uint8_t low = memory[cpu->pc++];
            uint8_t high = memory[cpu->pc++];
            uint16_t target = (((uint16_t)high << 8) | low) + cpu->X;

            uint8_t val = memory[target];

            val++;

            memory[target] = val;

            cpu->P = (val == 0) ? (cpu->P | 0x02) : (cpu->P & ~0x02);
            cpu->P = (val & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        default:
            printf("unknown opcode 0x%X\n",opcode);
            break;
    }
}

void run()
{
    printf("this is a thing :)\n");
}
