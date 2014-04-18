/********************************************************************************
* Filename: VirtualMachine.cpp
* Author: Daniel Urbach
* Date: 4/16/2014
* Description: This program simulates a machine by executing code
********************************************************************************/

#include "VirtualMachine.h"

// VirtualMachine Constructor
VirtualMachine::VirtualMachine()
{
	// Instantiate registers and memory
    reg = vector <int> ( REG_FILE_SIZE );
    mem = vector <int> ( MEM_SIZE );

	// Instantiate 'execute' switch
    execute = true;

    instr[0] = &VirtualMachine::load;
    instr[1] = &VirtualMachine::store;
    instr[2] = &VirtualMachine::add;
    instr[3] = &VirtualMachine::addc;
    instr[4] = &VirtualMachine::sub;
    instr[5] = &VirtualMachine::subc;
    instr[6] = &VirtualMachine::_and;
    instr[7] = &VirtualMachine::_xor;
    instr[8] = &VirtualMachine::_compl;
    instr[9] = &VirtualMachine::shl;
    instr[10] = &VirtualMachine::shla;
    instr[11] = &VirtualMachine::shr;
    instr[12] = &VirtualMachine::shra;
    instr[13] = &VirtualMachine::compr;
    instr[14] = &VirtualMachine::getstat;
    instr[15] = &VirtualMachine::putstat;
    instr[16] = &VirtualMachine::jump;
    instr[17] = &VirtualMachine::jumpl;
    instr[18] = &VirtualMachine::jumpe;
    instr[19] = &VirtualMachine::jumpg;
    instr[20] = &VirtualMachine::call;
    instr[21] = &VirtualMachine::_return;  
    instr[22] = &VirtualMachine::read;
    instr[23] = &VirtualMachine::write;
    instr[24] = &VirtualMachine::halt;
    instr[25] = &VirtualMachine::noop;
} // end VirtualMachine Constructor


// run method
// Execute program from binary code
void VirtualMachine::run( string fileName )
{
    pc = 0;
	ir = 0;
	sr = 0;
	sp = 256;
	base = 0;
	limit = 0;
	clock = 0;
	reg[0] = 0;
	reg[1] = 0;
	reg[2] = 0;
	reg[3] = 0;

    // set file names for input and output
    oFileName = fileName.erase( fileName.length() -2, 2 ) + ".o";
    inFileName = fileName.erase( oFileName.length() -2, 2 ) + ".in";
    outFileName = fileName.erase( inFileName.length() -3, 2 ) + ".out";
    
    // Read file into memory
    fstream oFile;
    oFile.open( oFileName.c_str(), ios::in );
	
    if ( !oFile.is_open() )
	{
        cout << "Failed to open " << oFileName << endl;
        exit( 1 );
    }

    string buffer;
    getline( oFile, buffer );
    
	int l = 0;

	// Increase memory limit
    while ( !oFile.eof() )
	{
        mem[l++] = atoi( buffer.c_str() );
        getline( oFile, buffer );
    }

    oFile.close();

	limit = l;
	cout << "limit: " << limit << endl;
    // execute loop
    while ( execute )
	{
        ir = mem[pc++];
        binaryCode.i = ir;
        ( this->*instr[binaryCode.f1.OP] )();
		cout << "pc: " << pc << endl;
		cout << "reg[0]: " << reg[0] << endl;
    }

    // write final clock to .out file   
    fstream outFile;
    outFile.open( outFileName.c_str(), ios::in | ios::out | ios::ate );
    outFile << "clock = " << clock << endl;
    outFile.close();

} // end run method


// setCarryBit method
// Set the carry bit
void VirtualMachine::setCarryBit() 
{
	// Get 17th bit
    int carryBit = reg[binaryCode.f1.RD] & 0x10000;
    carryBit = carryBit >> 17;

	// If sr carry bit is 1, reset to 0    
    if ( sr & 0x1 )
		sr -= 1;
    
	// Set carry bit to the register value
    sr += carryBit;
} // end setCarryBit method


// setRightCarryBit method
// Set the carry bit
void VirtualMachine::setRightCarryBit()
{
	// If sr carry bit is 1, reset to 0
    if ( sr & 0x1 )
        sr -= 1;

	// Set carry bit to 1
    sr += 1;
} // end setRightCarryBit method


// getCarryBit method
// Get the carry bit
int VirtualMachine::getCarryBit() 
{
    return ( sr & 0x1 );
} // end getCarryBit method


// setOverflow method
// Set the overflow bit
void VirtualMachine::setOverflow() 
{
    if ( !( sr & 0x10 ) )
        sr += 16;
} // end setOverflow method


