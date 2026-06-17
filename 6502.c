#include <stdio.h>
#include "6502.h"
#include "memory.h"

void power_up(cpu_6502 *cpu) {
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->pc = 0;
    cpu->S = 0xFD;
    cpu->P = 0x24;
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
            cpu->A |= immediate(memory, cpu->pc++, cpu->X);
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
            cpu->A |= absolute(memory, cpu->pc++, cpu->X);
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
            (cpu->A & zp) == 0 ? cpu->P |= 0x02 : cpu->P &= ~0x02;
            
            cpu->P = (zp & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);
            cpu->P = (zp & 0x40) ? (cpu->P | 0x40) : (cpu->P & ~0x40);
            break;
        }
        case 0x25:{

            uint8_t zp = zeropage(memory, cpu->pc++);
            cpu->A &= zp;
            
            (cpu->A == 0) ? cpu->P |= 0x02 : cpu->P &= ~0x02;
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
            
            (cpu->A == 0) ? cpu->P |= 0x02 : cpu->P &= ~0x02;
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
            
            (cpu->A == 0) ? cpu->P |= 0x02 : cpu->P &= ~0x02;
            cpu->P = (cpu->A & 0x80) ? (cpu->P | 0x80) : (cpu->P & ~0x80);

            break;
        }
        case 0x36:{
            uint8_t addr = memory[cpu->pc];
            uint8_t x_addr = (addr + x) & 0xFF;
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
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

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
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

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
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x65:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x66:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x68:{
            cpu->S++;
            cpu->A = memory[0x0100 + cpu->S];

            break;
        }
        case 0x69:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x6A:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x6C:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x6D:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x6E:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x70:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x71:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x75:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x76:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x78:{
            cpu->P |= 0x04;

            break;
        }
        case 0x79:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x7D:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x7E:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x81:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x84:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x85:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x86:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

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
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x8D:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x8E:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x90:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x91:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x94:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x95:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x96:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x98:{
            cpu->A = cpu->Y;
            break;
        }
        case 0x99:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x9A:{
            cpu->S = cpu->X;
            break;
        }
        case 0x9D:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xA0:{
            value = mem_read(memory, cpu->pc++);
            cpu->Y = value;
            break;
        }
        case 0xA1:{
            cpu->A = indirectXindex(memory, cpu->pc++, cpu->X);
            break;
        }
        case 0xA2:{
            value = mem_read(memory, cpu->pc++);
            cpu->X = value;
            break;
        }
        case 0xA4:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xA5:{
            cpu->A = zeropage(memory, cpu->pc++);
            break;
        }
        case 0xA6:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xA8:{
            cpu->Y = cpu->A;
            break;
        }
        case 0xA9:{
            cpu->A = immediate(memory, cpu->pc++);
            break;
        }
        case 0xAA:{
            cpu->X = cpu->A;
            break;
        }
        case 0xAC:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xAD:{
            cpu->A = absolute(memory, cpu->pc+1);
            cpu->pc += 2;
            break;
        }
        case 0xAE:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xB0:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xB1:{
            cpu->A = indirectYindex(memory, cpu->pc++, cpu->X);
            break;
        }
        case 0xB4:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xB5:{
            cpu->A = zeropageXindex(memory, cpu->pc++, cpu->X);
            break;
        }
        case 0xB6:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        
        case 0xB8:{
            cpu->P &= ~0x40;

            break;
        }
        case 0xB9:{
            cpu->A = absoluteY(memory, cpu->pc+1);
            cpu->pc += 2;
            break;
        }
        case 0xBA:{
            cpu->X = cpu->S;
            break;
        }
        case 0xBC:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xBD:{
            cpu->A = absoluteX(memory, cpu->pc++, cpu->X);
            break;
        }
        case 0xBE:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xC0:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xC1:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xC4:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xC5:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xC6:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xC8:{
            cpu->Y++;
            break;
        } 
        case 0xC9:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xCA:{
            cpu->X--;
            break;
        }
        case 0xCC:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xCD:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xCE:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xD0:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xD1:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xD5:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xD6:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xD8:{
            cpu->P &= ~0x08;

            break;
        }
        case 0xD9:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xDD:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xDE:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xE0:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xE1:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xE4:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xE5:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xE6:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xE8:{
            cpu->X++;
            break;
        }
        case 0xE9:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xEA:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xEC:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xED:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xEE:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xF0:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xF1:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xF5:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xF6:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xF8:{
            cpu->P |= 0x08;

            break;
        }  
        case 0xF9:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xFD:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0xFE:{
            //TODO
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

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
