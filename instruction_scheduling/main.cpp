//
//  main.cpp
//  OOO
//
//  Created by Aneshvardan Pushparaj on 11/9/19.
//  Copyright © 2019 Aneshvardan Pushparaj. All rights reserved.
//

#include <iostream>
#include "Pipeline.hpp"
#include <unistd.h>
int main(int argc, const char * argv[]) {
    // Create a pipeline from program argument
    Pipeline pipeline;
    std::string input = argv[1];
    pipeline.populate(input);
    
    // While not all instructions have commited, run through all stages in reverse order and increment cycle
    while(!pipeline.done())
    {
        pipeline.commit();
        pipeline.writeback();
        pipeline.issue();
        pipeline.dispatch();
        pipeline.rename();
        pipeline.decode();
        pipeline.fetch();
        
        pipeline.cycle++;
    }
    //pipeline.print();
    pipeline.produceFile("out.txt");
    return 0;
}
