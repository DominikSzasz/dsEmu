#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

uint16_t RA = 0x0000; // accumulator
uint16_t RB = 0x0000; // gen pur
uint16_t RC = 0x0000; // gen pur
uint16_t RD = 0x0000; // gen pur

uint16_t RSP = 0x0000; // stack pointer
uint16_t RSBP = 0x0000; // stack bottom pointer
uint16_t PC = 0x0000; // program counter
uint16_t INDEX = 0x0000; // index

uint16_t MEM[1000];
int MEMLEN = sizeof(MEM) / sizeof(MEM[0]);

bool CF = 0; // carry flag
bool NF = 0; // negative flag

uint16_t add(uint16_t a, uint16_t b) {
    uint16_t sum = a + b;
    // Check for overflow
    bool overflow = (sum < a) || (sum < b);
    if (overflow) {
        CF = 1;
    } else {
        CF = 0;
    }
    return sum;
}

uint16_t sub(uint16_t a, uint16_t b) {
    bool underflow = b > a; // Check for underflow
    if (underflow) {
        NF = 1;
    } else {
        NF = 0;
    }
    return underflow ? 0 : a - b;
}

void printMEM() {
    printf("000: ");
    for (int i = 0; i < MEMLEN; i++) {
        printf("%03u ", MEM[i]);
        if ((i + 1) % 10 == 0) {
            printf("\n%03d: ", i + 1);
        }
    }
    printf("\n");
}
#define OPSHUTDOWN 0xffff
#define OPHALT 0xfffe
#define OPSTDWRITEHEX 0x0001
#define OPSTDWRITEHEXRA 0x0002
#define OPSTDREAD 0x0003
#define OPJMP 0x0100
#define OPJMPEQCONDRARB 0x0101
#define OPJMPEQCONDRAV 0x0102
#define OPJMPEQCONDVV 0x0103
#define OPJMPEQCONDIV 0x0104
#define OPMEMMOVRAV 0x0200
#define OPMEMMOVRARB 0x0201
#define OPMEMMOVRARC 0x0202
#define OPMEMMOVRARD 0x0203
#define OPALUADD 0x0300
#define OPALUSUB 0x0301
#define OPINCI 0x0302
void runProgram() {
    while(PC < MEMLEN)
    {
        if(MEM[PC] == OPHALT)
            continue;
        else if(MEM[PC] == OPSHUTDOWN)
            break;
        else if(MEM[PC] == OPALUADD)
        {
            RA = add(RA, RB);
            PC++;
        }
        else if(MEM[PC] == OPINCI)
        {
            INDEX++;
            PC++;
        }
        else if(MEM[PC] == OPSTDWRITEHEX)
        {
            printf("0x%04x\n",MEM[PC+1]);
            PC+=2;
        }
        else if(MEM[PC] == OPSTDWRITEHEXRA)
        {
            printf("0x%04x\n",RA);
            PC++;
        }
        else if(MEM[PC] == OPJMPEQCONDRAV)
        {
            if(RA == MEM[PC+1])
                PC = MEM[PC+2];
            else
                PC+=3;
        }
        else if(MEM[PC] == OPJMPEQCONDRARB)
        {
            if(RA == RB)
                PC = MEM[PC+2];
            else
                PC+=3;
        }   
        else if(MEM[PC] == OPJMPEQCONDIV)
        {
            if(INDEX == MEM[PC+1])
                PC = MEM[PC+2];
            else
                PC+=3;
        }     
        else if(MEM[PC] == OPJMP)
        {
            PC = MEM[PC+1];
        }
        else if(MEM[PC] == OPMEMMOVRAV)
        {
            RA = MEM[PC+1];
            PC+=2;
        } 
        else if(MEM[PC] == OPMEMMOVRARB)
        {
            uint16_t placeholder = RA;
            RA = RB;
            RB = placeholder;
            PC+=1;
        } 
        else if(MEM[PC] == OPMEMMOVRARC)
        {
            uint16_t placeholder = RA;
            RA = RC;
            RC = placeholder;
            PC+=1;
        } 
        else if(MEM[PC] == OPMEMMOVRARD)
        {
            uint16_t placeholder = RA;
            RA = RD;
            RD = placeholder;
            PC+=1;
        } 
        else
        {
            printf("ERROR ON LINE: 0x%04x -- 0x%04x", PC, MEM[PC]);
        }
    }
}

int main() {
    // Initialize MEM
    memset(MEM, 0, sizeof(MEM));

    MEM[0] = OPMEMMOVRAV;
    MEM[1] = 0x0001;
    MEM[2] = OPMEMMOVRARB;
    MEM[3] = OPMEMMOVRARB;
    MEM[4] = OPJMPEQCONDRAV;
    MEM[5] = 0x0004;
    MEM[6] = 0x000e;
    MEM[7] = OPMEMMOVRARB;
    MEM[8] = OPMEMMOVRAV;
    MEM[9] = 0x0001;
    MEM[10] = OPALUADD;
    MEM[11] = OPMEMMOVRARB;
    MEM[12] = OPJMP;
    MEM[13] = 0x0003;
    MEM[14] = OPSTDWRITEHEXRA;
    MEM[15] = 0xFFFF;

    runProgram();

    return 0;
}