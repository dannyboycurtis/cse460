/********************************************************************************
* Filename: os.cpp
* Author: Daniel Urbach
* Date: 4/16/2014
* Description: This program simulates an operating system by taking a file
* with assembly instructions, creating the object file and running the code
********************************************************************************/

#include "Assembler.h"
#include "VirtualMachine.h"

using namespace std;

int main( int argc, char * argv[] )
{
    Assembler ass;
    VirtualMachine vm;
    ass.assemble( argv[1] );
    vm.run( argv[1] );
} // end main function
