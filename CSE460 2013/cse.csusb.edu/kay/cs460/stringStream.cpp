/*********************************************************************
stringStream.cpp
K Zemoudeh
4/4/2011

This program illustrates how an assembly program line could be parsed.
**********************************************************************/

#include <cstdlib>  // for exit()
#include <iostream> // for cout, ...
#include <fstream>  // for fstream::open()
#include <string>
#include <sstream>  // for istringstream

using namespace std;

main()
{
	fstream assemblyProg;
	string line, opcode;
	int rd, rs, constant;

	assemblyProg.open("prog.s", ios::in);
	if (!assemblyProg.is_open()) {
		cout << "prog.s failed to open.\n";
		exit(1);
	}

	getline(assemblyProg, line);
	while (!assemblyProg.eof()) {
		rd=-1; rs=-1; constant=-129; // init to invalid values

		// the following line is the trick
		istringstream str(line.c_str());
		str >> opcode;

		if (opcode == "add")
			str >> rd >> rs;
		else if (opcode == "addi")
			str >> rd >> constant;
		else cout << "Some other opcode: ";

		cout << opcode << " " << rd << " " << rs << " " << constant << endl;
		getline(assemblyProg, line);
	}
}
