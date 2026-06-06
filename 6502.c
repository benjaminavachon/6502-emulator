#include <stdio.h>
#include "6502.h"

void power_up(cpu_6502 *cpu) {
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->pc = 0;
    cpu->S = 0xFD;
    cpu->P = 0;
}

void step(cpu_6502 *cpu,uint8_t* memory) {
    uint8_t opcode = mem_read(memory,cpu->pc++);
    uint8_t value;
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
        case 0x4C:{
            value = mem_read(memory, cpu->pc++);
            cpu->pc = value;
            break;
        }
        case 0x20:
            break;
        case 0x60:
            break;
        case 0x48:
            break;
        case 0x68:
            break;
        case 0x08:
            break;
        case 0x28:
            break;
        case 0x18:
            break;
        case 0x38:
            break;
        case 0x58:
            break;
        case 0x78:
            break;
        case 0xB8:
            break;
        case 0xD8:
            break;
        case 0xF8:
            break;   
        default:
            printf("unknown opcode 0x%X\n",opcode);
            break;
    }
}

void run()
{
    printf("this is a thing :)\n");
}
