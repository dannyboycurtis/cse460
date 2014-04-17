

/********************************************************************************
 * James Small & William Elder
 * VirtualMachine.cpp
 * 4/17/13
 * This program simulates a 16 bit virtual machine.  It reads in machine code 
 * created by the Assembler program, pulls them all into memory, and then executes
 * each instruction one at a time.
 * Modified 5/7/13: Add the ability to run multiple programs at a time.  Main run
 * function was modified to allow this.  Also added the ability to load programs
 * into memory before beginning to run the programs.
 ********************************************************************************/

#include "VirtualMachine.h"

VirtualMachine::VirtualMachine()
{
    reg = vector <int> (REG_FILE_SIZE);
    mem = vector <int> (MEM_SIZE);
    execute = true;
    nextMemoryLocation = 0;
    pageNumberToGet = -1;
    masterClock = 0;

    accessRegisters = vector <int> (32); // for 32 frames

    for (int i = 0;i < accessRegisters.size();i++)
        accessRegisters[i] = -1;
        
        inverted = vector <string> (32); // for 32 frames

    for (int i = 0;i < inverted.size();i++)
        inverted[i] = "NULL";


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
}

int VirtualMachine::loadIntoMemory(string originalFileName, int pageNumber, int frameNumber, int * progBase, int * progLimit)
{
 
    // read into memory, increase limit
    fstream readIntoMemory;
    readIntoMemory.open(originalFileName.c_str(), ios::in);
	
    if (!readIntoMemory.is_open()) {
        cout << inputFileName << " failed to open.\n";
      
    }
    
    nextMemoryLocation = frameNumber * FRAME_SIZE;

    string line;
  
    for (int i = 0; i < pageNumber * FRAME_SIZE;i++) {
        getline(readIntoMemory, line);
        
    }
 /*       
    if (readIntoMemory.eof()) {
        returnCode(2);
        readIntoMemory.close();
         return 1;
    }
 */
    *progBase = nextMemoryLocation;
       
    for (int i = 0; i < FRAME_SIZE; i++)
    {
        if (readIntoMemory.eof()) {

            while (i < FRAME_SIZE) {
                mem[nextMemoryLocation++] = 0;
                i++;
            }

            break;            
        }    

        getline(readIntoMemory, line);
     
        mem[nextMemoryLocation++] = atoi(line.c_str());
        
        int temp = atoi(line.c_str());
        
    }      
    
    *progLimit = 8;
    
    readIntoMemory.close();
     
    inverted[frameNumber] = originalFileName;
     
    if (!lru)
        accessRegisters[frameNumber] = masterClock;
 
     return 0; 
}

void VirtualMachine::copyFromMemory(int frame, list<PCB*>* jobs)
{

    bool needToWriteToDisk = false; 
    
    string updateFileName;

    string process = inverted[frame];
    int page = 0;
    
    inverted[frame] = "NULL";

    if (!strcmp(process.c_str(),originalFileName.c_str()))
    {
        
        for (int i = 0;i < frames.size();i++)
        {
            if (frames[i] == frame) {
                page = i;
          
            break;
            }        
        }

        updateFileName = originalFileName;
            
        if(modified[page] == 1) {
            needToWriteToDisk = true;
            modified[page] = 0;   
        } 

        valid[page] = 0;
           

    }
    else {
        for (list<PCB *>::iterator it = jobs->begin(); it != jobs->end(); it++)
        {
            if(!strcmp((*it)->originalfilename.c_str(), process.c_str()))
            {
                for (int i = 0;i < (*it)->frames.size();i++)
                {
                    if ((*it)->frames[i] == frame) {
                    page = i;
        
                    break;
                    }
                }    
                    
                updateFileName = (*it)->originalfilename;
                        
                       
                if((*it)->modified[page] == 1) {
                    needToWriteToDisk = true;
                    (*it)->modified[page] = 0; 
                }  
          
                (*it)->valid[page] = 0;
          
            } 
        }   

    }
        
    if (needToWriteToDisk)
    {
    // copy back to disk
                  
        fstream readProgs;
        readProgs.open(updateFileName.c_str(), ios::in);
        
        string newFileName = updateFileName + "2";

        fstream newFile;
        newFile.open(newFileName.c_str(), ios::out);
                
        if (!readProgs.is_open()) {
            cout << "Progs failed to open.\n";
        
        }
    
        if (!newFile.is_open()) {
            cout << "Progs failed to open.\n";

        }
             
        string line;
    
        getline(readProgs,line);
    
        int i = 0;
        int count = FRAME_SIZE * page;
        int frameCount = FRAME_SIZE * frame;
                
        while (!readProgs.eof() && i < count)
        {  
            newFile << line << endl;
            getline(readProgs,line);
            i++;
        } 


        for(int j = 0;j < FRAME_SIZE && mem[frameCount] != 0;j++)
        {
            newFile << mem[frameCount] << endl;
            getline(readProgs,line);
            frameCount++;
        }

        while (!readProgs.eof())
        {
            newFile << line << endl;
            getline(readProgs,line);
        }

        readProgs.close();
        newFile.close();  
             
        string removeFile = "rm " + updateFileName;
	
        system(removeFile.c_str());
	
        int result= rename(newFileName.c_str()  , updateFileName.c_str() );
    }       

}

