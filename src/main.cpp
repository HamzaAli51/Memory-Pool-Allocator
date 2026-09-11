#include <chrono>
#include <iostream>
#include <vector>
#include "MemoryPool.h"
using namespace std;

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    int life;

    Particle() : x(0), y(0), z(0), vx(0), vy(0), vz(0), life(100) {}

    static MemoryPool<Particle>& pool() {
        static MemoryPool<Particle> instance(200000);
        return instance;
    }

    static void* operator new(size_t) {
        return pool().allocate();
    }

    static void operator delete(void* ptr) {
        pool().deallocate(ptr);
    }
};

template <typename Func>
long long timeMicros(Func&& fn) {
    auto start = chrono::high_resolution_clock::now();
    fn();
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::microseconds>(end - start).count();
}

int main() {
    constexpr int kIterations = 200000;

    cout << "=== Memory Pool Allocator Benchmark ===\n";
    cout << "Allocating/deallocating " << kIterations << " Particle objects\n\n";

    long long defaultTime = timeMicros([&] {
        vector<Particle*> ptrs;
        ptrs.reserve(kIterations);
        for (int i = 0; i < kIterations; ++i) {
            ptrs.push_back(::new Particle()); // forces the REAL global new, bypassing our overload
        }
        for (auto* p : ptrs) {
            ::delete p;
        }
    });

    long long poolTime = timeMicros([&] {
        vector<Particle*> ptrs;
        ptrs.reserve(kIterations);
        for (int i = 0; i < kIterations; ++i) {
            ptrs.push_back(new Particle()); // uses OUR overloaded operator new
        }
        for (auto* p : ptrs) {
            delete p;
        }
    });

    cout << "Default new/delete: " << defaultTime << " microseconds\n";
    cout << "Pool allocator:     " << poolTime << " microseconds\n";

    if (poolTime > 0) {
        double speedup = static_cast<double>(defaultTime) / static_cast<double>(poolTime);
        cout << "\nSpeedup: " << speedup << "x\n";
    }
}