/********************************************************************************
 * James Small & William Elder
 * OS.h
 * 6/7/13
 * This program is the main OS class that uses an assembler and virtual machine
 * objects to run our programs.  This class takes in the program files, converts
 * them to object code, loads them all into memory, and then proceeds to run them
 * all using context switches to allow all of the programs to run concurrently,
 * allowing each program to have access to the cpu at some point.
 ********************************************************************************/

#include "OS.h"


OS::OS() 
{
    contextSwitchClock = idleClock = pageReplaced = 0;
    MULTIPROGCAP = 5;
    
    for(int i = 0;i < 32; i++)
        unusedFrames.push_back(i);
        
    lessThanMultiProgCap = false;
}

OS::~OS()
{
    readProgs.close();
    system("rm progs");
}

void OS::start(int lruValue) 
{

/*******************************************************************************************************
update to take a string that determines if lru or fifo.  Based on that update, master variable in VM
that controls if lru or fifo

******************************************************************************************************/
/******************************************************************************************************/

    if (lruValue == 1)
        vm.lru = true;
    else
        vm.lru = false;
   
 // read in all s files
 
    system("ls *.s > progs");
     
    
    readProgs.open("progs", ios::in);
	
    if (!readProgs.is_open()) {
        cout << "Progs failed to open.\n";
        exit(1);
    }

    string line;
    
    for (int i = 0; i < MULTIPROGCAP && !readProgs.eof(); i++)
    {
        getline(readProgs, line);  
        
        if (readProgs.eof()) {
            lessThanMultiProgCap = true;
            break;
        }
            
        readIn(line);

    } // end reading into memory


    // copy first item in readyQ to running , load pcb data to vm  

    running = readyQ.front();
    readyQ.pop();
    givePCBToVM();
    
    int code = 0; 
    
    while (!done()) // main process loop
    {
        if (lessThanMultiProgCap) {
            
            getline(readProgs, line); 
            
            if (!readProgs.eof()) {
                lessThanMultiProgCap = false;
                readIn(line);  
            }       
        }     
    
        code = 0;
   
        vm.run(); // runs current proceess in vm, returns error code stored for later
        
        // get return code from status register
        code = vm.sr;    
        code = code & 0xe0;  
        code >>= 5;          
        
        // update clocks for context switch
        vm.masterClock += 5;
        contextSwitchClock += 5;
        
        // update all processes that aren't terminated with addition to context switch time
        for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++)
            if(!strcmp((*it)->state.c_str(), "terminated"))
                (*it)->contextSwitchTime += 5;

        // First item in main loop
    
        // move items from waitQ into readyQ if interrupt time hit
        if(!waitQ.empty()) {
            while (waitQ.front()->interruptTime <= vm.masterClock) {
                PCB * temp = waitQ.front();
                waitQ.pop();
                readyQ.push(temp); 
                temp->state = "ready";      
                if(waitQ.empty())
                    break;     
            }
        }

        // Second item in main loop
        

        // Update waiting time for all items in ready queue with latest cycle time from vm.run()
        for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++)
            if(strcmp((*it)->state.c_str(), "ready"))
                (*it)->waitingTime += vm.clock; 
            
        if(!(running == NULL)) {
    
            // update master clock and cpu time
            vm.masterClock += vm.clock;
            running->cpuTime += vm.clock;

            // main switch to handle return code, if terminated set, output errorcode.
            switch (code) {
                case 0:
                {         
        
                    int bit10 = vm.sr & 0x400;
        
                    bit10 = bit10 >> 10;
        
                    int frameNumber;
    
                    if (bit10 == 0) // time slice
                    {
                        getPCBFromVM();
                        readyQ.push(running); 
        
                        }
                    else // page fault
                    {
                        if (unusedFrames.empty())
                            replaceAlgorithm();
                
                        frameNumber = unusedFrames.front();
                        unusedFrames.pop_front();
           
        
                        int baseNew = 0;
                        int limitNew = 0;
            
                        if (vm.pageNumberToGet == -1) {
          
                            exit(1);
                        }

                        int temp = vm.loadIntoMemory(running->originalfilename, vm.pageNumberToGet, frameNumber,  &baseNew, &limitNew);
 
                        if (temp == 1) {
                            running->state = "terminated";
                            outputErrorCode("Out-of-Bound Reference");
                            lessThanMultiProgCap = true;
                        } else {
 
                            vm.frames[vm.pageNumberToGet] = frameNumber;
                            vm.valid[vm.pageNumberToGet] = 1;
 
                
                            // pc = next logical address
            
                            vm.base = vm.frames[vm.pageNumber(vm.pc)] * vm.FRAME_SIZE;
            
                            vm.pc = vm.frames[vm.pageNumber(vm.pc)] * vm.FRAME_SIZE + vm.offsetNumber(vm.pc);
 
                            waitQ.push(running);
                            running->state = "waiting";
                            getPCBFromVM();
                            running->interruptTime = vm.masterClock + 35;
             
                        }
      
                    }
           
                    break;   
                }        
                case 1:
                    running->state = "terminated";
                    lessThanMultiProgCap = true;
                    break;           
                case 2:
                    running->state = "terminated";
                    outputErrorCode("Out-of-Bound Reference");
                    lessThanMultiProgCap = true;
                    break;           
                case 3:
                    running->state = "terminated";           
                    outputErrorCode("Stack Overflow");
                    lessThanMultiProgCap = true;
                    break;      
                case 4:
                    running->state = "terminated";
                    outputErrorCode("Stack Underflow");
                    lessThanMultiProgCap = true;
                    break;
                case 5:
                    running->state = "terminated";
                    outputErrorCode("Invalid OPCode");
                    lessThanMultiProgCap = true;
                    break;
                case 6:
                {          
                    int readIn;
               
                    running->readIntoRegister >> readIn;

                    int readReg = vm.sr;
 
                    readReg &= 0x300;         
                    readReg >>= 8; 
                     
                    if(readReg < 0 || readReg > 3) {
                        outputErrorCode("Invalid IO Register");
                        break;
                    }
                
                    vm.reg[readReg] = readIn; 
      
                    vm.masterClock++;
                    running->ioTime += 27;
                    running->cpuTime++;  
                    waitQ.push(running);
                    running->state = "waiting";
                    getPCBFromVM(); 
                    running->interruptTime = vm.masterClock + 28;
               
                    break;
                }
            
                case 7:
                {        
                    int writeToReg = vm.sr;
           
                    writeToReg &= 0x300;   
                
                    writeToReg >>= 8;   
                
                    int temp = vm.reg[writeToReg] & 0x8000;
      
                    if (temp) 
                        temp = vm.reg[writeToReg] | 0xffff0000;
                    else
                        temp = vm.reg[writeToReg] & 0xffff;
                        
           
     
                    running->writeToRegister << temp << endl;
                
                    vm.masterClock++;
                    running->ioTime += 27;
                    running->cpuTime++;                   
               
                    waitQ.push(running);
                    running->state = "waiting";
                    getPCBFromVM();
                    running->interruptTime = vm.masterClock + 28;

                    break;
                }
                
                default:
                    outputErrorCode("Invalid Return Code");
                    running->state = "terminated";
                    lessThanMultiProgCap = true;
                    break;
            }
        
            running = NULL;
 
        } // end if running loop
        

        // Third item in main loop
   
        // if all items in waitQ, update vm.masterClock to get first item out of waitQ
        if(readyQ.empty() && !waitQ.empty()) {

            int temp = waitQ.front()->interruptTime - vm.masterClock;
    
            vm.masterClock += temp;
            idleClock += temp;
            readyQ.push(waitQ.front());
            waitQ.front()->state = "ready";
            waitQ.pop(); 
        
            // update idle time for all non terminated process in jobs list
            for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++)
                if(!strcmp((*it)->state.c_str(), "terminated"))
                    (*it)->idleTime += temp;      
        }

        // move next process from readyQ into running state, copy over pcb contents
        if(!readyQ.empty()) {
            running = readyQ.front();
            running->state = "running";
            readyQ.pop();
            givePCBToVM();
        }  
    
    } // end main process loop
   
    // run program accounting data

    // calculate turnaround time for all processes
    for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++)
        (*it)->turnaroundTime = (*it)->cpuTime + (*it)->waitingTime + (*it)->ioTime + (*it)->idleTime + (*it)->contextSwitchTime;
    
    // run system accounting data
        
    int systemTime = contextSwitchClock + idleClock; // calculate system time
         
    double floatingPointMasterClock = vm.masterClock;  // calculate CPU Utilization
        
    double cpuUtilization = (floatingPointMasterClock - idleClock) / floatingPointMasterClock;  // calculate CPU Utilization
                  
    int allCpuTime = 0;
           
    // iterate through all processes to add cpuTime
    for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++)
        allCpuTime += (*it)->cpuTime;    
            
    double userCpuUtilization = allCpuTime / floatingPointMasterClock; // calculate user cpu ulitizaton.
                   
    double throughPut = jobs.size() / (floatingPointMasterClock / 10000.0);  // calculate throughput
    
    
