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
void Assembler::assemble( string fileName )
{
    string buffer, opcode;
	
    // Set filenames for output and input files
    inputFile = fileName;
    outputFile = fileName.erase( fileName.length() - 2, 2 ) + ".o";

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
void Assembler::write( int binaryCode )
{
    fstream outFile;

	// Open output file
    outFile.open( outputFile.c_str(), ios::in | ios::out | ios::ate );

	// Check if output file opened correctly
    if ( !outFile.is_open() )
	{
        cout << "Failed to open " << outputFile << endl;
        exit( 1 );
    }

	// write instruction code
    outFile << binaryCode << endl;

    outFile.close();
} // end write function


// validateRD function
// Confirm RD value is within range
void Assembler::validateRD( int RD )
{
    if ( RD > 3 || RD < 0 )
	{
        cout << "Invalid RD value" << endl;
        exit( 1 );
    }
} // end validateRD


// validateRS function
// Confirm RS value is within range
void Assembler::validateRS( int RS )
{
    if ( RS > 3 || RS < 0 )
	{
        cout << "Invalid RS value" << endl;
        exit ( 1 );
    }
} // end validateRS


// validateADDR function
// Confirm ADDR value is within range
void Assembler::validateADDR( int ADDR )
{  
    if ( ADDR > 255 || ADDR < 0 )
	{
        cout << "Invalid ADDR value" << endl;
        exit( 1 );
    }
} // end validateADDR


// validateCONST function
// Confirms CONST value is within range
void Assembler::validateCONST( int CONST )
{
    if ( CONST > 127 || CONST < -128 )
	{
        cout << "Invalid CONST value" << endl;
        exit( 1 );
    }
}// end validateCONST


// load function
// Load register value to a register
void Assembler::load( string s )
{
    int RD, ADDR;
    string OPCODE;
    
    istringstream iStream( s.c_str() );  

	// Read in instruction
    iStream >> OPCODE >> RD >> ADDR;
    
	// Confirm RD and ADDR are valid
    validateRD( RD );
    validateADDR( ADDR );
 
	// Insert OPCODE, RD and ADDR
    int binary = 0; // 00000
    binary += RD << 9;
    binary += ADDR;

    write( binary );
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

	// Confirm RD and CONST are valid
    validateRD( RD );
    validateCONST( CONST );
    
	// Insert OPCODE, RD, I and CONST
    int binary = 0; // 00000
    binary += RD << 9;
    binary += I << 8;

	// If CONST is negative, take 2's complement
    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

    binary += CONST;

    write( binary );
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

	// Confirm RD and ADDR are valid
    validateRD( RD );
    validateADDR( ADDR );

	// Insert OPCODE, RD and ADDR
    int binary = 1; // 00001
    binary = binary << 11;
    binary += RD << 9;
    binary += ADDR;

    write( binary );
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

	// Confirm RD and RS are valid
    validateRD(RD);
    validateRS(RS);
    
	// Insert OPCODE, RD, I and RS
    int binary = 2; // 00010
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;
    binary += RS << 6;

    write( binary );
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

	// Confirm RD and CONST are valid
    validateRD( RD );
    validateCONST( CONST );

	// Insert OPCODE, RD, I and CONST
    int binary = 2; // 00010
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if (CONST < 0)
        CONST = CONST & 0x00FF;

    binary += CONST;

    write( binary );
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

	// Confirm RD and RS are valid
    validateRD( RD );
    validateRS( RS );

	// Insert OPCODE, RD, I and RS
    int binary = 3; // 00011
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;
    binary += RS << 6;

    write( binary );
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

	// Confirm RD and CONST are valid
    validateRD( RD );
    validateCONST( CONST );

	// Insert OPCODE, RD, I and CONST
    int binary = 3; // 00011
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

    binary += CONST;

    write( binary );
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

	// Confirm RD and RS are valid
    validateRD( RD );
    validateRS( RS );

	// Insert OPCODE, RD, I and RS
    int binary = 4; // 00100
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;
    binary += RS << 6;

    write( binary );
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

	// Confirm RD and CONST are valid
    validateRD( RD );
    validateCONST( CONST );

	// Insert OPCODE, RD, I and CONST
    int binary = 4; // 00100
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

    binary += CONST;

    write( binary );
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

	// Confirm RD and RS are valid
    validateRD( RD );
    validateRS( RS );

	// Insert OPCODE, RD, I and RS
    int binary = 5; // 00101
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;
    binary += RS << 6;

    write( binary );
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

	// Confirm RD and CONST are valid
    validateRD( RD );
    validateCONST( CONST );

	// Insert OPCODE, RD, I and CONST
    int binary = 5; // 00101
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

    binary += CONST;

    write( binary );
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

	// Confirm RD and RS are valid
    validateRD( RD );
    validateRS( RS );

	// Insert OPCODE, RD, I and RS
    int binary = 6; // 00110
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;
    binary += RS << 6;

    write( binary );
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

	// Confirm RD and CONST are valid
    validateRD( RD );
    validateCONST( CONST );

	// Insert OPCODE, RD, I and CONST
    int binary = 6; // 00110
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

    binary += CONST;

    write( binary );
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

	// Confirm RD and RS are valid
    validateRD( RD );
    validateRS( RS );

	// Insert OPCODE, RD, i and RS
    int binary = 7; // 00111
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;
    binary += RS << 6;

    write( binary );
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

	// Confirm RD and CONST are valid
    validateRD( RD );
    validateCONST( CONST );

	// Insert OPCODE, RD, I and CONST
    int binary = 7; // 00111
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

    binary += CONST;

    write( binary );
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
    validateRD( RD );

	// Insert OPCODE and RD
    int binary = 8; // 01000
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
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
    validateRD( RD );

	// Insert OPCODE and RD
    int binary = 9; // 01001
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
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
    validateRD( RD );

	// Insert OPCODE and RD
    int binary = 10; // 01010
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
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
    validateRD( RD );

	// Insert OPCODE and RD
    int binary = 11; // 01011
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
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
    validateRD( RD );

	// Insert OPCODE and RD
    int binary = 12; // 01100
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
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

	// Confirm RD and RD are valid
    validateRD( RD );
    validateRS( RS );

	// Insert OPCODE, RD and RS
    int binary = 13; // 01101
    binary = binary << 11;
    binary += RD << 9;
    binary += RS << 6;

    write( binary );
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

	// Confirm RD and CONST are valid
    validateRD( RD );
    validateCONST( CONST );

	// Insert OPCODE, RD, I and CONST
    int binary = 13; // 01101
    binary = binary << 11;
    binary += RD << 9;
    binary += I << 8;

    if ( CONST < 0 )
        CONST = CONST & 0x00FF;

    binary += CONST;

    write( binary );
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
    validateRD( RD );

	// Insert OPCODE and RD
    int binary = 14; // 01110
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
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
    validateRD( RD );

	// Insert OPCODE and RD
    int binary = 15; // 01111
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
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
    validateADDR( ADDR );

	// Insert OPCODE and ADDR
    int binary = 16; // 10000
    binary = binary << 11;
    binary += ADDR;

    write( binary );
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
    validateADDR( ADDR );

	// Insert OPCODE and ADDR
    int binary = 17; // 10001
    binary = binary << 11;
    binary += ADDR;

    write( binary );
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
    validateADDR( ADDR );

	// Insert OPCODE and ADDR
    int binary = 18; // 10010
    binary = binary << 11;
    binary += ADDR;

    write( binary );
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
    validateADDR( ADDR );

	// Insert OPCODE and ADDR
    int binary = 19; // 10011
    binary = binary << 11;
    binary += ADDR;

    write( binary );
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
    validateADDR( ADDR );

	// Insert OPCODE and ADDR
    int binary = 20; // 10100
    binary = binary << 11;
    binary += ADDR;

    write( binary );
} // end call function


// _return function
// Pop and restore status
void Assembler::_return( string s )
{
	// Insert OPCODE
    int binary = 21; // 10101
	binary = binary << 11;

    write( binary );
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
    validateRD( RD );

	// Insert OPCODE and RD
    int binary = 22; // 10110
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
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
    validateRD( RD );

	// Insert OPCODE and RD
    int binary = 23; // 10111
    binary = binary << 11;
    binary += RD << 9;

    write( binary );
} // end write function


// halt function
// Halt execution
void Assembler::halt( string s )
{
	// Insert OPCODE
    int binary = 24; // 11000
	binary = binary << 11;

    write( binary );
} // end halt function


// noop function
// No operation
void Assembler::noop( string s )
{
	// Insert OPCODE
    int binary = 25; // 11001
	binary = binary << 11;

    write( binary );
} // end noop function
