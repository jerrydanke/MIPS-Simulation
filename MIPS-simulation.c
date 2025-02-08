#include <stdio.h>
#include <string.h>

#define MEM_SIZE 10
#define DATA_MEM_SIZE 100

// ALU 控制訊號
typedef enum { ALU_ADD, ALU_SUB, ALU_AND, ALU_OR, ALU_SLT } ALUOp_t;

// ALU 結構
typedef struct {
    int result;   // ALU 計算結果
    int zero;     // Zero Flag (用於 beq)
} ALU;

// MIPS 指令格式
typedef struct {
    char type;  // 'R' or 'I'
    int rd, rs, rt;
    int imm;
    char op[10];  // 指令名稱，如 "addi"、"add"、"lw"
} Instruction;

// CPU 硬體模擬
typedef struct {
    int regs[32];            // 32 個 MIPS 寄存器
    int pc;                  // 程式計數器
    Instruction instr_mem[MEM_SIZE];  // 指令記憶體
    int data_mem[DATA_MEM_SIZE];  // 數據記憶體
} CPU;

// 初始化 CPU
void init_cpu(CPU *cpu) {
    memset(cpu->regs, 0, sizeof(cpu->regs));
    memset(cpu->data_mem, 0, sizeof(cpu->data_mem));
    cpu->pc = 0;
}

// ALU 執行運算
ALU execute_alu(int input1, int input2, ALUOp_t alu_op) {
    ALU alu;
    switch (alu_op) {
        case ALU_ADD:
            alu.result = input1 + input2;
            break;
        case ALU_SUB:
            alu.result = input1 - input2;
            break;
        case ALU_AND:
            alu.result = input1 & input2;
            break;
        case ALU_OR:
            alu.result = input1 | input2;
            break;
        case ALU_SLT:
            alu.result = (input1 < input2) ? 1 : 0;
            break;
    }
    alu.zero = (alu.result == 0) ? 1 : 0;  // Zero Flag 用於 beq
    return alu;
}

// 加載指令進記憶體
void load_instructions(CPU *cpu) {
    cpu->instr_mem[0] = (Instruction){'I', 8, 0, 0, 1, "addi"};  // addi $t0, $zero, 1
    cpu->instr_mem[1] = (Instruction){'I', 9, 0, 0, 2, "addi"};  // addi $t1, $zero, 2
    cpu->instr_mem[2] = (Instruction){'R', 10, 8, 9, 0, "add"};  // add  $t2, $t0, $t1
    cpu->instr_mem[3] = (Instruction){'R', 11, 10, 9, 0, "sub"}; // sub  $t3, $t2, $t1
    cpu->instr_mem[4] = (Instruction){'R', 12, 8, 9, 0, "and"};  // and  $t4, $t0, $t1
    cpu->instr_mem[5] = (Instruction){'R', 13, 8, 9, 0, "or"};   // or   $t5, $t0, $t1
    cpu->instr_mem[6] = (Instruction){'R', 14, 10, 9, 0, "slt"}; // slt  $t6, $t2, $t1
    cpu->instr_mem[7] = (Instruction){'I', 0, 0, 0, 0, "halt"};  // 結束指令
}

// 執行 MIPS 硬體模擬（流水線 + ALU）
void execute_pipeline(CPU *cpu) {
    int cycle = 0;

    while (1) {
        Instruction instr = cpu->instr_mem[cpu->pc];

        printf("[Cycle %d] Executing: %s\n", cycle, instr.op);

        ALU alu;
        ALUOp_t alu_op;

        if (strcmp(instr.op, "addi") == 0) { // I-type
            alu = execute_alu(cpu->regs[instr.rs], instr.imm, ALU_ADD);
            cpu->regs[instr.rd] = alu.result;
        } else if (strcmp(instr.op, "add") == 0) { // R-type
            alu = execute_alu(cpu->regs[instr.rs], cpu->regs[instr.rt], ALU_ADD);
            cpu->regs[instr.rd] = alu.result;
        } else if (strcmp(instr.op, "sub") == 0) {
            alu = execute_alu(cpu->regs[instr.rs], cpu->regs[instr.rt], ALU_SUB);
            cpu->regs[instr.rd] = alu.result;
        } else if (strcmp(instr.op, "and") == 0) {
            alu = execute_alu(cpu->regs[instr.rs], cpu->regs[instr.rt], ALU_AND);
            cpu->regs[instr.rd] = alu.result;
        } else if (strcmp(instr.op, "or") == 0) {
            alu = execute_alu(cpu->regs[instr.rs], cpu->regs[instr.rt], ALU_OR);
            cpu->regs[instr.rd] = alu.result;
        } else if (strcmp(instr.op, "slt") == 0) {
            alu = execute_alu(cpu->regs[instr.rs], cpu->regs[instr.rt], ALU_SLT);
            cpu->regs[instr.rd] = alu.result;
        } else if (strcmp(instr.op, "halt") == 0) { // 結束
            break;
        }

        cpu->pc++;  // 下一條指令
        cycle++;
    }
}

// 顯示 CPU 狀態
void print_cpu_state(CPU *cpu) {
    printf("PC: %d\n", cpu->pc);
    for (int i = 8; i <= 14; i++) {
        printf("t%d = %d\n", i - 8, cpu->regs[i]);
    }
}

int main(void) {
    CPU cpu;
    init_cpu(&cpu);
    load_instructions(&cpu);

    execute_pipeline(&cpu);  // 執行模擬
    print_cpu_state(&cpu);  // 顯示 CPU 狀態

    return 0;
}
