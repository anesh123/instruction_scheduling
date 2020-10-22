Instruction Scheduling\
performs dataflow scheduling of pipeline instructions\

Input file:\
Name it test.in\
Each input has to contains 2 comma separated parameters\
Parameter 1: number of physical registers in the system\
Parameter 2: issue width of machine (https://en.wikipedia.org/wiki/Wide-issue)

Following lines:
Instructions needed to schedule(between 1-256 lines)
Format:
  R,REG,REG,REG
  I,REG,REG,IMM
  L,REG,IMM,REG
  S,REG,IMM,REG
  
  <REG> and <IMM> - encoded as decimals\
  REG - (+)ve integer between 0-31(inclusive) i.e [0-31]\
  IMM - (+)ve integer value between 0 and 2^16\
  First <REG> is destination for R,I,L\
  Memory(not modelled)- destination for S\
  https://inst.eecs.berkeley.edu/~cs61c/resources/su18_lec/Lecture7.pdf - page 7\

Output:\
  Output will be integers representing stages of pipeline execution\
  <FE>,<DE>,<RE>,<DI>,<IS>,<WB>,<CO>\
  Fetch,Decode,Rename,Dispatch,Issue,Writeback,Commit\
  https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-823-computer-system-architecture-fall-2005/lecture-notes/l12_ooo_pipes.pdf\
  
Sample:\
Input:\

64,2\
L,2,80,4\
L,3,64,5\
R,2,2,3\
S,2,24,29\
I,1,0,8\
R,6,6,1\
R,7,7,1\
L,4,0,6\
L,5,0,7\
L,2,80,4\
L,3,64,5\
R,2,2,3\
S,2,24,29

Output:\

0,1,2,3,4,5,6\
0,1,2,3,4,5,6\
1,2,3,4,5,6,7\
1,2,3,4,6,7,8\
2,3,4,5,6,7,8\
2,3,4,5,7,8,9\
3,4,5,6,7,8,9\
3,4,5,6,8,9,10\
4,5,6,7,8,9,10\
4,5,6,7,9,10,11\
5,6,7,8,9,10,11\
5,6,7,8,10,11,12\
6,7,8,9,11,12,13

Instructions:\

1)Download onto local machine\
2)Copy test.in file to directory\
3)Convert to tar.gz file\
4)Copy to virtual machine\
5)Untar file in new directory\
6)Run make test\

References:\
Complex Pipelining- Out of Order Execution & Register Renaming, Arvind - https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-823-computer-system-architecture-fall-2005/lecture-notes/l12_ooo_pipes.pdf


  
  
  
  
  
