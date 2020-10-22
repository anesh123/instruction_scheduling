//
//  Pipeline.cpp
//  OOO
//
//  Created by Aneshvardan Pushparaj on 11/9/19.
//  Copyright © 2019 Aneshvardan Pushparaj All rights reserved.
//

#include "Pipeline.hpp"
using namespace std;

void Pipeline::populate(std::string filePath)
{
    // Open the file, read inputs, and close
    ifstream file;
    file.open(filePath);
    string strng;   // Temp string for storing reead data
    if (!file.good()) {
        printf("File error\n"); // Error check if file is corrupt
        exit(1);
    }
    
    while (file.good()) {
        // Read the first line and update regCount and width
        getline(file,strng);
        istringstream ss(strng);
        string tempString;
        getline(ss,tempString, ',');
        Pipeline::regCount = stoi(tempString);
        getline(ss,tempString);
        Pipeline::width = stoi(tempString);
        
        // Read remaining lines and update the instructions vector
        while (getline(file, strng)) {
            Pipeline::instruction inst;
            istringstream ss(strng);
            getline(ss, tempString, ',');
            if      (tempString == "R") {inst.type = 1;}
            else if (tempString == "I") {inst.type = 2;}
            else if (tempString == "L") {inst.type = 3;}
            else if (tempString == "S") {inst.type = 4;}
            else {
                printf("Read Error");
                exit(1);
            }
            
            getline(ss, tempString, ',');
            if (inst.type < 4) {inst.dest = stoi(tempString);}  // For a store, the first register is considered an operand
            else {inst.oper1 = stoi(tempString);}
            
            // Do not update immediate values (leave at -1)
            getline(ss, tempString, ',');
            if (inst.type <=2 ) {inst.oper1 = stoi(tempString);}
            
            getline(ss, tempString);
            if (inst.type != 2) {inst.oper2 = stoi(tempString);
            }
            instructions.push_back(inst);
        }
    }
    file.close();
    
    // Construct readyTable with all variables ready and a free list of registers 32,33,...
    for (int i = 0; i< regCount; i++)
    {
        readyTable.push_back(true);
        if (i < 32) { mapTable.push_back(i);}
        else
        {
            mapTable.push_back(i);
            freeList.push_front(i);
        }
    }
}

void Pipeline::produceFile(std::string filePath)
{
    // Open the file, read inputs, and close
    ofstream file;
    file.open(filePath);
    string strng;   // Temp string for storing reead data
    if (!file.good()) {
        printf("File error\n"); // Error check if file is corrupt
        exit(1);
    }
    for (auto &i:instructions)
    {
        file << i.fetchCycle << "," << i.decodeCycle << "," << i.renameCycle << ","
        << i.dispatchCycle << "," << i.issueCycle << "," << i.writebackCycle << ","
        << i.commitCycle << "\n";
    }
}



void Pipeline::print()
{
    printf("Type\tDest\tOper1\tOper2\trDest\trOper1\trOper2\tF\tD\tR\tD\tI\tW\tC\n");
    for (auto& i:instructions)
    {
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               i.type, i.dest, i.oper1, i.oper2, i.Rdest, i.Roper1, i.Roper2,
               i.fetchCycle, i.decodeCycle, i.renameCycle, i.dispatchCycle, i.issueCycle,
               i.writebackCycle, i.commitCycle);
    }
}


bool Pipeline::done()   // Check if the last instruction has committed.
{
    return instructions[instructions.size()-1].commitCycle > -1;
}


bool Pipeline::instructionReady(int instNumber)
{
    bool oper1Ready, oper2Ready;
    
    // If operator is immediate, set to ready. Else look at ready table
    if (instructions[instNumber].Roper1 <= -1) {oper1Ready = true;}
    else {oper1Ready = readyTable[instructions[instNumber].Roper1];}
    if (instructions[instNumber].Roper2 <= -1) {oper2Ready = true;}
    else {oper2Ready = readyTable[instructions[instNumber].Roper2];}
    
    // if instruction is store, check if any loads are still issuing +oper1 and 2
    if (instructions[instNumber].type == 4 ) { return oper1Ready && oper2Ready && lwCount <=0; }
    // if instruction is load, check if any stores are still issuing +oper1 and 2
    else if (instructions[instNumber].type == 3 ) { return oper1Ready && oper2Ready && swCount <=0; }
    // Instruction is ready for issue if both operands are ready
    return oper1Ready && oper2Ready;
}