void VirtualMachine::run() {

    clock = 0;
    execute = true;
    sr &= 0xffffff1f; // resets return code to 0
	
    sr = sr & 0xfffffbff; // resets bit 10 to 0
          
    pageNumberToGet = -1;

    int logicalPc = physicalToLogical(pc);
   
    if (!valid[pageNumber(logicalPc)]) {
  
        pageNumberToGet = pageNumber(logicalPc);
        pc = logicalPc;
 
        sr = sr | 0x400;
        
        execute = false;
        
        return;
     }

    // execute loop
    while (execute)
    {
   
        if (pc < base)
            exit(1);

        if (pc >= base + limit) // page fault
        { 
            int pageNum = physicalToLogical(pc - 1);
    
            pageNum = pageNumber(pageNum);
            
            pageNum++;
     
            if (valid[pageNum] == 0) { // page not in memory
                pageNumberToGet = pageNum;
                pc = pageNum * FRAME_SIZE;
    
                sr = sr | 0x400; 
      
                execute = false;
                continue;

            } else { // page in memory
        
                pc = frames[pageNum] * FRAME_SIZE;
           
                base = pc;
            
                if(lru)
                    accessRegisters[frames[pageNumber(physicalToLogical(pc))]] = masterClock;
 
            }

    }
        ir = mem[pc++];
        finalcode.i = ir;
        (this->*instr[finalcode.f1.OP])();
        if(clock >= 15)
            execute = false;
       
    } // end while loop

} // run function

void VirtualMachine::setCarryBit() 
{
    int temp = reg[finalcode.f1.RD] & 0x10000; // get 17th bit only
    temp = temp >> 17; // right shift 17
    
    if (sr & 0x1) // check if sr carry bit is 1, if so, subtract 1 from it to reset to 0
        sr -= 1;
    
    sr += temp; // add current carry value into sr
}

void VirtualMachine::setRightCarryBit()
{
    if (sr & 0x1) // check if sr carry bit is 1, if so, subtract 1 from it to reset to 0
        sr -= 1;
    
    sr += 1; // add current carry value into sr
}

int VirtualMachine::getCarryBit() 
{
    return (sr & 0x1);
}

void VirtualMachine::setOverflow() 
{
    if (!(sr & 0x10))
        sr += 16;
}

void VirtualMachine::setGreaterThan()
{
    if (!(sr & 0x2))
        sr += 2;
}

int VirtualMachine::getGreaterThan()
{
    int temp = sr & 0x2;
    temp = temp >> 1;
    return temp;
}

void VirtualMachine::resetGreaterThan()
{
    sr = sr & 0xfffffffd;
}

void VirtualMachine::setLessThan()
{
    if (!(sr & 0x8))
        sr += 8;
}

int VirtualMachine::getLessThan()
{
    int temp = sr & 0x8;
    temp = temp >> 3;
    return temp;
}

void VirtualMachine::resetLessThan()
{
    sr = sr & 0xfffffff7;
}

void VirtualMachine::setEqualTo()
{
    if (!(sr & 0x4))
        sr += 4;
}

int VirtualMachine::getEqualTo()
{
    int temp = sr & 0x4;
    temp = temp >> 2;
    return temp;
}

void VirtualMachine::resetEqualTo()
{
    sr = sr & 0xfffffffb;
}

