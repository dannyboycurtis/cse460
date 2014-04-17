/********************************************************************************
* Filename: Assembler.cpp
* Author: Daniel Urbach
* Date: 4/16/2014
* Description: This program translates assembly instructions into binary
********************************************************************************/

#include "Assembler.h"


// Assembler constructor
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
} // end Assembler constructor



// assemble function
// Convert assembly code to binary
void Assembler::assemble( string filename )
{
    string line, opcode;
	
    // Set filenames for output and input files
    inputFile = filename;
    outputFile = filename.erase( filename.length() - 2, 2 ) + ".o";

    fstream output;
    
	// Create .o file for output
    output.open( outputFile.c_str(), ios::out );
    output.close();

	fstream assemblyCode;

    // Open .s file for input	
    assemblyCode.open( inputFile.c_str(), ios::in );
	
	// Check if file can be opened
    if ( !assemblyCode.is_open() )
	{
        cout << "Failed to open " << inputFile << endl;
        exit( 1 );
    }

	// Load initial line of assembly into buffer
    getline( assemblyCode, buffer );
    
	// Iterate through assembly code and call relevant function
    while ( !assemblyCode.eof() )
	{
		// Check if line is a comment
        if ( buffer[0] == '!' )
		{
            getline( assemblyCode, buffer );
            continue;
        }

        istringstream str( buffer.c_str() );

        try
		{
            str >> opcode;

            if ( not instr[opcode] )
			    throw NullPointerException();
			else 
                ( this->*instr[opcode] )( buffer.c_str() );
		}

		catch ( NullPointerException e )
		{
			cerr << e.what() << endl;
			exit( 1 );
		}

        getline( assemblyCode, buffer );
    }

    assemblyCode.close();
} // end assemble function



// write function
// Write binary source code to file
void Assembler::write( int sourceCode )
{
    fstream outFile;

	// Open output file
    outFile.open( outputFile.c_str(), ios::in | ios::out | ios::ate );

	// Check if output file opened correctly
    if ( !outFile.is_open() )
	{
        cout << "Failed to open " << outputFile << endl;
        exit(1);
    }

	// write instruction code
    outFile << sourceCode << endl;

    outFile.close();
} // end write function



// checkRD function
// Confirm RD value is within range
void Assembler::checkRD( int RD )
{
    if ( RD > 3 || RD < 0 )
	{
        cout << "Invalid RD value" << endl;
        exit( 1 );
    }
} // end checkRD



// checkRS function
// Confirm RS value is within range
void Assembler::checkRS( int RS )
{
    if ( RS > 3 || RS < 0 )
	{
        cout << "Invalid RS value" << endl;
        exit (1 );
    }
} // end checkRS



// checkADDR function
// Confirm ADDR value is within range
void Assembler::checkADDR( int ADDR )
{  
    if ( ADDR > 255 || ADDR < 0 )
	{
        cout << "Invalid ADDR value" << endl;
        exit( 1 );
    }
} // end checkADDR



// checkCONST function
// Confirms CONST value is within range
void Assembler::checkCONST( int CONST )
{
    if ( CONST > 127 || CONST < -128 )
	{
        cout << "Invalid CONST value" << endl;
        exit( 1 );
    }
}// end checkCONST



// load function
// Load register value to a register
void Assembler::load( string s )
{
    int RD, ADDR;
    string OPCODE;
    
    istringstream iStream( s.c_str() );  

	// Read in instruction
    iStream >> OPCODE >> RD >> ADDR;
    
	// Confirm RD is valid
    checkRD( RD );

	// Confirm ADDR is valid
    checkADDR( ADDR );
 
	// Insert OPCODE 00000
    int num = 0;

	// Insert RD
    num += RD << 9;

	// Insert ADDR
    num += ADDR;

    write( num );
} // end load function



// loadi function
// Load a value into a register
void Assembler::loadi( string s )
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> CONST;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm CONST is valid
    checkCONST( CONST );
    
	// Insert OPCODE 00000
    int num = 0;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// If CONST is negative, take 2's complement
    if ( CONST < 0 )
        CONST = CONST & 0x00FF;
    
	// Insert CONST
    num += CONST;

    write( num );
} // end loadi function



// store function
// Store a value to an address
void Assembler::store( string s )
{
    int RD, ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );        

	// Read in instruction
    iStream >> OPCODE >> RD >> ADDR;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm ADDR is valid
    checkADDR( ADDR );

	// Insert OPCODE 00001
    int num = 1;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert ADDR
    num += ADDR;

    write( num );
} // end store function



// add function
// Add two register values  
void Assembler::add( string s )
{
    int RD, RS, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> RS;

	// Confirm RD is valid
    checkRD(RD);

	// Confirm RS is valid
    checkRS(RS);
    
	// Insert OPCODE 00010
    int num = 2;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// Insert RS
    num += RS << 6;

    write(num);
} // end add function