// setGreatThan method
// Set the greater-than bit
void VirtualMachine::setGreaterThan()
{
    if ( !( sr & 0x2 ) )
        sr += 2;
} // end setGreaterThan method


// getGreaterThan method
// Get the greater-than bit
int VirtualMachine::getGreaterThan()
{
    int temp = sr & 0x2;
    temp = temp >> 1;
    return temp;
} // end getGreaterThan method


// resetGreaterThan method
// Reset the greater-than bit
void VirtualMachine::resetGreaterThan()
{
    sr = sr & 0xfffffffd;
} // end resetGreaterThan method


// setLessThan method
// Set less-than bit
void VirtualMachine::setLessThan()
{
    if ( !( sr & 0x8 ) )
        sr += 8;
} // end setLessThan method


// getLessThan method
// Get less-than bit
int VirtualMachine::getLessThan()
{
    int temp = sr & 0x8;
    temp = temp >> 3;
    return temp;
} // end getLessThan method


// resetLessThan method
// Reset less-than bit
void VirtualMachine::resetLessThan()
{
    sr = sr & 0xfffffff7;
} // end resetLessThan method


// setEqualTo method
// Set equal-to bit
void VirtualMachine::setEqualTo()
{
    if ( !( sr & 0x4 ) )
        sr += 4;
} // end setEqualTo method


// getEqualTo method
// Get equal-to bit
int VirtualMachine::getEqualTo()
{
    int temp = sr & 0x4;
    temp = temp >> 2;
    return temp;
} // end getEqualTo method


// resetEqualTo method
// Reset equal-to bit
void VirtualMachine::resetEqualTo()
{
    sr = sr & 0xfffffffb;
} // end resetEqualTo method


// load method
// Load value from memory address to register
// r[RD] = mem[ADDR]
void VirtualMachine::load()
{
	if ( binaryCode.f2.I == 0 )
	{
    	if ( binaryCode.f2.ADDR >= base + limit || binaryCode.f2.ADDR < base )
		{
        	cout << "Address out of range of program\n";
        	exit( 1 );
    	}
	}
    clock += 4;
    
    if ( binaryCode.f2.I == 0 ) 
        reg[binaryCode.f2.RD] = mem[binaryCode.f2.ADDR];
    else
        loadi();
} // end load method


// loadi method
// Load value to register
// r[RD] = CONST
void VirtualMachine::loadi()
{
    clock += 1;
    
    reg[binaryCode.f3.RD] = binaryCode.f3.CONST;
} // end loadi method


// store method
// Store register value to memory address
// mem[ADDR] = r[RD]
void VirtualMachine::store()
{
    if ( binaryCode.f2.ADDR >= base + limit || binaryCode.f2.ADDR < base )
	{
        cout << "Address out of range of program\n";
        exit( 1 );
    }
 
    clock += 4;

    mem[binaryCode.f2.ADDR] = reg[binaryCode.f2.RD];
} // end store method


// add method
// Add two register values
//  r[RD] = r[RD] + r[RS]
void VirtualMachine::add()
{
    clock += 1;
    
    if ( binaryCode.f1.I == 0 ) {
        
        int temp = reg[binaryCode.f1.RD] + reg[binaryCode.f1.RS]; 
        
        if ( reg[binaryCode.f1.RD] >= 0 && reg[binaryCode.f1.RS] >= 0 && temp < 0 )
            setOverflow();

        else if ( reg[binaryCode.f1.RD] < 0 && reg[binaryCode.f1.RS] < 0 && temp >= 0 )
            setOverflow();
            
        reg[binaryCode.f1.RD] = temp;
        setCarryBit(); 
    }

    else
        addi();
} // end add method


// addi method
// Add a value to a register value
//  r[RD] = r[RD] + CONST
void VirtualMachine::addi() 
{
    clock += 1;
     
    int temp = reg[binaryCode.f3.RD] + binaryCode.f3.CONST;
        
    if ( reg[binaryCode.f3.RD] >= 0 && binaryCode.f3.CONST >= 0 && temp < 0 )
        setOverflow();

    else if ( reg[binaryCode.f3.RD] < 0 && binaryCode.f3.CONST < 0 && temp >= 0 )
        setOverflow();
            
    reg[binaryCode.f3.RD] = temp;
    setCarryBit(); 
} // end addi method


