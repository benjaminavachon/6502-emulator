#include <stdio.h>
#include "6502.h"

int main(){
    uint8_t memory[65536] = {0};

    cpu_6502 cpu;
    power_up(&cpu);

    load_test_program(memory);

    memory[0xFFFC] = 0x00;
    memory[0xFFFD] = 0x80;

    cpu.pc = 0x8000;

    for (int i = 0; i < 10; i++) {
        step(&cpu, memory);

        printf("A=%02X X=%02X Y=%02X PC=%04X SP=%02X\n",
            cpu.A, cpu.X, cpu.Y, cpu.pc, cpu.S);
    }

    return 0;
}
