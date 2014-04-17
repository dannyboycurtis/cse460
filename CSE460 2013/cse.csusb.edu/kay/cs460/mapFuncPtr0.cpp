/******************************************************************
mapFuncPtr0.cpp
K Zemoudeh
4/7/11

This program illustrates how a map of function pointers works.
******************************************************************/

#include <iostream>
#include <string>
#include <map>

using namespace std;

// define Function Pointer type
typedef void (*FP)(string);

// each instruction has its own function
void add(string s) { cout << "ADD " << s << endl; }
void sub(string s) { cout << "SUB " << s << endl; }
void load(string s) { cout << "LOAD " << s << endl; }
// ... and the rest

main()
{
	// the map
	map<string, FP> instr;

	instr["add"] = &add;
	instr["sub"] = &sub;
	instr["load"] = &load;
	// ... and the rest

	string opcode;

	cin >> opcode;
	(*instr[opcode])("something");
}
