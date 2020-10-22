//
//  Pipeline.hpp
//  OOO
//
//  Created by Aneshvardan Pushparaj on 11/9/19.
//  Copyright © 2019 Aneshvardan Pushparaj. All rights reserved.
//

#ifndef Pipeline_hpp
#define Pipeline_hpp

#include <stdio.h>
#include <queue>    // Deque is used in almost every stage
#include <vector>   // For storing ready and map tables
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>    // For sorting deques

class Pipeline{

private:
    // All information of an intstruction are stored in a vector and modified in place
    // -1 indicates unassigned value and/or immediate operand value
    struct instruction
    {
        int type;   // 1==R, 2==I, 3==L, 4==S
        int dest=-1;
        int oper1=-1;
        int oper2=-1;
        
        // Renamed variables
        int Rdest=-1;
        int Roper1=-1;
        int Roper2=-1;
        
        int overwrittenReg=-1;  // This register will be put back in the free list after commit
        
        // Cycles where the instruction went through a stage
        int fetchCycle=-1;
        int decodeCycle=-1;
        int renameCycle=-1;
        int dispatchCycle=-1;
        int issueCycle=-1;
        int writebackCycle=-1;
        int commitCycle=-1;
    };
    
    int PC = 0;             // Program Counter used by fetch
    int regCount = 0;       // number of physical registers available (>32)
    int width;              // Width of the machine (how many instructions are processed in parallel)
    int swCount = 0;        // Number of store words currently issued. Decremented after writeback
    int lwCount = 0;        // Number of load words currently issued. Decremented after writeback
    
    // Each integer queue holds indicies of instructions to be processed
    std::deque<int> freeList;       // Holds available registers for renaming purposes
    std::deque<int> freeList2;      // Intermediate free list. Used to split freeing registers to two half cycles
    std::deque<int> IQ;             // Instruction Queue used by the issue stage
    std::deque<int> fetchedInst;
    std::deque<int> decodedInst;
    std::deque<int> renamedInst;
    std::deque<int> issuedInst;
    std::deque<int> writtenInst;
    
    std::vector<int>  mapTable;     // Table that shows what the ISA register is currently mapped to
    std::vector<bool> readyTable;   // Tag each register with ready bit, used by issue to know if operands are ready
    std::vector<instruction> instructions;  // Instruction memory where most of the modification and bookkeeping will happen


public:
    // Fetches w instructions and holds them in a queue. Increments PC+=w, and updates each instruction
    // To reflect the cycle is has been fetched
    void fetch();
    
    // Decodes all instructions currently in fetch queue and updates them to reflect cycle time
    void decode();
    
    // Rename instructions in decode queue using next free registers in the free list. Stalls if free list is empty
    void rename();
    
    // Dispatches instructions to the IQ.
    void dispatch();
    
    // Determines if an instruction is ready to issue and issues <=w instructions that are ready.
    // Increments sw/lw counters
    void issue();
    
    // All issued instructions are written. finished instructions are tagged as ready in the ready table
    void writeback();
   
    // Commits instructions, updates free list (1 and 2), and sw/lw counters are decremented
    void commit();
    
    // Is the pipeline done commiting all instructions by seeing if the last instruction has commited
    bool done();
    
    // Given an index to an instructions, we determine if an instruction is ready for execution
    // Takes into account conservative load/store and operands readiness
    bool instructionReady(int instNumber);
    
    // Reads an input file and stores inputs into program memory
    void populate(std::string filePath);
    
    // Initial cycle time to be incremented after executing all 7 phases
    int cycle = 0;
    
    // Prints current pipeline instruction status
    void print();
    
    void produceFile(std::string filePath);
};
#endif /* Pipeline_hpp */