void VirtualMachine::load() //  r[RD] = mem[ADDR]
{
    if (finalcode.f2.I == 0) {
    
        if (!isItInMemory(finalcode.f2.ADDR))
        {
             sr = sr | 0x400; 
             pc--;
             pc = physicalToLogical(pc);
             pageNumberToGet = pageNumber(finalcode.f2.ADDR);
             execute = false;
             return;
        }
        
        if (!strcmp(originalFileName.c_str(),"subVector.o")) {
            
            
            
        }

        int temp1 = pageNumber(finalcode.f2.ADDR);
        
        int temp2 = offsetNumber(finalcode.f2.ADDR);
        
        int temp3 = frames[temp1] * FRAME_SIZE + temp2;
        
        clock += 4;

        reg[finalcode.f2.RD] = mem[temp3];    
    }
    else
        loadi();
}

void VirtualMachine::loadi() //  r[RD] = CONST
{
    clock += 1;
    
    reg[finalcode.f3.RD] = finalcode.f3.CONST;   
}

void VirtualMachine::store() //  mem[ADDR] = r[RD]
{
        if (!isItInMemory(finalcode.f2.ADDR))
        {
             sr = sr | 0x400; 
             pc--;
             pc = physicalToLogical(pc);
             pageNumberToGet = pageNumber(finalcode.f2.ADDR);
             execute = false;
             return;
        }

        int temp1 = pageNumber(finalcode.f2.ADDR);
        int temp2 = offsetNumber(finalcode.f2.ADDR);
        
        int temp3 = frames[temp1] * FRAME_SIZE + temp2;
 
        clock += 4;

        mem[temp3] = reg[finalcode.f2.RD];
        
        modified[temp1] = 1;
}

void VirtualMachine::add() //  r[RD] = r[RD] + r[RS]
{
    clock += 1;
    
    if (finalcode.f1.I == 0) {
        
        int temp = reg[finalcode.f1.RD] + reg[finalcode.f1.RS]; 

        
        if (reg[finalcode.f1.RD] >= 0 && reg[finalcode.f1.RS] >= 0 && temp < 0)
            setOverflow();
        else if (reg[finalcode.f1.RD] < 0 && reg[finalcode.f1.RS] < 0 && temp >= 0)
            setOverflow();
            
        reg[finalcode.f1.RD] = temp;
        setCarryBit(); 
        

    }
    else
        addi();
}

void VirtualMachine::addi() //  r[RD] = r[RD] + CONST
{
    clock += 1;
     
    int temp = reg[finalcode.f3.RD] + finalcode.f3.CONST;
        
    if (reg[finalcode.f3.RD] >= 0 && finalcode.f3.CONST >= 0 && temp < 0)
        setOverflow();
    else if (reg[finalcode.f3.RD] < 0 && finalcode.f3.CONST < 0 && temp >= 0)
        setOverflow();
         
    reg[finalcode.f3.RD] = temp;
    setCarryBit(); 
}


void VirtualMachine::addc() //  r[RD] = r[RD] + r[RS] + CARRY
{
    clock += 1;
    
    if (finalcode.f1.I == 0) {
    
        int carry = getCarryBit();      
        int temp = reg[finalcode.f1.RD] + reg[finalcode.f1.RS] + carry; 
        
        if (reg[finalcode.f1.RD] >= 0 && reg[finalcode.f1.RD] >= 0 &&  carry >= 0 && temp < 0)
            setOverflow();
        else if (reg[finalcode.f1.RD] < 0 && reg[finalcode.f1.RD] < 0 && carry < 0 && temp >= 0)
            setOverflow();
            
        reg[finalcode.f1.RD] = temp;
        setCarryBit(); 
    }
    else
        addci();
}

void VirtualMachine::addci() //  r[RD] = r[RD] + CONST + CARRY
{
    clock += 1;
    
    int carry = getCarryBit();     
    int temp = reg[finalcode.f3.RD] + finalcode.f3.CONST + carry;
        
    if (reg[finalcode.f3.RD] >= 0 && finalcode.f3.CONST >= 0 &&  carry >= 0 && temp < 0)
        setOverflow();
    else if (reg[finalcode.f3.RD] < 0 && finalcode.f3.CONST < 0 && carry < 0 && temp >= 0)
        setOverflow();
            
    reg[finalcode.f3.RD] = temp;
    setCarryBit(); 
}

