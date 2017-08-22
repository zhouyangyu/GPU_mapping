//
//  generateReads.cpp
//  mapping
//
//  Created by Joe Zhou on 12/2/16.
//  Copyright © 2016 Joe Zhou. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

// filename, readlength, numReads
int main(int argc, const char * argv[]) {
    //read sequence from file, allocate sequence on heap b/c large size
    std::string * sequence = new std::string();
    std::ifstream file;
    file.open(argv[1]);
    getline(file,*sequence);
    file.close();
    
    std::string s = *sequence;
    int readLength;
    sscanf(argv[2],"%d",&readLength);
    int numReads;
    sscanf(argv[3],"%d",&numReads);
    int seqLength = s.length();
    std::cout << "seqLength= " << seqLength <<"\n";
    std::string outFileName = std::to_string(readLength) + "_" + std::to_string(numReads);
    std::cout << outFileName;
    
    std::ofstream out(outFileName);
    std::ofstream score(outFileName + "keyScore");
    std::srand(3);
    int startIndex [seqLength];
    for(int i=0; i < seqLength; i++){
        startIndex[i] = 0;
    }
    
    for(int i=0; i<numReads; i++){
        int starta = rand() % (seqLength-readLength-2)+1;
        for(int j=0; j<rand() % 3;j++){ // random amount of reads around a position
            int start = starta + rand() % 2; // random start position around starta
            out << s.substr(start,readLength) << "\n";
            startIndex[start]++;
        }
    }
    
    for(int i=0; i<seqLength;i++){
        //std::cout << startIndex[i];
        score << std::to_string(startIndex[i]) << "\n";
    }
    
    
    out.close();
    score.close();
    return 0;
}