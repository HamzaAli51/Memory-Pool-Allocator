#pragma once
#include <cstddef>
#include <cstdlib>
#include <mutex>
using namespace std;

template <typename T>
class MemoryPool {
public:
    explicit MemoryPool(size_t numBlocks) {
        numBlocks_ = numBlocks;
        size_t bigger = sizeof(Node) > sizeof(T) ? sizeof(Node) : sizeof(T);
        blockSize_ = bigger;

        arena_ = static_cast<byte*>(malloc(numBlocks_ * blockSize_));
        freeListHead_ = reinterpret_cast<Node*>(arena_);

        for (size_t i = 0; i < numBlocks_ - 1; ++i) {
            Node* current = reinterpret_cast<Node*>(arena_ + i * blockSize_);
            Node* nextNode = reinterpret_cast<Node*>(arena_ + (i + 1) * blockSize_);
            current->next = nextNode;
        }

        Node* lastNode = reinterpret_cast<Node*>(arena_ + (numBlocks_ - 1) * blockSize_);
        lastNode->next = nullptr;
    }

    ~MemoryPool() {
        free(arena_);
    }

    void* allocate() {
        lock_guard<mutex> lock(mutex_);
        Node* freeNode = freeListHead_;
        freeListHead_ = freeNode->next;
        return static_cast<void*>(freeNode);
    }

    void deallocate(void* ptr) {
        lock_guard<mutex> lock(mutex_);
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
    mutex mutex_; // makes allocate/deallocate thread-safe
};