void VirtualMachine::sub() //  r[RD] = r[RD] - r[RS]
{
    clock += 1;
    
    

    if (finalcode.f1.I == 0) {
        
        int temp = reg[finalcode.f1.RD] - reg[finalcode.f1.RS];
        int complRS = ~reg[finalcode.f1.RS] + 1;
        
        if (reg[finalcode.f1.RD] >= 0 && complRS >= 0 && temp < 0)
            setOverflow();
        else if (reg[finalcode.f1.RD] < 0 && complRS < 0 && temp >= 0)
            setOverflow();
            
        reg[finalcode.f1.RD] = temp;
        setCarryBit(); 
    }
    else
        subi();
}

void VirtualMachine::subi() //  r[RD] = r[RD] - CONST
{
    clock += 1;

        int temp = reg[finalcode.f1.RD] - finalcode.f3.CONST;
        int complCONST = ~finalcode.f3.CONST + 1;
        
        if (reg[finalcode.f1.RD] >= 0 && complCONST >= 0 && temp < 0)
            setOverflow();
        else if (reg[finalcode.f1.RD] < 0 && complCONST < 0 && temp >= 0)
            setOverflow();
            
        reg[finalcode.f1.RD] = temp;
        setCarryBit();
}

void VirtualMachine::subc() //  r[RD] = r[RD] - r[RS] - CARRY
{
    clock += 1;
    
    if (finalcode.f1.I == 0) {

        int temp = reg[finalcode.f1.RD] - reg[finalcode.f1.RS] - getCarryBit();
        int complRS = ~reg[finalcode.f1.RS] + 1;
        
        if (reg[finalcode.f1.RD] >= 0 && complRS >= 0 && temp < 0)
            setOverflow();
        else if (reg[finalcode.f1.RD] < 0 && complRS < 0 && temp >= 0)
            setOverflow();
            
        reg[finalcode.f1.RD] = temp;
        setCarryBit(); 
    }
    else
        subci();
}

void VirtualMachine::subci() // r[RD] = r[RD] - CONST - CARRY
{
    clock += 1;

    int temp = reg[finalcode.f1.RD] - finalcode.f3.CONST - getCarryBit();
    int complRS = ~finalcode.f3.CONST + 1;
        
    if (reg[finalcode.f1.RD] >= 0 && complRS >= 0 && temp < 0)
        setOverflow();
    else if (reg[finalcode.f1.RD] < 0 && complRS < 0 && temp >= 0)
        setOverflow();
            
    reg[finalcode.f1.RD] = temp;
    setCarryBit(); 
}

void VirtualMachine::_and() // r[RD] = r[RD] & r[RS]
{
    clock += 1;
    
    if (finalcode.f1.I == 0) 
        reg[finalcode.f1.RD] = reg[finalcode.f1.RD] & reg[finalcode.f1.RS];  
    else
        andi();
}

void VirtualMachine::andi() // r[RD] = r[RD] & CONST
{
    clock += 1;
    reg[finalcode.f1.RD] = reg[finalcode.f1.RD] & finalcode.f3.CONST;
}


void VirtualMachine::_xor() // r[RD] = r[RD] ^ r[RS]
{
    clock += 1;
    
    if (finalcode.f1.I == 0)
        reg[finalcode.f1.RD] = reg[finalcode.f1.RD] ^ reg[finalcode.f1.RS]; 
    else
        xori();
}

void VirtualMachine::xori() // r[RD] = r[RD] ^ CONST
{
    clock += 1; 
    reg[finalcode.f1.RD] = reg[finalcode.f1.RD] ^ finalcode.f3.CONST;
}


void VirtualMachine::_compl() // r[RD] = ~ r[RD]
{
    clock += 1;
    reg[finalcode.f1.RD] = ~reg[finalcode.f1.RD];
}

void VirtualMachine::shl() //  r[RD] = r[RD] << 1, shift-in-bit = 0
{
    clock += 1;
    reg[finalcode.f1.RD] = reg[finalcode.f1.RD] << 1;
    setCarryBit();
}

void VirtualMachine::shla() // shl arithmetic  Set CARRY & Sign Ext
{
    clock += 1;
    if (reg[finalcode.f1.RD] < 0) {
        reg[finalcode.f1.RD] = reg[finalcode.f1.RD] << 1; 
        reg[finalcode.f1.RD] = reg[finalcode.f1.RD] | 0xffff8000;
    }
    else
       reg[finalcode.f1.RD] = reg[finalcode.f1.RD] << 1;
    
    setCarryBit();
}