// addi function
// Add value to register value
void Assembler::addi( string s )
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> CONST;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm CONST is valid
    checkCONST( CONST );

	// Insert OPCODE 00010
    int num = 2;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;
    
	// If CONST is negative, take 2's complement	
    if (CONST < 0)
        CONST = CONST & 0x00FF;

	// Insert CONST
    num += CONST;

    write( num );
} // end addi function



// addc function
// Add two register values and carry value
void Assembler::addc( string s )
{
    int RD, RS, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> RS;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm RS is valid
    checkRS( RS );

	// Insert OPCODE 00011
    int num = 3;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// Insert RS
    num += RS << 6;

    write( num );
} // end addc function



// addci function
// Add value to register value and carry value
void Assembler::addci( string s )
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> CONST;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm CONST is valid
    checkCONST( CONST );

	// Insert OPCODE 00011
    int num = 3;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// If CONST is negative, take 2's complement
    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

	// Insert CONST
    num += CONST;

    write( num );
} // end addci function



// sub function
// Subtract a register value from another register value
void Assembler::sub( string s )
{
    int RD, RS, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> RS;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm RS is valid
    checkRS( RS );

	// Insert OPCODE 00100
    int num = 4;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// Insert RS
    num += RS << 6;

    write( num );
} // end sub function



// subi function
// Subtract a value from a register value
void Assembler::subi( string s )
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> CONST;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm CONST is valid
    checkCONST( CONST );

	// Insert OPCODE 00100
    int num = 4;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// If CONST is negative, take 2's complement
    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

	// Insert CONST
    num += CONST;

    write( num );
} // end subi function



// subc function
// Subtract a register value from another with carry value
void Assembler::subc( string s )
{
    int RD, RS, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> RS;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm RS is valid
    checkRS( RS );

	// Insert OPCODE 00101
    int num = 5;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// Insert RS
    num += RS << 6;

    write( num );
} // end subc function



// subci function
// Subtract a value from a register value with carry value
void Assembler::subci( string s )
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> CONST;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm CONST is valid
    checkCONST( CONST );

	// Insert OPCODE 00101
    int num = 5;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// If CONST is negative, take 2's complement
    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

	// Insert CONST
    num += CONST;

    write( num );
} // end subci function



// _and function
// Perform 'and' operation on two register values
void Assembler::_and( string s )
{
    int RD, RS, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> RS;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm RS is valid
    checkRS( RS );

	// Insert OPCODE 00110
    int num = 6;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// Insert RS
    num += RS << 6;

    write( num );
} // end _and function



// andi function
// Perform 'and' operation on a value and a register value
void Assembler::andi( string s )
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> CONST;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm CONST is valid
    checkCONST( CONST );

	// Insert OPCODE 00110
    int num = 6;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// If CONST is negative, take 2's complement
    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

	// Insert CONST
    num += CONST;

    write( num );
} // end andi function



// _xor function
// Perform 'exclusive or' operation on two register values
void Assembler::_xor( string s )
{
    int RD, RS, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> RS;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm RS is valid
    checkRS( RS );

	// Insert OPCODE 00111
    int num = 7;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// Insert RS
    num += RS << 6;

    write( num );
} // end _xor function



// xori function
// Perform 'exclusive or' operation on a value and a register value
void Assembler::xori( string s )
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> CONST;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm CONST is valid
    checkCONST( CONST );

	// Insert OPCODE 00111
    int num = 7;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// If CONST is negative, take 2's complement
    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

	// Insert CONST
    num += CONST;

    write( num );
} // end xori function



// _compl function
// Take the complement of a register value
void Assembler::_compl( string s )
{
    int RD, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD;

	// Confirm RD is valid
    checkRD( RD );

	// Insert OPCODE 01000
    int num = 8;
    num = num << 11;

	// Insert RD
    num += RD << 9;

    write( num );
} // end _compl function



// shl function
// Bit-shift left a register value
void Assembler::shl( string s )
{
    int RD, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD;

	// Confirm RD is valid
    checkRD( RD );

	// Insert OPCODE 01001
    int num = 9;
    num = num << 11;

	// Insert RD
    num += RD << 9;

    write( num );
} // end shl function



// shla function
// Arithmetic bit-shift left a register value
void Assembler::shla( string s )
{
    int RD, I = 0;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD;

	// Confirm RD is valid
    checkRD( RD );

	// Insert OPCODE 01010
    int num = 10;
    num = num << 11;

	// Insert RD
    num += RD << 9;

    write( num );
} // end shal function



// shr function
// Bit-shift right a register value
void Assembler::shr( string s )
{
    int RD;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD;

	// Confirm RD is valid
    checkRD( RD );

	// Insert OPCODE 01011
    int num = 11;
    num = num << 11;

	// Insert RD
    num += RD << 9;

    write( num );
} // end shr function



