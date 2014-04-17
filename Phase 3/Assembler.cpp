/********************************************************************************
 * James Small & William Elder
 * Assembler.cpp
 * 4/11/13
 * This program simultes an assembler, translating assembly instructions into
 * binary instruction format.
 ********************************************************************************/


#include "Assembler.h"

Assembler::Assembler()
{
    instr["load"] = &Assembler::load;
    instr["loadi"] = &Assembler::loadi;
    instr["store"] = &Assembler::store;
    instr["add"] = &Assembler::add;
    instr["addi"] = &Assembler::addi;
    instr["addc"] = &Assembler::addc;
    instr["addci"] = &Assembler::addci;
    instr["sub"] = &Assembler::sub;
    instr["subi"] = &Assembler::subi;
    instr["subc"] = &Assembler::subc;
    instr["subci"] = &Assembler::subci;
    instr["and"] = &Assembler::_and;
    instr["andi"] = &Assembler::andi;
    instr["xor"] = &Assembler::_xor;
    instr["xori"] = &Assembler::xori;
    instr["compl"] = &Assembler::_compl;
    instr["shl"] = &Assembler::shl;
    instr["shla"] = &Assembler::shla;
    instr["shr"] = &Assembler::shr;
    instr["shra"] = &Assembler::shra;
    instr["compr"] = &Assembler::compr;
    instr["compri"] = &Assembler::compri;
    instr["getstat"] = &Assembler::getstat;
    instr["putstat"] = &Assembler::putstat;
    instr["jump"] = &Assembler::jump;
    instr["jumpl"] = &Assembler::jumpl;
    instr["jumpe"] = &Assembler::jumpe;
    instr["jumpg"] = &Assembler::jumpg;
    instr["call"] = &Assembler::call;
    instr["return"] = &Assembler::_return;
    instr["read"] = &Assembler::read;
    instr["write"] = &Assembler::write;
    instr["halt"] = &Assembler::halt;
    instr["noop"] = &Assembler::noop;    
}

void Assembler::assemble(string filename)
{
    string line, opcode;
	
    // Set File Names for Output and Input
    inputFileName=filename;
    outputFileName = filename.erase(filename.length() -2, 2) + ".o";
    
    // Create Output .o File
    fstream output;
    output.open(outputFileName.c_str(), ios::out);
    output.close();

	
    // Open .s file for input	
    fstream assemblyProg;
    assemblyProg.open(inputFileName.c_str(), ios::in);
	
    if (!assemblyProg.is_open()) {
        cout << inputFileName << " failed to open.\n";
        exit(1);
    }

    getline(assemblyProg, line);
    
    while (!assemblyProg.eof()) {
        
        if (line[0] == '!') {
            getline(assemblyProg, line);
            continue;
        }
        istringstream str(line.c_str());

        try {
            str >> opcode;
            if (not instr[opcode] )
		    throw NullPointerException();
		else 
                (this->*instr[opcode])(line.c_str());
	}
	catch (NullPointerException e) {
		cerr << e.what() << endl;
		exit(1);
	}
        getline(assemblyProg, line);
    }
    assemblyProg.close();
}

void Assembler::write(int objcode)
{
    fstream fout;
    fout.open(outputFileName.c_str(), ios::in | ios::out | ios::ate);

    if (!fout.is_open()) {
        cout << outputFileName << " failed to open.\n";
        exit(1);
    }
    fout << objcode << endl;
    fout.close();
}

void Assembler::checkRD(int RD)
{
    if (RD > 3 || RD < 0){
        cout << "RD value out of range" << endl;
        exit(1);
    }
}

void Assembler::checkRS(int RS)
{
    if (RS > 3 || RS < 0){
        cout << "RS value out of range" << endl;
        exit(1);
    }
}

void Assembler::checkADDR(int ADDR)
{  
    if (ADDR > 255 || ADDR < 0) {
        cout << "ADDR value out of range" << endl;
        exit(1);
    }
}

void Assembler::checkCONST(int CONST)
{
    if (CONST > 127 || CONST < -128){
        cout << "CONST value out of range" << endl;
        exit(1);
    }
}

void Assembler::load(string s)
{
    int RD, ADDR;
    string OPCODE;
    
    istringstream stream(s.c_str());  
    stream >> OPCODE >> RD >> ADDR; // Don't need OPCODE
    
    checkRD(RD);
    checkADDR(ADDR);
 
    int finalnum = 0;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += ADDR;

    write(finalnum);
}