/*******************************************************************************************************
calculate hit ratio and add to output for each file below

******************************************************************************************************/
        
    // iterate through pcb list and output throughput and vm utilization to all files
        
     for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++) {
             
        fstream writeAccounting;
        writeAccounting.open((*it)->writefilename.c_str(), ios::in | ios::out | ios::ate);
	
        if (!writeAccounting.is_open()) {
            cout << (*it)->writefilename << " failed to open.\n";
           
            exit(1);
        }
            
        writeAccounting << "\nProcess Specific Accounting Data\n\n";
        writeAccounting << "CPU Time = " << (*it)->cpuTime << " cycles\n";
        writeAccounting << "Waiting Time = " << (*it)->waitingTime << " cycles\n";
        writeAccounting << "Turnaround Time = " << (*it)->turnaroundTime << " cycles\n";
        writeAccounting << "IO Time = " << (*it)->ioTime << " cycles\n";
        writeAccounting << "Largest Stack Size = " << (*it)->largestStackSize << endl;
          
        writeAccounting << "\nSystem Specific Accounting Data\n\n";            
        writeAccounting << "System Time = " << systemTime << " cycles\n";
        writeAccounting << "System CPU Utilization = " << cpuUtilization * 100 << "%\n";
        writeAccounting << "User CPU Utilization = " << userCpuUtilization * 100 << "%\n";
        writeAccounting << "ThroughPut = " << throughPut << " processes per second\n";

        writeAccounting.close();
    }
        
   
    // make sure all .st files are deleted.
   
    system("rm *.st");
    
    

}

