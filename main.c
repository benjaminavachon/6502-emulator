#include <stdio.h>
#include "6502.h"

int main(){
    uint8_t memory[65536] = {0};

    cpu_6502 cpu;
    power_up(&cpu);

    reset(&cpu, memory);

    

    memory[0x0000] = 0xA9; // LDA #$05
    memory[0x0001] = 0x05;

    memory[0x0002] = 0xAA; // TAX

    memory[0x0003] = 0xE8; // INX

    memory[0x0004] = 0xA0; // LDY #$10
    memory[0x0005] = 0x10;

    memory[0x0006] = 0xC8; // INY

    for (int i = 0; i < 6; i++) {
        step(&cpu, memory);

        printf("A=%02X X=%02X Y=%02X PC=%04X SP=%02X\n", cpu.A, cpu.X, cpu.Y, cpu.pc, cpu.S);
    }

    return 0;
}