// addc method
// Add two register values and carry value
//  r[RD] = r[RD] + r[RS] + CARRY
void VirtualMachine::addc() 
{
    clock += 1;
    
    if ( binaryCode.f1.I == 0 )
	{    
        int carry = getCarryBit();      
        int temp = reg[binaryCode.f1.RD] + reg[binaryCode.f1.RS] + carry; 
        
        if ( reg[binaryCode.f1.RD] >= 0 && reg[binaryCode.f1.RD] >= 0 &&  carry >= 0 && temp < 0 )
            setOverflow();

        else if ( reg[binaryCode.f1.RD] < 0 && reg[binaryCode.f1.RD] < 0 && carry < 0 && temp >= 0 )
            setOverflow();
            
        reg[binaryCode.f1.RD] = temp;
        setCarryBit(); 
    }

    else
        addci();
} // end addc method


// addci method
// Add value to register and carry value
//  r[RD] = r[RD] + CONST + CARRY
void VirtualMachine::addci()
{
    clock += 1;
    
    int carry = getCarryBit();     
    int temp = reg[binaryCode.f3.RD] + binaryCode.f3.CONST + carry;
        
    if ( reg[binaryCode.f3.RD] >= 0 && binaryCode.f3.CONST >= 0 &&  carry >= 0 && temp < 0 )
        setOverflow();

    else if ( reg[binaryCode.f3.RD] < 0 && binaryCode.f3.CONST < 0 && carry < 0 && temp >= 0 )
        setOverflow();
            
    reg[binaryCode.f3.RD] = temp;
    setCarryBit(); 
} // end addci method


// sub method
// Subtract a register value from a register value
//  r[RD] = r[RD] - r[RS]
void VirtualMachine::sub() 
{
    clock += 1;
    
    if ( binaryCode.f1.I == 0 )
	{        
        int temp = reg[binaryCode.f1.RD] - reg[binaryCode.f1.RS];
        int complRS = ~reg[binaryCode.f1.RS] + 1;
        
        if ( reg[binaryCode.f1.RD] >= 0 && complRS >= 0 && temp < 0 )
            setOverflow();

        else if ( reg[binaryCode.f1.RD] < 0 && complRS < 0 && temp >= 0 )
            setOverflow();
            
        reg[binaryCode.f1.RD] = temp;
        setCarryBit(); 
    }

    else
        subi();
} // end sub method


// subi method
// Subtract a value from a register value
//  r[RD] = r[RD] - CONST
void VirtualMachine::subi()
{
    clock += 1;

	int temp = reg[binaryCode.f1.RD] - binaryCode.f3.CONST;
	int complCONST = ~binaryCode.f3.CONST + 1;
        
	if ( reg[binaryCode.f1.RD] >= 0 && complCONST >= 0 && temp < 0 )
		setOverflow();

	else if ( reg[binaryCode.f1.RD] < 0 && complCONST < 0 && temp >= 0 )
		setOverflow();
            
	reg[binaryCode.f1.RD] = temp;
	setCarryBit();
} // end subi method


// subc method
// Subtract a register value and carry value from a register value
//  r[RD] = r[RD] - r[RS] - CARRY
void VirtualMachine::subc() 
{
    clock += 1;
    
    if ( binaryCode.f1.I == 0 )
	{
        int temp = reg[binaryCode.f1.RD] - reg[binaryCode.f1.RS] - getCarryBit();
        int complRS = ~reg[binaryCode.f1.RS] + 1;
        
        if ( reg[binaryCode.f1.RD] >= 0 && complRS >= 0 && temp < 0 )
            setOverflow();

        else if ( reg[binaryCode.f1.RD] < 0 && complRS < 0 && temp >= 0 )
            setOverflow();
            
        reg[binaryCode.f1.RD] = temp;
        setCarryBit(); 
    }

    else
        subci();
} // end subc method


// subci method
// Subtract value and carry value from a register value
// r[RD] = r[RD] - CONST - CARRY
void VirtualMachine::subci() 
{
    clock += 1;

    int temp = reg[binaryCode.f1.RD] - binaryCode.f3.CONST - getCarryBit();
    int complRS = ~binaryCode.f3.CONST + 1;
        
    if ( reg[binaryCode.f1.RD] >= 0 && complRS >= 0 && temp < 0 )
        setOverflow();

    else if ( reg[binaryCode.f1.RD] < 0 && complRS < 0 && temp >= 0 )
        setOverflow();
            
    reg[binaryCode.f1.RD] = temp;
    setCarryBit(); 
} // end subci method


// _and method
// Perform 'and' operation on two register values
// r[RD] = r[RD] & r[RS]
void VirtualMachine::_and()
{
    clock += 1;
    
    if ( binaryCode.f1.I == 0 ) 
        reg[binaryCode.f1.RD] = reg[binaryCode.f1.RD] & reg[binaryCode.f1.RS];  

    else
        andi();
} // end _and method


