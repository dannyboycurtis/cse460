/********************************************************************************
 * James Small & William Elder
 * PCB.h
 * 5/7/13
 * This is the process control block class to be used in our OS.  We keep a list
 * of pointers to these PCB's in order to hold information about all current
 * running processes on the system.  The information about the processes is copied
 * to and from these PCB's in order to facilitate our os simulation.
 ********************************************************************************/

#ifndef PCB_H
#define PCB_H

using namespace std;

class PCB {
private:
    int pc,sr,sp,base,limit;
    static const int REG_FILE_SIZE = 4;
    static const int FRAME_COUNT = 32;
    vector < int > reg;
    
    string readfilename, writefilename, originalfilename, stfilename, state;
    
    fstream readIntoRegister;
    fstream writeToRegister;
    
    int cpuTime, waitingTime, turnaroundTime, contextSwitchTime, idleTime, ioTime,largestStackSize, interruptTime;
    
    vector <int> frames;
    vector <int> valid;
    vector <int> modified;
    
public: 
    PCB() {
        reg = vector <int> (REG_FILE_SIZE);
        pc = sr = cpuTime = waitingTime = turnaroundTime = ioTime = largestStackSize = contextSwitchTime = idleTime = 0;
        for (int i = 0;i < REG_FILE_SIZE; i++) {
            reg[i] = 0;
        }
        sp = 256;
        state = "new";    
        
        frames = vector <int> (FRAME_COUNT);
        valid = vector <int> (FRAME_COUNT);
        modified = vector <int> (FRAME_COUNT);
        
        for (int i = 0; i < FRAME_COUNT; i++) 
            frames[i] = valid[i] = modified[i] = 0;      
    
    };
    
    void openReadInFileStream() {
        readIntoRegister.open(readfilename.c_str(), ios::in);
	
        if (!readIntoRegister.is_open()) {
       
           // exit(1);
        }   
    }
    
    void openWriteOutFileStream() {
    
        writeToRegister.open(writefilename.c_str(), ios::out);
	
        if (!writeToRegister.is_open()) {
            cout << writefilename << " failed to open.\n";
            exit(1);
        }
    }
      
    ~PCB() {
        readIntoRegister.close();
        writeToRegister.close();
    }
    
    friend class OS; 
    friend class VirtualMachine;
};

#endif
