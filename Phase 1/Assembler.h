/********************************************************************************
* Filename: Assembler.h
* Author: Daniel Urbach
* Date: 4/16/2014
* Description: Assembler class declaration
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

class Assembler
{
private:
    typedef void(Assembler::*FP)(string);
    
    // Utility methods
    void validateRD( int RD );
    void validateRS( int RS );
    void validateADDR( int ADDR );
    void validateCONST( int CONST );
    void write( int binaryCode );
    
    // Instruction methods
    void load( string s );
    void loadi( string s );
    void store( string s );
    void add( string s );
    void addi( string s );
    void addc( string s );
    void addci( string s );
    void sub( string s );
    void subi( string s );
    void subc( string s );
    void subci( string s );
    void _and( string s );
    void andi( string s );
    void _xor( string s );
    void xori( string s );
    void _compl( string s );
    void shl( string s );
    void shla( string s );
    void shr( string s );
    void shra( string s );
    void compr( string s );
    void compri( string s );
    void getstat( string s );
    void putstat( string s );
    void jump( string s );
    void jumpl( string s );
    void jumpe( string s );
    void jumpg( string s );
    void call( string s );
    void _return( string s );
    void read( string s );
    void write( string s );
    void halt( string s );
    void noop( string s );
        
    //variables
    string inputFile, outputFile;
    map<string, FP> instr;

public:

    Assembler();
    void assemble( string fileName );	    
};

#endif