// andi method
// Perform 'and' operation on a value and register value
// r[RD] = r[RD] & CONST
void VirtualMachine::andi() 
{
    clock += 1;
    reg[binaryCode.f1.RD] = reg[binaryCode.f1.RD] & binaryCode.f3.CONST;
} // end andi method


// _xor method
// Perform 'xor' operation on two register values
// r[RD] = r[RD] ^ r[RS]
void VirtualMachine::_xor() 
{
    clock += 1;
    
    if ( binaryCode.f1.I == 0 )
        reg[binaryCode.f1.RD] = reg[binaryCode.f1.RD] ^ reg[binaryCode.f1.RS]; 

    else
        xori();
} // end _xor method


// xori method
// Perform 'xor' operation on a value and a register value
// r[RD] = r[RD] ^ CONST
void VirtualMachine::xori() 
{
    clock += 1; 
    reg[binaryCode.f1.RD] = reg[binaryCode.f1.RD] ^ binaryCode.f3.CONST;
} // end xori method


// _compl method
// Take complement of a register value
// r[RD] = ~ r[RD]
void VirtualMachine::_compl() 
{
    clock += 1;
    reg[binaryCode.f1.RD] = ~reg[binaryCode.f1.RD];
} // end _compl method


// shl method
// Perform bit-shift left operation on a register value
// r[RD] = r[RD] << 1, shift-in-bit = 0
void VirtualMachine::shl() 
{
    clock += 1;
    reg[binaryCode.f1.RD] = reg[binaryCode.f1.RD] << 1;
    setCarryBit();
} // end shl method


// shla method
// Perform arithmetic bit-shift left operation on a register value
// shl arithmetic  Set CARRY & Sign Ext
void VirtualMachine::shla() 
{
    clock += 1;

    if ( reg[binaryCode.f1.RD] < 0 )
	{
        reg[binaryCode.f1.RD] = reg[binaryCode.f1.RD] << 1; 
        reg[binaryCode.f1.RD] = reg[binaryCode.f1.RD] | 0xffff8000;
    }

    else
       reg[binaryCode.f1.RD] = reg[binaryCode.f1.RD] << 1;
    
    setCarryBit();
} // end shla method


// shr method
// Perform bit-shift right on a register value
// r[RD] = r[RD] >> 1, shift-in-bit = 0
void VirtualMachine::shr() 
{
    clock += 1;
    int temp = reg[binaryCode.f1.RD];
    temp = temp & 0x1;
    
    if ( temp )
        setRightCarryBit();
    
    reg[binaryCode.f1.RD] = reg[binaryCode.f1.RD] >> 1;   
} // end shr method


// shra method
// Perform arithmetic bit-shift right on a register value
// shr arithmetic  Set CARRY & Sign Ext
void VirtualMachine::shra() 
{
    clock += 1;
    int temp = reg[binaryCode.f1.RD];
    temp = temp & 0x1;
    
    if ( temp )
        setRightCarryBit();
     
    if ( reg[binaryCode.f1.RD] < 0 )
	{
        reg[binaryCode.f1.RD] = reg[binaryCode.f1.RD] >> 1; 
        reg[binaryCode.f1.RD] = reg[binaryCode.f1.RD] | 0xffff8000;
    }

    else
       reg[binaryCode.f1.RD] = reg[binaryCode.f1.RD] >> 1;   
} // end shra method


// compr method
// Compare two register values
void VirtualMachine::compr() 
{
    clock += 1;
    
    if ( binaryCode.f1.I == 0 )
	{
        if ( reg[binaryCode.f1.RD] < reg[binaryCode.f1.RS] )
		{
            setLessThan();
            resetEqualTo();
            resetGreaterThan();
        }

        else if ( reg[binaryCode.f1.RD] > reg[binaryCode.f1.RS] )
		{
            setGreaterThan();
            resetEqualTo();
            resetLessThan();
        }

        else
		{
            setEqualTo();
            resetLessThan();
            resetGreaterThan();
        }
    }

    else 
        compri();
} // end compr method


// compri method
// Compare a value with a register value
void VirtualMachine::compri()
{
    clock += 1;
    
    if ( reg[binaryCode.f1.RD] < binaryCode.f3.CONST )
	{
        setLessThan();
        resetEqualTo();
        resetGreaterThan();
    }

    else if ( reg[binaryCode.f1.RD] > binaryCode.f3.CONST )
	{
        setGreaterThan();
        resetEqualTo();
        resetLessThan();
    }

    else if ( reg[binaryCode.f1.RD] == binaryCode.f3.CONST )
	{
        setEqualTo();
        resetLessThan();
        resetGreaterThan();
    }
} // end compri method