void Pipeline::fetch()
{
    for (int i = 0; i < width; i++)
    {
        if (PC == static_cast<int>(instructions.size())) {break;}   // Stop if we fetched all instructions
         // Fetch instructions up until width and reflect the cycle
        fetchedInst.push_back(PC);
        instructions[PC++].fetchCycle = cycle;
    }
}

void Pipeline::decode()
{
    // Read all fetched instructions, update cycle and place instruction in decode queue
    for (auto i:fetchedInst)
    {
        instructions[i].decodeCycle = cycle;
        fetchedInst.pop_back();
        decodedInst.push_back(i);
    }
}

void Pipeline::rename()
{
    for (auto i:decodedInst)
    {
        if (instructions[i].type < 4)   // Rename non-store words
        {
            if (freeList.empty()) {break;}  // Stall if free list is empty
            
            // Rename destination register, lookup renamed ISA operand registers in mapTable
            instructions[i].Rdest = freeList.back();
            if (instructions[i].oper1 > -1)
            {
               instructions[i].Roper1 = mapTable[instructions[i].oper1];
            }
            if (instructions[i].oper2 > -1)
            {
            instructions[i].Roper2 = mapTable[instructions[i].oper2];
            }
            
            // Look for the overwritten register, update mapTable to point the ISA register to the new rename one
            // Update the destination register tag to not ready, and place all instruction in rename queue
            instructions[i].overwrittenReg = mapTable[instructions[i].dest];
            mapTable[instructions[i].dest] = instructions[i].Rdest;
            freeList.pop_back();
            instructions[i].renameCycle = cycle;    // Update cycle
            decodedInst.pop_front();
            renamedInst.push_back(i);
            readyTable[instructions[i].Rdest] = false;
        }
        else    // Abridged renaming for S instructions
        {
            instructions[i].renameCycle = cycle;
            decodedInst.pop_front();
            renamedInst.push_back(i);
        }
    }
}


void Pipeline::dispatch()
{
    // Put all renamed instructions in the IQ and update cycle
    for (auto i:renamedInst)
    {
        IQ.push_back(i);
        renamedInst.pop_front();
        instructions[i].dispatchCycle = cycle;
    }
}

void Pipeline::issue()
{
    // If instructions are ready for execution, issue them and increment counters
    int count = 0;
    for (int i = 0; i < IQ.size(); i++)
    {
        int inst = IQ[i];
        deque<int>::iterator it = IQ.begin();
        if(instructionReady(inst))
        {
            instructions[inst].issueCycle = cycle;
            issuedInst.push_back(inst);
            advance(it, i);
            IQ.erase(it);
            count++;
            i--; // decrement i after deleting elements
            if (instructions[inst].type == 4) {swCount++;}
            if (instructions[inst].type == 3) {lwCount++;}  // Increment L/S counters
            if(count >= width ) {break;}    // Stop if have issued the maximum number of instructions w
        }
    }
}


void Pipeline::writeback()
{
    // Write all issued instructions and update readyTable
    for (auto i:issuedInst)
    {
        instructions[i].writebackCycle = cycle;
        if (instructions[i].Rdest > -1)
        {
            readyTable[instructions[i].Rdest] = true;   // register is now ready
        }
        
        if (instructions[i].type == 4) {swCount--;} // Decrement sw counter here (assuming address forwarding)
        writtenInst.push_back(i);
        issuedInst.pop_front();
    }
}

void Pipeline::commit()
{
    // Move from second free list to first (takes an extra cycle)
    for (auto i:freeList2)
    {
        freeList.push_back(i);
        freeList2.pop_front();
    }
    
    std::sort(writtenInst.begin(), writtenInst.end());  // Sort issued instructions to commit in order
    
    // Commit instructions, decrement LW counter, update freelist
    int count = 0;
    for (int i = 0; i < writtenInst.size(); i++)
    {
        int inst = writtenInst[i];
        if (instructions[inst].type == 3) {lwCount--;}  // Decrement load counter

        deque<int>::iterator it = writtenInst.begin();
        if (inst == 0)  // The first instruction will commit regardless
        {
            instructions[inst].commitCycle = cycle;
            freeList2.push_back(instructions[inst].overwrittenReg);
            advance(it, i);
            writtenInst.erase(it);
            count++;
            i--;
        }
        else{
                
            if(instructions[inst-1].commitCycle > -1)
            {
                instructions[inst].commitCycle = cycle;
                if (instructions[inst].type < 4)
                {
                    freeList2.push_back(instructions[inst].overwrittenReg);
                }
                advance(it, i);
                writtenInst.erase(it);
                count++;
                //size--;
                i--;
                if(count >= width) {break;} // Stop when width is reached
            }
        }
    }
}