// shra function
// Arithmetic bit-shift right a register value
void Assembler::shra( string s )
{
    int RD;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD;

	// Confirm RD is valid
    checkRD( RD );

	// Insert OPCODE 01100
    int num = 12;
    num = num << 11;

	// Insert RD
    num += RD << 9;

    write( num );
} // end shra function



// compr function
// Compare two register values
void Assembler::compr( string s )
{
    int RD, RS;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> RS;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm RS is valid
    checkRS( RS );

	// Insert OPCODE 01101
    int num = 13;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert RS
    num += RS << 6;

    write( num );
} // end compr function



// compri function
// Compare a value and a register value
void Assembler::compri( string s )
{
    int RD, CONST, I = 1;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD >> CONST;

	// Confirm RD is valid
    checkRD( RD );

	// Confirm CONST is valid
    checkCONST( CONST );

	// Insert OPCODE 01101
    int num = 13;
    num = num << 11;

	// Insert RD
    num += RD << 9;

	// Insert I
    num += I << 8;

	// If CONST is negative, take 2's complement
    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

	// Insert CONST
    num += CONST;

    write( num );
} // end compri function



// getstat function
// Put value of SR into register
void Assembler::getstat( string s )
{
    int RD;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD;

	// Confirm RD is valid
    checkRD( RD );

	// Insert OPCODE 01110
    int num = 14;
    num = num << 11;

	// Insert RD
    num += RD << 9;

    write( num );
} // end getstat function



// putstat function
// Put register value into SR
void Assembler::putstat( string s )
{
    int RD;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD;

	// Confirm RD is valid
    checkRD( RD );

	// Insert OPCODE 01111
    int num = 15;
    num = num << 11;

	// Insert RD
    num += RD << 9;

    write( num );
} // end putstat function



// jump function
// Perform 'jump' operation to ADDR
void Assembler::jump( string s )
{
    int ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> ADDR;

	// Confirm ADDR is valid
    checkADDR( ADDR );

	// Insert OPCODE 10000
    int num = 16;
    num = num << 11;

	// Insert ADDR
    num += ADDR;

    write( num );
} // end jump function



// jumpl function
// Perform 'jump' operation to ADDR if LESS flag set
void Assembler::jumpl( string s )
{
    int ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> ADDR;

	// Confirm ADDR is valid
    checkADDR( ADDR );

	// Insert OPCODE 10001
    int num = 17;
    num = num << 11;

	// Insert ADDR
    num += ADDR;

    write( num );
} // end jumpl function



// jumpe function
// Perform 'jump' operation to ADDR if EQUAL flag set
void Assembler::jumpe( string s )
{
    int ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> ADDR;

	// Confirm ADDR is valid
    checkADDR( ADDR );

	// Insert OPCODE 10010
    int num = 18;
    num = num << 11;

	// Insert ADDR
    num += ADDR;

    write( num );
} // end jumpe function



// jumpg function
// Perform 'jump' operation to ADDR if GREATER flag set
void Assembler::jumpg( string s )
{
    int ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> ADDR;

	// Confirm ADDR is valid
    checkADDR( ADDR );

	// Insert OPCODE 10011
    int num = 19;
    num = num << 11;

	// Insert ADDR
    num += ADDR;

    write( num );
} // end jumpg function



// call function
// Put ADDR in PC
void Assembler::call( string s )
{
    int ADDR;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> ADDR;

	// Confirm ADDR is valid
    checkADDR( ADDR );

	// Insert OPCODE 10100
    int num = 20;
    num = num << 11;

	// Insert ADDR
    num += ADDR;

    write( num );
} // end call function



// _return function
// Pop and restore status
void Assembler::_return( string s )
{
	// Insert OPCODE 10101
    int num = 21;
	num = num << 11;

    write( num );
} // end _return function



// read function
// Put value from .in file into register
void Assembler::read( string s )
{
    int RD;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD;

	// Confirm RD is valid
    checkRD( RD );

	// Insert OPCODE 10110
    int num = 22;
    num = num << 11;

	// Insert RD
    num += RD << 9;

    write( num );
} // end read function



// write function
// Put value from register into .out file
void Assembler::write( string s )
{
    int RD;
    string OPCODE;

    istringstream iStream( s.c_str() );

	// Read in instruction
    iStream >> OPCODE >> RD;

	// Confirm RD is valid
    checkRD( RD );

	// Insert OPCODE 10111
    int num = 23;
    num = num << 11;

	// Insert RD
    num += RD << 9;

    write( num );
} // end write function



// halt function
// Halt execution
void Assembler::halt( string s )
{
	// Insert OPCODE 11000
    int num = 24;
	num = num << 11;

    write( num );
} // end halt function



// noop function
// No operation
void Assembler::noop( string s )
{
	// Insert OPCODE 11001
    int num = 25;
	num = num << 11;

    write( num );
} // end noop function

