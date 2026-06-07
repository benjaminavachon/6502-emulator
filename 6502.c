#include <stdio.h>
#include "6502.h"

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
        case 0xA9:{
            value = mem_read(memory, cpu->pc++);
            cpu->A = value;
            break;
        }
        case 0xA2:{
            value = mem_read(memory, cpu->pc++);
            cpu->X = value;
            break;
        }
        case 0xA0:{
            value = mem_read(memory, cpu->pc++);
            cpu->Y = value;
            break;
        }
        case 0xAA:{
            //value = mem_read(memory, cpu->pc++);
            cpu->pc++;
            cpu->X = cpu->A;
            break;
        }
        case 0xA8:{
            //value = mem_read(memory, cpu->pc++);
            cpu->pc++;
            cpu->Y = cpu->A;
            break;
        }
        case 0x8A:{
            //value = mem_read(memory, cpu->pc++);
            cpu->pc++;
            cpu->A = cpu->X;
            break;
        }
        case 0x98:{
            //value = mem_read(memory, cpu->pc++);
            cpu->pc++;
            cpu->A = cpu->Y;
            break;
        }
        case 0xBA:{
            //value = mem_read(memory, cpu->pc++);
            cpu->pc++;
            cpu->X = cpu->S;
            break;
        }
        case 0x9A:{
            //value = mem_read(memory, cpu->pc++);
            cpu->pc++;
            cpu->S = cpu->X;
            break;
        }
        case 0xE8:{
            //value = mem_read(memory, cpu->pc++);
            cpu->pc++;
            cpu->X++;
            break;
        }
        case 0xC8:{
            //value = mem_read(memory, cpu->pc++);
            cpu->pc++;
            cpu->Y++;
            break;
        } 
        case 0xCA:{
            //value = mem_read(memory, cpu->pc++);
            cpu->pc++;
            cpu->X--;
            break;
        }
        case 0x88:{
            //value = mem_read(memory, cpu->pc++);
            cpu->pc++;
            cpu->Y--;
            break;
        }
        case 0x4C: {
            low = mem_read(memory, cpu->pc++);
            high = mem_read(memory, cpu->pc++);

            cpu->pc = ((uint16_t)high << 8) | low;
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
        case 0x60:{
            cpu->S++;
            low = memory[0x0100 + cpu->S];
            cpu->S++; 
            high = memory[0x0100 + cpu->S];

            target = ((uint16_t)high << 8) | low;

            cpu->pc = target+1;

            break;
        }
        case 0x48:{
            cpu->S--;
            memory[0x0100 + cpu->S] = cpu->A;
            

            break;
        }
        case 0x68:{
            cpu->S++;
            cpu->A = memory[0x0100 + cpu->S];

            break;
        }
        case 0x08:{
            cpu->S--;
            memory[0x0100 + cpu->S] = cpu->P;
            

            break;
        }
        case 0x28:{
            cpu->S++;
            cpu->P = memory[0x0100 + cpu->S];
            

            break;
        }
        case 0x18:{
            cpu->P &= ~0x01;

            break;
        }
        case 0x38:{
            cpu->P |= 0x01;

            break;
        }
        case 0x58:{
            cpu->P &= ~0x04;

            break;
        }
        case 0x78:{
            cpu->P |= 0x04;

            break;
        }
        case 0xB8:{
            cpu->P &= ~0x40;

            break;
        }
        case 0xD8:{
            cpu->P &= ~0x08;

            break;
        }
        case 0xF8:{
            cpu->P |= 0x08;

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
