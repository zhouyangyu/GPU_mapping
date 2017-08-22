
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "st.h"

//Genomefilename, readFileName
int main(int argc, const char * argv[]) {
    //read sequence from file, allocate sequence on heap b/c large size
    std::string * input = new std::string();
    std::ifstream file;
    file.open(argv[1]);
    getline(file,*input);
    file.close();
    std::string sequence = *input;
    delete input;
    sequence += "$";//end of sequence char
    
    char * s = new char[sequence.size() + 1];
    std::copy(sequence.begin(), sequence.end(), s);
    s[sequence.size()] = '\0';
    
    buildSuffixTree(s);
    delete[] s;
    file.open(argv[2]);
    double matchScore [sequence.size()];//needs to be on heap
    std::string line;
    int readLength;
    clock_t start = clock();
    while(std::getline(file, line)){
        //std::cout << line;
        readLength = line.length();
        //convert string to char* for checkForSubString();
        char* currRead = (char*)malloc(line.size()+1);
        memcpy(currRead, line.c_str(),line.size()+1);
        checkForSubString(currRead);
        free(currRead);
        
        std::vector<long> matchIndices = getMatchIndex();
        clearMatchIndex();
        for(int i=0; i<matchIndices.size();i++){
            for(long j=matchIndices[i];j<matchIndices[i]+readLength;j++){
                matchScore[j] += 1.0/(double)matchIndices.size();
            }
        }
    }
    clock_t end = clock();
    std::cout <<(end-start)/double(CLOCKS_PER_SEC)*1000 << "\n";
    Node* root = getRoot();
    freeSuffixTreeByPostOrder(root);
    
    std::ofstream out(std::strcat((char*)argv[2], "score"));
    for(int i=0; i<sequence.size()-1;i++){
        out << std::to_string(matchScore[i]) << "\n";
        //printf("%f ", matchScore[i]);
    }
    
    return 0;
}
