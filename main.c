#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "6502.h"
#include "memory.h"

uint8_t *load_file(const char *file, size_t *size){
    FILE *f = fopen(file, "rb");

    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t *buffer = malloc(*size);
    fread(buffer, 1, *size, f);

    fclose(f);
    return buffer;
}

int main(){
    uint8_t memory[65536] = {0};

    cpu_6502 cpu;
    power_up(&cpu);

    size_t rom_size;
    uint8_t *rom = load_file("baseball.nes", &rom_size);

    load_program(memory, rom);

    reset(&cpu, memory);

    for (int i = 0; i < 5; i++) {
        printf("A=%02X X=%02X Y=%02X PC=%04X opcode=%02x S=%02X P=%02X\n", cpu.A, cpu.X, cpu.Y, cpu.pc, memory[cpu.pc],cpu.S, cpu.P);

        step(&cpu, memory);

    }

    return 0;
}