void Assembler::loadi(string s)
{
    int RD, CONST, I=1;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> CONST; // Don't need OPCODE

    checkRD(RD);
    checkCONST(CONST);
    
    int finalnum = 0;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;
    
    if (CONST < 0)
        CONST = CONST & 0x00FF;
    
    finalnum += CONST;

    write(finalnum);
}

void Assembler::store(string s)
{
    int RD, ADDR;
    string OPCODE;

    istringstream stream(s.c_str());        
    stream >> OPCODE >> RD >> ADDR; // Don't need OPCODE

    checkRD(RD);
    checkADDR(ADDR);
        
    int finalnum = 1;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += ADDR;

    write(finalnum);
}
  
void Assembler::add(string s)
{
    int RD, RS, I = 0;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> RS; // Don't need OPCODE
    
    checkRD(RD);
    checkRS(RS);
    
    int finalnum = 2;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;
    finalnum += RS << 6;

    write(finalnum);
}
    
void Assembler::addi(string s)
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> CONST; // Don't need OPCODE
    
    checkRD(RD);
    checkCONST(CONST);

    int finalnum = 2;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;
    
    if (CONST < 0)
        CONST = CONST & 0x00FF;
        
    finalnum += CONST;

    write(finalnum);
}
    
void Assembler::addc(string s)
{
    int RD, RS, I = 0;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> RS; // Don't need OPCODE

    checkRD(RD);
    checkRS(RS);

    int finalnum = 3;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;
    finalnum += RS << 6;

    write(finalnum);
}
    
void Assembler::addci(string s)
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> CONST; // Don't need OPCODE

    checkRD(RD);
    checkCONST(CONST);

    int finalnum = 3;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;

    if (CONST < 0)
        CONST = CONST & 0x00FF;
        
    finalnum += CONST;

    write(finalnum);
}
    
void Assembler::sub(string s)
{
    int RD, RS, I = 0;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> RS; // Don't need OPCODE

    checkRD(RD);
    checkRS(RS);

    int finalnum = 4;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;
    finalnum += RS << 6;

    write(finalnum);
}

void Assembler::subi(string s)
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> CONST; // Don't need OPCODE

    checkRD(RD);
    checkCONST(CONST);

    int finalnum = 4;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;

    if (CONST < 0)
        CONST = CONST & 0x00FF;
        
    finalnum += CONST;

    write(finalnum);
}
    
void Assembler::subc(string s)
{
    int RD, RS, I = 0;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> RS; // Don't need OPCODE

    checkRD(RD);
    checkRS(RS);

    int finalnum = 5;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;
    finalnum += RS << 6;

    write(finalnum);
}

void Assembler::subci(string s)
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> CONST; // Don't need OPCODE

    checkRD(RD);
    checkCONST(CONST);

    int finalnum = 5;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;

    if (CONST < 0)
        CONST = CONST & 0x00FF;
        
    finalnum += CONST;

    write(finalnum);
}
    
void Assembler::_and(string s)
{
    int RD, RS, I = 0;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> RS; // Don't need OPCODE

    checkRD(RD);
    checkRS(RS);

    int finalnum = 6;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;
    finalnum += RS << 6;

    write(finalnum);
}
   
void Assembler::andi(string s)
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> CONST; // Don't need OPCODE

    checkRD(RD);
    checkCONST(CONST);

    int finalnum = 6;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;

    if (CONST < 0)
        CONST = CONST & 0x00FF;
       
    finalnum += CONST;

    write(finalnum);
}
    
void Assembler::_xor(string s)
{
    int RD, RS, I = 0;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> RS; // Don't need OPCODE

    checkRD(RD);
    checkRS(RS);

    int finalnum = 7;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;
    finalnum += RS << 6;

    write(finalnum);
}
    
void Assembler::xori(string s)
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> CONST; // Don't need OPCODE

    checkRD(RD);
    checkCONST(CONST);

    int finalnum = 7;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;

    if (CONST < 0)
        CONST = CONST & 0x00FF;
        
    finalnum += CONST;

    write(finalnum);
}
    
void Assembler::_compl(string s)
{
    int RD, I = 0;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD; // Don't need OPCODE

    checkRD(RD);

    int finalnum = 8;
    finalnum = finalnum << 11;
    finalnum += RD << 9;

    write(finalnum);
}
   
