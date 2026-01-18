#pragma once

#include<iostream>

// ========================================
// 追踪对象，用于验证生命周期
// ========================================
class TrackedObject
{
    static int construct_count;
    static int destruct_count;
    static int copy_count;
    static int move_count;

    int value;

public:
    TrackedObject(int v = 0) : value(v)
    {
        construct_count++;
        std::cout << "  Construct " << value << " (total: " << construct_count << ")" << std::endl;
    }

    ~TrackedObject()
    {
        destruct_count++;
        std::cout << "  Destruct " << value << " (total: " << destruct_count << ")" << std::endl;
    }

    TrackedObject(const TrackedObject& other) : value(other.value)
    {
        copy_count++;
        std::cout << "  Copy " << value << std::endl;
    }

    TrackedObject(TrackedObject&& other) noexcept : value(other.value)
    {
        move_count++;
        std::cout << "  Move " << value << std::endl;
        other.value = -1;
    }

    TrackedObject& operator=(const TrackedObject& other)
    {
        value = other.value;
        copy_count++;
        return *this;
    }

    TrackedObject& operator=(TrackedObject&& other) noexcept
    {
        value = other.value;
        move_count++;
        other.value = -1;
        return *this;
    }

    int GetValue() const { return value; }

    static void ResetCounters()
    {
        construct_count = destruct_count = copy_count = move_count = 0;
    }

    static void PrintCounters()
    {
        std::cout << "  [Stats] Construct: " << construct_count
                  << ", Destruct: " << destruct_count
                  << ", Copy: " << copy_count
                  << ", Move: " << move_count << std::endl;
    }

    static bool VerifyBalance()
    {
        bool balanced = (construct_count == destruct_count);
        if (!balanced)
            std::cout << "  ⚠️ Memory leak detected! "
                      << (construct_count - destruct_count) << " objects leaked." << std::endl;
        return balanced;
    }
};

int TrackedObject::construct_count = 0;
int TrackedObject::destruct_count = 0;
int TrackedObject::copy_count = 0;
int TrackedObject::move_count = 0;
