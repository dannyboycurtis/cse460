/******************************************************************
mapFuncPtr2.cpp
K Zemoudeh
4/5/10

This program improves on mapFuncPtr.cpp by incorporating Exception 
Handling. The original program fails on an invalid opcode; this
program doesn't.
*******************************************************************/

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>

using namespace std;

class NullPointerException: public runtime_error {
public:
	NullPointerException(): runtime_error("Null Pointer!") { }
};

class Assembler {
	// define Function Pointer type
	typedef void (Assembler::*FP)(string);

	// each instruction has its own function
	void add(string s) { cout << "ADD " << s << endl; }
	void sub(string s) { cout << "SUB " << s << endl; }
	void load(string s) { cout << "LOAD " << s << endl; }
	// ... and the rest

	// the map
	map<string, FP> instr;
public:
	Assembler();
	int assemble();
};

// constructor: populates the map with the address of each function
Assembler::Assembler()
{
	instr["add"] = &Assembler::add;
	instr["sub"] = &Assembler::sub;
	instr["load"] = &Assembler::load;
	// ... and the rest
}

int Assembler::assemble()
{
	string opcode;

	try {
		cin >> opcode;
		if (not instr[opcode] )
			throw NullPointerException();
		else (this->*instr[opcode])("something");
	}
	catch (NullPointerException e) {
		cerr << e.what() << endl;
		return 1;
	}
}

main()
{
	Assembler a;
	a.assemble();
}
