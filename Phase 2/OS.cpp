/********************************************************************************
 * James Small & William Elder
 * OS.h
 * 5/7/13
 * This program is the main OS class that uses an assembler and virtual machine
 * objects to run our programs.  This class takes in the program files, converts
 * them to object code, loads them all into memory, and then proceeds to run them
 * all using context switches to allow all of the programs to run concurrently,
 * allowing each program to have access to the cpu at some point.
 ********************************************************************************/

#include "OS.h"


OS::OS() 
{
    masterClock = contextSwitchClock = idleClock = 0;
}

void OS::start() 
{
 // read in all s files
 
    system("ls *.s > progs");
     
    fstream readProgs;
    readProgs.open("progs", ios::in);
	
    if (!readProgs.is_open()) {
        cout << "Progs failed to open.\n";
        exit(1);
    }

    string line;
    getline(readProgs, line);
    
    while (!readProgs.eof()) {
        
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
    
        vm.loadIntoMemory(p->originalfilename, &base, &limit);
    
        p->base = base;
        p->limit = limit;
        p->pc = base;
        
        getline(readProgs, line);      
    } // end reading into memory
    
    readProgs.close();
    system("rm progs");

    // copy first item in readyQ to running , load pcb data to vm  

    running = readyQ.front();
    readyQ.pop();
    givePCBToVM();
    
    int code = 0; 
    
    while (!done()) // main process loop
    {
        code = 0;
   
        vm.run(); // runs current proceess in vm, returns error code stored for later
        
        // get return code from status register
        code = vm.sr;    
        code = code & 0xe0;  
        code >>= 5;          
        
        // update clocks for context switch
        masterClock += 5;
        contextSwitchClock += 5;
        
        // update all processes that aren't terminated with addition to context switch time
        for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++)
            if(!strcmp((*it)->state.c_str(), "terminated"))
                (*it)->contextSwitchTime += 5;

        // First item in main loop
    
        // move items from waitQ into readyQ if interrupt time hit
        if(!waitQ.empty()) {
            while (waitQ.front()->interruptTime <= masterClock) {
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
            masterClock += vm.clock;
            running->cpuTime += vm.clock;

            // main switch to handle return code, if terminated set, output errorcode.
            switch (code) {
                case 0:
                    getPCBFromVM();
                    readyQ.push(running);          
                    break;           
                case 1:
                    running->state = "terminated";
                    break;           
                case 2:
                    running->state = "terminated";
                    outputErrorCode("Out-of-Bound Reference");
                    break;           
                case 3:
                    running->state = "terminated";           
                    outputErrorCode("Stack Overflow");
                    break;      
                case 4:
                    running->state = "terminated";
                    outputErrorCode("Stack Underflow");
                    break;
                case 5:
                    running->state = "terminated";
                    outputErrorCode("Invalid OPCode");
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
      
                    masterClock++;
                    running->ioTime += 27;
                    running->cpuTime++;  
                    waitQ.push(running);
                    running->state = "waiting";
                    getPCBFromVM(); 
                    running->interruptTime = masterClock + 28;
               
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
                
                    masterClock++;
                    running->ioTime += 27;
                    running->cpuTime++;                   
               
                    waitQ.push(running);
                    running->state = "waiting";
                    getPCBFromVM();
                    running->interruptTime = masterClock + 28;

                    break;
                }
                
                default:
                    outputErrorCode("Invalid Return Code");
                    running->state = "terminated";
                    break;
            }
        
            running = NULL;
 
        } // end if running loop
        

        // Third item in main loop
   
        // if all items in waitQ, update masterclock to get first item out of waitQ
        if(readyQ.empty() && !waitQ.empty()) {

            int temp = waitQ.front()->interruptTime - masterClock;
    
            masterClock += temp;
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
         
    double floatingPointMasterClock = masterClock;  // calculate CPU Utilization
        
    double cpuUtilization = (floatingPointMasterClock - idleClock) / floatingPointMasterClock;  // calculate CPU Utilization
                  
    int allCpuTime = 0;
           
    // iterate through all processes to add cpuTime
    for (list<PCB *>::iterator it = jobs.begin(); it != jobs.end(); it++)
        allCpuTime += (*it)->cpuTime;    
            
    double userCpuUtilization = allCpuTime / floatingPointMasterClock; // calculate user cpu ulitizaton.
                   
    double throughPut = jobs.size() / (floatingPointMasterClock / 10000.0);  // calculate throughput
        
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
    
    for (int i = 0;i <= vm.REG_FILE_SIZE - 1;i++)
        vm.reg[i] = running->reg[i];
    
    if (running->sp < 256) {
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
}



void OS::getPCBFromVM()
{
    //copy contents of VM into running PCB, including stack information
     
    running->pc= vm.pc; 
    running->sr= vm.sr; 
    running->sp=vm.sp;
    running->base= vm.base;
    running->limit= vm.limit;
    
    for (int i = 0;i <= vm.REG_FILE_SIZE - 1;i++) {
        running->reg[i] = vm.reg[i];
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

// main to run OS
int main()
{
  OS os;
  os.start(); 
} 


