#pragma once
#include <cstddef>
#include <cstdlib>
using namespace std;

template <typename T>
class MemoryPool {
public:
    explicit MemoryPool(size_t numBlocks) {
        numBlocks_ = numBlocks;
        size_t bigger = sizeof(Node) > sizeof(T) ? sizeof(Node) : sizeof(T);
        blockSize_ = bigger;

        // Step A: malloc the arena.
        // How many total bytes do we need? (numBlocks_ blocks, each blockSize_ bytes)
        arena_ = static_cast<byte*>(malloc( numBlocks_ * blockSize_ ));

        // Step B: freeListHead_ starts at block index 0 —
        // that's just the very start of the arena, cast to a Node*.
        freeListHead_ = reinterpret_cast<Node*>( arena_ );

        // Step C: link block i to block i+1, for every block except the last one.
        // Loop from i = 0 up to (but not including) ??? 
        for (size_t i = 0; i < numBlocks_ - 1; ++i) {
            Node* current = reinterpret_cast<Node*>(arena_ + i * blockSize_);
            Node* nextNode = reinterpret_cast<Node*>( arena_ + (i + 1) * blockSize_ ); 
            current->next = nextNode;
        }

        // Step D: the LAST block (index numBlocks_ - 1) must point to nullptr,
        // marking the end of the free-list chain.
        Node* lastNode = reinterpret_cast<Node*>( arena_ + (numBlocks_ - 1) * blockSize_ );
        lastNode->next = nullptr;
    }

    ~MemoryPool();
    void* allocate()
    {
        Node* freeNode = freeListHead_;
        freeListHead_ = freeNode->next;
        return static_cast<void*>(freeNode);
    }
    void deallocate(void* ptr)
    {
        Node* nodeToFree = static_cast<Node*>(ptr);
        nodeToFree->next = freeListHead_;
        freeListHead_ = nodeToFree;
    }

private:
    struct Node {
        Node* next;
    };

    byte* arena_;
    Node* freeListHead_;
    size_t blockSize_;
    size_t numBlocks_;
};