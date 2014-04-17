/******************************************************************
mapFuncPtr1.cpp
K Zemoudeh
4/5/10

This program illustrates how a map of function pointers could
be used to implement the Assembler class.
******************************************************************/

#include <iostream>
#include <string>
#include <map>

using namespace std;

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

	cin >> opcode;

	/***********************************************************
 	* The syntax for invoking a pointer to a member function is:
 	*
 	* object.*member-function-pointer
 	* or
 	* object_ptr->*member-function-pointer
 	***********************************************************/
	(this->*instr[opcode])("something");
}

main()
{
	Assembler a;
	a.assemble();
}