void OS::givePCBToVM() 
{
    //copy contents of PCB running into VM, including stack information
    
    vm.pc = running->pc;
    vm.sr = running->sr;
    vm.sp = running->sp;
    vm.base = running->base;
    vm.limit = running->limit;
    
    vm.inputFileName = running->readfilename;
    vm.originalFileName = running->originalfilename;
    
    for (int i = 0;i <= vm.REG_FILE_SIZE - 1;i++)
        vm.reg[i] = running->reg[i];
    

    
    if (running->sp < 256) {
        
        int spTemp = running->sp;
        
        while (spTemp < 256)
        {
            int frameNumberToOverwrite = vm.pageNumber(spTemp);
            
            
            vm.copyFromMemory(frameNumberToOverwrite,&jobs);
            
            spTemp += 6;
            
        }
        
        
        
        
        
        fstream readProgs;
        readProgs.open(running->stfilename.c_str(), ios::in);
	
        if (!readProgs.is_open()) {
            cout << running->stfilename << " failed to open.\n";
            exit(1);
        }
    
        int stackPointer = running->sp;
        
        string line;
        getline(readProgs, line);
    
        while (!readProgs.eof()) {
            vm.mem[stackPointer++] = atoi(line.c_str());
            getline(readProgs, line);
        } 
    }
    
    // copy to TLB
    
    vm.frames = running->frames;
    vm.valid = running->valid;
    vm.modified = running->modified;
    
    if(vm.lru)
        vm.accessRegisters[vm.frames[vm.pageNumber(vm.physicalToLogical(vm.pc))]] = vm.masterClock;
    
}