// getstat method
// Load status register into register
// r[RD] = SR
void VirtualMachine::getstat() 	
{
    clock += 1;
    reg[binaryCode.f1.RD] = sr;
} // end getstat method


// putstat method
// Load register value into status register
// SR = r[RD]
void VirtualMachine::putstat() 
{
    clock += 1;
    sr = reg[binaryCode.f1.RD];
} // end putstat method


// jump method
// Load memory address into program count
// pc = ADDR
void VirtualMachine::jump() 
{
    if ( binaryCode.f2.ADDR >= base + limit || binaryCode.f2.ADDR < base )
	{
        cout << "Address out of range of program\n";
        exit( 1 );
    }
    
    clock += 1;

    pc = binaryCode.f2.ADDR;
} // end jump method


// jumpl method
// Load memory address into program count if less-than bit is set
// if LESS == 1, pc = ADDR, else do nothing
void VirtualMachine::jumpl() 
{
    if ( binaryCode.f2.ADDR >= base + limit || binaryCode.f2.ADDR < base )
	{
        cout << "Address out of range of program\n";
        exit( 1 );
    }
    
    clock += 1;    
    
    if ( getLessThan() )
        pc = binaryCode.f2.ADDR;
} // end jumpl method


// jumpe method
// Load memory address into program count if equal-to bit is set
// if EQUAL == 1, pc = ADDR, else do nothing
void VirtualMachine::jumpe() 
{
    if ( binaryCode.f2.ADDR >= base + limit || binaryCode.f2.ADDR < base )
	{
        cout << "Address out of range of program\n";
        exit( 1 );
    }
    
    clock += 1; 
    
    if( getEqualTo() ) 
        pc = binaryCode.f2.ADDR;
} // end jumpe method


// jumpg method
// Load memory address into program count if greater-than bit is set
// if GREATER == 1, pc = ADDR, else do nothing
void VirtualMachine::jumpg() 
{
    if ( binaryCode.f2.ADDR >= base + limit || binaryCode.f2.ADDR < base )
	{
        cout << "Address out of range of program\n";
        exit( 1 );
    }

    clock += 1;
    
    if( getGreaterThan() )
        pc = binaryCode.f2.ADDR;
} // end jumpg method


// call method
// Load memory address into program count
// push VM status; pc = ADDR
void VirtualMachine::call() 
{
    clock += 4;
    
    if ( sp < limit + 6 )
	{
        cout << "Stack Full" << endl;
        exit( 1 );
    }
        
    mem[--sp] = sr;    
    mem[--sp] = reg[0];
    mem[--sp] = reg[1];
    mem[--sp] = reg[2];
    mem[--sp] = reg[3];
    mem[--sp] = pc;
   
    pc = binaryCode.f2.ADDR;
} // end call method


// _return method
// Load previous memory address into status register
// pop and restore VM status
void VirtualMachine::_return() 
{
    clock += 4;
    
    pc = mem[sp++];
    reg[3] = mem[sp++];    
    reg[2] = mem[sp++];  
    reg[1] = mem[sp++];  
    reg[0] = mem[sp++];  
    sr = mem[sp++];  
} // end _return method


// read method
// Read file contents into register
// read new content of r[RD] from .in file
void VirtualMachine::read() 
{
    clock += 28;    
    
    fstream oFile;
    oFile.open( inFileName.c_str(), ios::in );
	
    if ( !oFile.is_open() )
	{
        cout << "Failed to open " << inFileName << endl;
        exit( 1 );
    }

    string buffer;
    getline( oFile, buffer );
    oFile.close();
    
    int input = atoi( buffer.c_str() );

    reg[binaryCode.f1.RD] = input;    
} // end read method


// write method
// Write register value to a file
// write r[RD] into .out file
void VirtualMachine::write() 
{
    clock += 28;

    fstream outFile;
    outFile.open( outFileName.c_str(), ios::out );
    
    int temp = reg[binaryCode.f1.RD] & 0x8000;
    
    if ( temp ) 
        temp = reg[binaryCode.f1.RD] | 0xffff0000;

    else
        temp = reg[binaryCode.f1.RD] & 0xffff;
    
   
    outFile << temp << endl;
    outFile.close();
} // end write method


// halt method
// Halt execution
void VirtualMachine::halt()
{
    clock += 1;
    //execute = false;
	exit( 1 );
} // end halt method


// noop method
// No operation
void VirtualMachine::noop()
{
    clock += 1;     
} // end noop method
