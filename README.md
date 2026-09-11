# Memory-Pool-Allocator
Custom C++ memory pool allocator with free-list, benchmarked vs new/delete


Prompt
g++ -std=c++17 -O2 -Iinclude src/main.cpp -o memory_pool_demo -pthread
./memory_pool_demo