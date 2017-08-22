//
//  gpuMain.cpp
//  mapping
//
//  Created by Joe Zhou on 12/13/16.
//  Copyright © 2016 Joe Zhou. All rights reserved.
//

#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "sa.h"
#include <ctime>

__device__
void strncmpGPU(const char *s1, const char *s2, size_t n, int* result){
    int threadID = blockIdx.x * blockDim.x + threadIdx.x;
    for( ; n>0;s1++, s2++, --n)
        if(*s1 != *s2)
            *result = ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
        else if (*s1 == '\0')
            *result = 0;
    *result = 0;
}

//__global__
//void saxpy(int n, float a, float *x, float *y)
//{
//    int i = blockIdx.x*blockDim.x + threadIdx.x;
//    if (i < n) y[i] = a*x[i] + y[i];
//}
//
//__global__
//void foo(int* result, int n)
//{
//    int threadID = blockIdx.x * blockDim.x + threadIdx.x;
//    if (threadID < n)
//        result[threadID] = threadID;
//}

__global__
void search(char*** readsGroups, int numReads, char* ref, int* sa, int* result, int m, int n)
{
    int threadID = blockIdx.x * blockDim.x + threadIdx.x;
    
    char** reads = readsGroups[threadID];
    //int m = strlen(reads[1]);  // get length of pattern, needed for strncmp()
    //int n = strlen(ref);
    
    //int *result = new int[n];
    //int unmatched = 0;
    
    for(int i=0; i< numReads; i++){
        char* pat = reads[i];
        // Do simple binary search for the pat in txt using the
        // built suffix array
        int l = 0, r = n-1;  // Initilize left and right indexes
        while (l <= r)
        {
            // See if 'pat' is prefix of middle suffix in suffix array
            int mid = l + (r - l)/2;
            int *res = new int;
            strncmpGPU(pat, ref+sa[mid], m, res);
            
            // If match found at the middle, print it and return
            if (*res == 0)
            {
                //return sa[mid];
                result[sa[mid]] = result[sa[mid]] + 1;
            }
            
            // Move to left half if pattern is alphabtically less than
            // the mid suffix
            if (*res < 0) r = mid - 1;
            
            // Otherwise move to right half
            else l = mid + 1;
        }
        
        //unmatched++;
    }
}

//Genomefilename, readsFileName, number of blocks, number of threads, number of reads
int main(int argc, const char * argv[]) {
    clock_t start, end;
    start = clock();
    //read sequence from file, allocate sequence on heap b/c large size
    std::string * input = new std::string();
    std::ifstream file;
    file.open(argv[1]);
    getline(file,*input);
    file.close();
    std::cout << "ref file read\n";
    std::cout << "time took = " << (clock()-start)/double(CLOCKS_PER_SEC)*1000 << "\n";
    std::string sequence = *input;
    //delete input;
    sequence += "$";//end of sequence char
    int numBlocks= *argv[3];
    int numThreads = *argv[4];
    int totalThreads = numBlocks * numThreads;
    
    char* s = (char*)malloc(sequence.size()*sizeof(char));//on heap
    std::copy(sequence.begin(), sequence.end(), s);
    s[sequence.size()] = '\0';
    
    std::vector<int> sav = buildSuffixArray(sequence, sequence.size());
    std::cout << "suffix tree build\n";
    std::cout << "time took = " << (clock()-start)/double(CLOCKS_PER_SEC)*1000 << "\n";
    int* sa = (int*)malloc(sav.size()*sizeof(int));//on heap
    sa = &sav[0];
    file.open(argv[2]);
    std::string line;
    
    int numReads = *argv[5];
    int index = 0;
    char** reads = (char**)malloc((numReads+1000000000000000000+1)*sizeof(char*));
    std::cout << "2 \n";
    while(std::getline(file, line)){
        //convert string to char* for checkForSubString();
        char* currRead = (char*)malloc(line.size()+1);
        memcpy(currRead, line.c_str(),line.size());
        reads[index] = currRead;
        //free(currRead);
        index = index +1;
    }
    std::cout << "3 \n";
    char*** readsGroups = (char***)malloc(totalThreads*sizeof(char**));
    int groupsize = numReads/totalThreads;
    for(int i=0;i<totalThreads;i++){
        char** readg = (char**)malloc(groupsize*sizeof(char*));
        memcpy(readg, reads+i*groupsize, groupsize*sizeof(char*));
        readsGroups[i] = readg;
    }
    
    std::cout << "reads read \n";
    std::cout << "time took = " << (clock()-start)/double(CLOCKS_PER_SEC)*1000 << "\n";
    
    int* result = (int*)malloc(sequence.size()*sizeof(int));
    for(int i=0; i<sequence.size(); i++){
        result[i] = 0;
    }
    
    //allocate GPU memory
    char* d_s; int* d_sa; char*** d_readsGroups; int* d_result;
    cudaMalloc(&d_s, sequence.size()*sizeof(char));
    cudaMalloc(&d_sa, sav.size()*sizeof(int));
    cudaMalloc(&d_readsGroups, totalThreads*sizeof(char**));
    cudaMalloc(&d_result, sequence.size()*sizeof(int));
    
    //transfer data from CPU to GPU
    cudaMemcpy(d_s,s,sequence.size()*sizeof(char),cudaMemcpyHostToDevice);
    cudaMemcpy(d_sa,sa,sav.size()*sizeof(int),cudaMemcpyHostToDevice);
    cudaMemcpy(d_readsGroups,readsGroups,totalThreads*sizeof(char**),cudaMemcpyHostToDevice);
    cudaMemcpy(d_result,result,sequence.size()*sizeof(int),cudaMemcpyHostToDevice);
    
    std::cout << "performing search on GPU\n";
    std::cout << "time took = " << (clock()-start)/double(CLOCKS_PER_SEC)*1000 << "\n";
    
    //perform search
    search<<<numBlocks,numThreads>>>(d_readsGroups, numReads, d_s, d_sa, d_result, strlen(reads[1]), strlen(s));
    //foo<<<256,256>>>(d_result, strlen(s));
    
    std::cout << "search finished\n";
    std::cout << "time took = " << (clock()-start)/double(CLOCKS_PER_SEC)*1000 << "\n";
    
    //transfer alignment result back to CPU
    cudaMemcpy(result, d_result, sequence.size()*sizeof(int), cudaMemcpyDeviceToHost);
    //cudaMemcpy(y, d_y, N*sizeof(float), cudaMemcpyDeviceToHost);
    
    //free heap memory on GPU and CPU
        cudaFree(d_s);
        cudaFree(d_sa);
        cudaFree(d_readsGroups);
        cudaFree(d_result);
        free(s);
        free(sa);
    for(int i=0; i < numReads;i++){
        //free(reads[i]);
    }
    
    //    for(int i=0; i<sequence.size();i++){
    //        std::cout << result[i] << " ";
    //    }
    
    //free(result);
    
    end = clock();
    std::cout << "finished";
    std::cout << "time took = " << (end-start)/double(CLOCKS_PER_SEC)*1000 << "\n";
}