void VirtualMachine::shr() // r[RD] = r[RD] >> 1, shift-in-bit = 0
{
    clock += 1;
    int temp = reg[finalcode.f1.RD];
    temp = temp & 0x1;
    
    if (temp)
        setRightCarryBit();
    
    reg[finalcode.f1.RD] = reg[finalcode.f1.RD] >> 1;   
}

void VirtualMachine::shra() // shr arithmetic  Set CARRY & Sign Ext
{
    clock += 1;
    int temp = reg[finalcode.f1.RD];
    temp = temp & 0x1;
    
    if (temp)
        setRightCarryBit();
     
    if (reg[finalcode.f1.RD] < 0) {
        reg[finalcode.f1.RD] = reg[finalcode.f1.RD] >> 1; 
        reg[finalcode.f1.RD] = reg[finalcode.f1.RD] | 0xffff8000;
    }
    else
       reg[finalcode.f1.RD] = reg[finalcode.f1.RD] >> 1;   
}

void VirtualMachine::compr() 
{
    clock += 1;
    if (finalcode.f1.I == 0) {
        
        if (reg[finalcode.f1.RD] < reg[finalcode.f1.RS]) {
            setLessThan();
            resetEqualTo();
            resetGreaterThan();
        }
        else if (reg[finalcode.f1.RD] > reg[finalcode.f1.RS]) {
            setGreaterThan();
            resetEqualTo();
            resetLessThan();
        }
        else {
            setEqualTo();
            resetLessThan();
            resetGreaterThan();
        }
    }
    else 
        compri();
}

void VirtualMachine::compri()
{
    clock += 1;
    if (reg[finalcode.f1.RD] < finalcode.f3.CONST) {
        setLessThan();
        resetEqualTo();
        resetGreaterThan();
    }
    else if (reg[finalcode.f1.RD] > finalcode.f3.CONST) {
        setGreaterThan();
        resetEqualTo();
        resetLessThan();
    }
    else if (reg[finalcode.f1.RD] == finalcode.f3.CONST) {
        setEqualTo();
        resetLessThan();
        resetGreaterThan();
    }
}

void VirtualMachine::getstat() 	// r[RD] = SR
{
    clock += 1;
    reg[finalcode.f1.RD] = sr;
}

void VirtualMachine::putstat() // SR = r[RD]
{
    clock += 1;
    sr = reg[finalcode.f1.RD];
}

void VirtualMachine::jump() // pc = ADDR
{
    if (!isItInMemory(finalcode.f2.ADDR))
    {
        sr = sr | 0x400; 
        pc--;
        pc = physicalToLogical(pc);
        pageNumberToGet = pageNumber(finalcode.f2.ADDR);
        execute = false;
        return;
    }

    int temp1 = pageNumber(finalcode.f2.ADDR);
    int temp2 = offsetNumber(finalcode.f2.ADDR);
        
    int temp3 = frames[temp1] * FRAME_SIZE + temp2;
    
    clock += 1;

    pc = temp3;
    base = frames[temp1] * FRAME_SIZE;
}

void VirtualMachine::jumpl() // if LESS == 1, pc = ADDR, else do nothing
{

    if (!isItInMemory(finalcode.f2.ADDR))
    {
        sr = sr | 0x400; 
        pc--;
        pc = physicalToLogical(pc);
        pageNumberToGet = pageNumber(finalcode.f2.ADDR);
        execute = false;
        return;
    }
    int temp1 = pageNumber(finalcode.f2.ADDR);
    int temp2 = offsetNumber(finalcode.f2.ADDR);
    
    int temp3 = frames[temp1] * FRAME_SIZE + temp2;
    
    clock += 1;    
    
    if(getLessThan()) {
        pc = temp3;
        base = frames[temp1] * FRAME_SIZE;
    }
}

void VirtualMachine::jumpe() // if EQUAL == 1, pc = ADDR, else do nothing
{
    if (!isItInMemory(finalcode.f2.ADDR))
    {      
        sr = sr | 0x400; 
        pc--;
        pc = physicalToLogical(pc);
        pageNumberToGet = pageNumber(finalcode.f2.ADDR);
     
        execute = false;
        return;
    }

    int temp1 = pageNumber(finalcode.f2.ADDR);
    int temp2 = offsetNumber(finalcode.f2.ADDR);
        
    int temp3 = frames[temp1] * FRAME_SIZE + temp2;

    if(getEqualTo()) 
      {
        pc = temp3;
        base = frames[temp1] * FRAME_SIZE;        
    }
}

