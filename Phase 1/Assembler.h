/********************************************************************************
 * James Small & William Elder
 * Assembler.h
 * 4/11/13
 * This program simultes an assembler, translating assembly instructions into
 * binary instruction format.
 ********************************************************************************/

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <cstdlib>
#include <stdexcept>

using namespace std;

class NullPointerException: public runtime_error {
public:
	NullPointerException(): runtime_error("Invalid Operand Read") { }
};

class Assembler{
private:

    //typedefs
    typedef void(Assembler::*FP)(string);
    
    // Utility Functions
    void checkRD(int RD);
    void checkRS(int RS);
    void checkADDR(int ADDR);
    void checkCONST(int CONST);
    void write(int objcode);
    
    // Instruction Functions
    void load(string s);
    void loadi(string s);
    void store(string s);
    void add(string s);
    void addi(string s);
    void addc(string s);
    void addci(string s);
    void sub(string s);
    void subi(string s);
    void subc(string s);
    void subci(string s);
    void _and(string s);
    void andi(string s);
    void _xor(string s);
    void xori(string s);
    void _compl(string s);
    void shl(string s);
    void shla(string s);
    void shr(string s);
    void shra(string s);
    void compr(string s);
    void compri(string s);
    void getstat(string s);
    void putstat(string s);
    void jump(string s);
    void jumpl(string s);
    void jumpe(string s);
    void jumpg(string s);
    void call(string s);
    void _return(string s);
    void read(string s);
    void write(string s);
    void halt(string s);
    void noop(string s);
        
    //variables
    string inputFileName, outputFileName;
    map<string, FP> instr;

public:

    Assembler();
    void assemble(string filename);	    
};

#endif