void Assembler::shl(string s)
{
    int RD, I = 0;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD; // Don't need OPCODE

    checkRD(RD);

    int finalnum = 9;
    finalnum = finalnum << 11;
    finalnum += RD << 9;

    write(finalnum);
}
    
void Assembler::shla(string s)
{
    istringstream stream(s.c_str());
    int RD, I=0;
    string OPCODE;
    stream>>OPCODE>>RD; // Don't need OPCODE
    checkRD(RD);
    int finalnum = 10;
    finalnum = finalnum<<11;
    finalnum += RD<<9;
    write(finalnum);
}

void Assembler::shr(string s)
{
    int RD;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD; // Don't need OPCODE

    checkRD(RD);

    int finalnum = 11;
    finalnum = finalnum << 11;
    finalnum += RD << 9;

    write(finalnum);
}

void Assembler::shra(string s)
{
    int RD;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD; // Don't need OPCODE

    checkRD(RD);

    int finalnum = 12;
    finalnum = finalnum << 11;
    finalnum += RD << 9;

    write(finalnum);
}

void Assembler::compr(string s)
{
    int RD, RS;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> RS; // Don't need OPCODE

    checkRD(RD);
    checkRS(RS);

    int finalnum = 13;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += RS << 6;

    write(finalnum);
}

void Assembler::compri(string s)
{
    int RD, I = 1, CONST;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD >> CONST; // Don't need OPCODE

    checkRD(RD);
    checkCONST(CONST);

    int finalnum = 13;
    finalnum = finalnum << 11;
    finalnum += RD << 9;
    finalnum += I << 8;

    if (CONST < 0)
        CONST = CONST & 0x00FF;
    
    finalnum += CONST;

    write(finalnum);
}

void Assembler::getstat(string s)
{
    int RD;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD; // Don't need OPCODE

    checkRD(RD);

    int finalnum = 14;
    finalnum = finalnum << 11;
    finalnum += RD << 9;

    write(finalnum);
}

void Assembler::putstat(string s)
{
    int RD;
    string OPCODE;
    istringstream stream(s.c_str());

    stream >> OPCODE >> RD; // Don't need OPCODE

    checkRD(RD);

    int finalnum = 15;
    finalnum = finalnum << 11;
    finalnum += RD << 9;

    write(finalnum);
}

void Assembler::jump(string s)
{
    int ADDR;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> ADDR; // Don't need OPCODE

    checkADDR(ADDR);

    int finalnum=16;
    finalnum = finalnum << 11;
    finalnum += ADDR;

    write(finalnum);
}

void Assembler::jumpl(string s)
{
    int ADDR;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> ADDR; // Don't need OPCODE

    checkADDR(ADDR);

    int finalnum = 17;
    finalnum = finalnum << 11;
    finalnum += ADDR;

    write(finalnum);
}

void Assembler::jumpe(string s)
{
    int ADDR;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> ADDR; // Don't need OPCODE

    checkADDR(ADDR);

    int finalnum = 18;
    finalnum = finalnum << 11;
    finalnum += ADDR;

    write(finalnum);
}

void Assembler::jumpg(string s)
{
    int ADDR;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> ADDR; // Don't need OPCODE

    checkADDR(ADDR);

    int finalnum = 19;
    finalnum = finalnum << 11;
    finalnum += ADDR;

    write(finalnum);
}

void Assembler::call(string s)
{
    int ADDR;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> ADDR; // Don't need OPCODE

    checkADDR(ADDR);

    int finalnum = 20;
    finalnum = finalnum << 11;
    finalnum += ADDR;

    write(finalnum);
}

void Assembler::_return(string s)
{
    int finalnum = 21;
    write(finalnum << 11);
}

void Assembler::read(string s)
{
    int RD;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD; // Don't need OPCODE

    checkRD(RD);

    int finalnum = 22;
    finalnum = finalnum << 11;
    finalnum += RD << 9;

    write(finalnum);
}
    
void Assembler::write(string s)
{
    int RD;
    string OPCODE;

    istringstream stream(s.c_str());
    stream >> OPCODE >> RD; // Don't need OPCODE

    checkRD(RD);

    int finalnum = 23;
    finalnum = finalnum << 11;
    finalnum += RD << 9;

    write(finalnum);
}

void Assembler::halt(string s)
{
    int finalnum = 24;
    write(finalnum << 11);
}

void Assembler::noop(string s)
{
    int finalnum = 25;
    write(finalnum << 11);
}