void OS::getPCBFromVM()
{
    //copy contents of VM into running PCB, including stack information
     
    running->pc= vm.pc; 
    running->sr= vm.sr; 
    running->sp=vm.sp;
    running->base= vm.base;
    running->limit= vm.limit;
    
    vm.osJobs = &jobs;
    
    for (int i = 0;i <= vm.REG_FILE_SIZE - 1;i++) {
        running->reg[i] = vm.reg[i];
    }
    
    
    
    if (running->sp < 256) {
        
        int spTemp = running->sp;
        
        while (spTemp < 256)
        {
            vm.inverted[vm.pageNumber(spTemp)] = "NULL";
            
            
            
            
            spTemp += 6;
            
        }

    }
    
        
    if(running->sp < 256)
    {
        fstream readProgs;
        readProgs.open(running->stfilename.c_str(), ios::out);
	
        if (!readProgs.is_open()) {
        
            cout << running->stfilename << " failed to open.\n";
          
            exit(1);
        }
    
        int stackPointer = running->sp;
        
        int temp = 256 - stackPointer;
        
        if(running->largestStackSize < temp)
            running->largestStackSize = temp;
    
        while (stackPointer<256) {
             readProgs<<vm.mem[stackPointer++]<<endl;
        } 
   }
   
   // TLB to PCB
    
    running->frames = vm.frames;
    running->valid = vm.valid;
    running->modified = vm.modified; 
}

void OS::outputErrorCode(string s)
{
    fstream output;
    output.open(running->writefilename.c_str(), ios::in | ios::out | ios::ate);
   
    output << "\nError Code = " << s << endl << endl;
    output.close();
}

// check if all processes have been terminated
bool OS::done() 
{
    if(readyQ.empty() && waitQ.empty() && !running)
        return true;
    
    return false;
}

void OS::readIn(string line)
{
        as.assemble(line); // assemble each file
    
        //create PCB for each file, put in jobs list, and put in ready queue
    
        PCB * p = new PCB;
        jobs.push_back(p);
        readyQ.push(p);
        p->state = "ready";
        
        string fileName = line.erase(line.length() -2, 2);
        
        // set file names for input and output
        p->originalfilename = fileName + ".o";          
        p->readfilename = fileName + ".in";           
        p->writefilename = fileName + ".out";       
        p->stfilename =  fileName + ".st";        

        // Open input and output file streams in PCB
        p->openReadInFileStream();
        p->openWriteOutFileStream();
        
        //read into memory, modify PCB with values for base and limit
    
        int base, limit;
        
        //calculate page and frame number to use

        if (unusedFrames.empty())
                replaceAlgorithm();
   
        int frameNumber = unusedFrames.front();
        unusedFrames.pop_front();
        
        int pageNum = vm.pageNumber(p->pc);  

        int temp = vm.loadIntoMemory(p->originalfilename, pageNum, frameNumber, &base, &limit);
    
        p->base = base;
        p->limit = limit;
        
        //set page table
        
        p->frames[pageNum] = frameNumber;
        p->valid[pageNum] = 1;
        
        p->pc = frameNumber * vm.FRAME_SIZE + vm.offsetNumber(p->pc);
        
}

void OS::replaceAlgorithm()
{

    int oldest = 0;
    
    if (!strcmp(vm.inverted[oldest].c_str(),"Stack"))
        oldest = 1;
 
    for (int i = oldest + 1; i < vm.accessRegisters.size();i++)
    {

        if (vm.accessRegisters[i] < vm.accessRegisters[oldest])
        {
            if (strcmp(vm.inverted[i].c_str(),"Stack"))
                oldest = i;
            
        }
    }
    
    
    
    vm.copyFromMemory(oldest,&jobs);

    unusedFrames.push_back(oldest);

    pageReplaced++;
    
    
}

// main to run OS
int main(int argc, char * argv[])
{
/*******************************************************************************************************
update to allow the passing in of a variable to determine lru or fifo and pass to .start() function.

******************************************************************************************************/
    OS os;
    
    if (argv[1] == NULL) {
        cout << "Invalid Switch Used.  -lru or -fifo only\n\n";
	exit(1);
    }

    string lruValue = argv[1];
    if (strcmp(lruValue.c_str(), "-lru") == 0) { 
	os.start(1);
    }
    else if (strcmp(lruValue.c_str(), "-fifo") == 0) {
	os.start(0);
    }
    else {
	cout << "Invalid Switch Used.  -lru or -fifo\n\n";
    }
}