void VirtualMachine::jumpg() // if GREATER == 1, pc = ADDR, else do nothing
{
    if (!isItInMemory(finalcode.f2.ADDR))
    {
        sr = sr | 0x400;
        pc--;
        pc = physicalToLogical(pc);
        pageNumberToGet = pageNumber(finalcode.f2.ADDR);
        execute = false;
        return;
    }

    int temp1 = pageNumber(finalcode.f2.ADDR);
    int temp2 = offsetNumber(finalcode.f2.ADDR);
        
    int temp3 = frames[temp1] * FRAME_SIZE + temp2;
    clock += 1;
    
    if(getGreaterThan())
        {
        pc = temp3;
        base = frames[temp1] * FRAME_SIZE;
    }
}

void VirtualMachine::call() // push VM status; pc = ADDR
{
    if (!isItInMemory(finalcode.f2.ADDR))
    {
        sr = sr | 0x400;
        pc--;
        pc = physicalToLogical(pc);
        pageNumberToGet = pageNumber(finalcode.f2.ADDR);
        execute = false;
        return;
    }
    
    int oldPc = pc;
 
    int temp1 = pageNumber(finalcode.f2.ADDR);
    int temp2 = offsetNumber(finalcode.f2.ADDR);
        
    int temp3 = frames[temp1] * FRAME_SIZE + temp2;

    pc = temp3;
    base = frames[temp1] * FRAME_SIZE;

    clock += 4;

 
    int newSp = sp - 6;

    int frameNumberToOverwrite = pageNumber(newSp);
 
    if (strcmp(inverted[frameNumberToOverwrite].c_str(),"NULL"))
    {

        copyFromMemory(frameNumberToOverwrite,osJobs);
    
    }
    
   
     inverted[frameNumberToOverwrite] = "Stack";
  
        
    mem[--sp] = sr;    
    mem[--sp] = reg[0];
    mem[--sp] = reg[1];
    mem[--sp] = reg[2];
    mem[--sp] = reg[3];
    mem[--sp] = oldPc;
}

void VirtualMachine::_return() // pop and restore VM status
{
    clock += 4;

    if (sp==256){
        returnCode(4);
        execute=false;
        return;
    }
    
    inverted[pageNumber(sp)] = "NULL";
    
    
    
    pc = mem[sp++];
    reg[3] = mem[sp++];
    reg[2] = mem[sp++];
    reg[1] = mem[sp++];
    reg[0] = mem[sp++];
    sr = mem[sp++];  
    
    int temp1 = pageNumber(pc);
    
    base = temp1 * FRAME_SIZE;
  
    
}

void VirtualMachine::read() // read new content of r[RD] from .in file
{
    returnCode(6);
    readOrWrite(finalcode.f1.RD);
    execute = false;
}

void VirtualMachine::write() // write r[RD] into .out file
{
    returnCode(7);
    readOrWrite(finalcode.f1.RD);
    execute = false;
}

void VirtualMachine::halt() // halt execution
{
    int temp1 = pageNumber(pc - 1);


    clock += 1;
    returnCode(1);
    execute = false;
}

void VirtualMachine::noop() // no operation
{
    clock += 1;     
}

void VirtualMachine::returnCode(int i)
{
    sr &= 0xffffff1f; 

    int temp = i << 5;
    
    sr |= temp;
}

void VirtualMachine::readOrWrite(int i)
{
    sr &= 0xfffffcff;

    int temp = i << 8;
    
    sr |= temp;
}

 int VirtualMachine::pageNumber(int addr)
    {
         int page = addr & 0xf8; 
         return page >> 3;
    }
    
int VirtualMachine::offsetNumber(int addr)
    {
        return addr & 0x7; 
        
    }
    
bool VirtualMachine::isItInMemory(int ADDR) 
{
    int temp = pageNumber(ADDR);
    
    if(valid[temp] == 1)
        return true;
    
    
    return false;    


}

int VirtualMachine::physicalToLogical(int ADDR)
{
    int frame = pageNumber(ADDR);
    
    int offset = offsetNumber(ADDR);
    int page = 0;
    
    for (int i = 0;i < frames.size();i++)
    {
        if (frames[i] == frame) {
            page = i;
            break;
        }        
    }
   
    page = page << 3;
    page = page + offset;

    
    return page;
}
    
    
    
    
    
    
    
    
    
    
    
    
    
