/********************************************************************************
 * James Small & William Elder
 * os.cpp
 * 4/17/13
 * This is the OS program that takes as an argument the filename of the assembly
 * program that needs to be run.  It first passes the filename to the assembler
 * program to create the object code, it then passes the filename to the Virtual
 * Machine program to run the previously created object code.
 ********************************************************************************/

#include "Assembler.h"
#include "VirtualMachine.h"

using namespace std;

int  main(int argc, char * argv[])
{
    Assembler as;
    VirtualMachine vm;
    as.assemble(argv[1]);
    vm.run(argv[1]);
} 
