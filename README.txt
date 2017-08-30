Author: Joe Zhou
This is the final project for Concurrent and Parallel Programming class.
Paper: https://drive.google.com/file/d/0B_WU_ECymFUBUmQzdVpVOFpHOEU/view

Usage:
Must have CUDA 8.0 and Thrust v1.8.0 installed on compatible NVIDIA Machine.

main.cpp is the code to be run on CPU.
to compile: g++ main.cpp sa.cpp

gpuMain.cu is the CUDA code to be run on GPU.
to compile: nvcc gpuMain.cu sa.cu

40_100000 contains 88608 reads, each of the length 40 letters.
generateReads.cpp generate reads from reference file WS6 and outputs the read file (such as 40_100000 and 40_100000keyScore) 40_100000keyScore is the correct match starting index.

