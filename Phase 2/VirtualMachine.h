/********************************************************************************
 * James Small & William Elder
 * VirtualMachine.H
 * 4/17/13
 * This program simulates a 16 bit virtual machine.  It reads in machine code 
 * created by the Assembler program, pulls them all into memory, and then executes
 * each instruction one at a time.
 * Modified 5/7/13: Add the ability to run multiple programs at a time.  Main run
 * function was modified to allow this.  Also added the ability to load programs
 * into memory before beginning to run the programs.
 ********************************************************************************/

#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <stdexcept>

using namespace std;

class format1 {
public:
    unsigned UNUSED:6;
    unsigned RS:2;
    unsigned I:1;
    unsigned RD:2;
    unsigned OP:5;
};

class format2 {
public:
    unsigned ADDR:8;
    unsigned I:1;
    unsigned RD:2;
    unsigned OP:5;
};

class format3 {
public:
    int CONST:8;
    unsigned I:1;
    unsigned RD:2;
    unsigned OP:5;
};

union instruction {
    int i;
    format1 f1;
    format2 f2;
    format3 f3;
};

class VirtualMachine {
private:
    //typedefs
    typedef void(VirtualMachine::*FP)();

    // Variables
    static const int REG_FILE_SIZE = 4;
    static const int MEM_SIZE = 256;
    vector < int > reg;
    vector < int > mem;
    instruction finalcode;
    int pc, ir, sr, sp, base, limit, clock, nextMemoryLocation;
    map<int, FP> instr;
    string inputFileName, readFileName, writeFileName;
    bool execute;
       
// Operations

    void setCarryBit();
    void setRightCarryBit();
    void setOverflow();
    int getCarryBit();
    void setGreaterThan();
    int getGreaterThan();
    void resetGreaterThan();
    void setLessThan();
    int getLessThan();
    void resetLessThan();
    void setEqualTo();
    int getEqualTo();
    void resetEqualTo();
    void returnCode(int i);
    void readOrWrite(int i);

// Instruction Functions
  
    void load();
    void loadi();
    void store();
    void add();
    void addi();
    void addc();
    void addci();   
    void sub();
    void subi();
    void subc();
    void subci();
    void _and();
    void andi();
    void _xor();
    void xori();
    void _compl();
    void shl();
    void shla();
    void shr();
    void shra();
    void compr();
    void compri();
    void getstat();
    void putstat();
    void jump();
    void jumpl();
    void jumpe();
    void jumpg();
    void call();
    void _return();   
    void read();
    void write();
    void halt();
    void noop();

public:
    VirtualMachine();
    void run();
    void loadIntoMemory(string inputFileName, int * progBase, int * progLimit);
    friend class OS;
};

#endif
