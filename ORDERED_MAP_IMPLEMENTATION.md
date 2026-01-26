# OrderedMap Implementation Summary

## Overview
This document summarizes the implementation of ordered map data structures for the CMCore library.

## Problem Statement
Create a set of ordered (sorted) map and set data structures, including:
- OrderedValueMap/Set for trivially copyable types
- OrderedManagedMap/Set for non-trivial types

## Implementation Approach

### Design Philosophy
The implementation follows the same pattern as existing ordered structures in CMCore:
- Uses **sorted arrays with binary search** instead of red-black trees
- Maintains elements in sorted order based on keys
- Provides O(log n) lookup performance
- Provides O(n) insertion/deletion (due to array shifting)

### Rationale for Sorted Arrays vs Red-Black Trees
1. **Consistency**: Existing OrderedValueSet and OrderedManagedSet use sorted arrays
2. **Cache-friendly**: Contiguous memory improves cache performance for iterations
3. **Simpler code**: Easier to maintain and debug
4. **Good for read-heavy workloads**: Excellent iteration and lookup performance
5. **No external dependencies**: No need for complex tree balancing algorithms

## Files Created

### 1. inc/hgl/type/OrderedMap.h
Main implementation file containing:

#### Helper Functions
- `FindDataPositionInSortedArray<>`: Binary search to find element position
- `FindInsertPositionInSortedArray<>`: Binary search to find insertion position

#### Classes

**OrderedMapTemplate<K, V, KVData>**
- Base template class for ordered maps
- Uses ObjectPool for memory management
- Uses ValueArray for storing pointers to key-value pairs
- Maintains sorted order by key
- Provides O(log n) lookups via binary search

**OrderedValueMap<K, V>**
- For trivially copyable key and value types
- Inherits from OrderedMapTemplate
- Uses KeyValue<K, V> for storage
- Operator[] support for convenient access

**OrderedManagedMapTemplate<K, V, KVData>**
- For non-trivial value types (pointers to objects)
- Inherits from OrderedMapTemplate<K, V*, KVData>
- Automatically manages object lifetimes (new/delete)
- Provides Unlink (remove without delete) and Delete (remove with delete) operations

**OrderedManagedMap<K, V>**
- Convenience typedef for OrderedManagedMapTemplate
- Uses KeyValue<K, V*> for storage

## Key Features

### Ordering Guarantees
- Elements are always maintained in sorted order by key
- Keys must support `operator<` for comparison
- Iteration yields elements in sorted order

### Operations Complexity
- **Find**: O(log n) - binary search
- **Add**: O(n) - binary search + insertion/shift
- **Delete**: O(n) - binary search + removal/shift
- **Iterate**: O(n) - sequential access

### Memory Management
- **OrderedValueMap**: Uses object pool for KeyValue structs
- **OrderedManagedMap**: Manages both KeyValue structs and pointed-to objects
- Clear() and Free() methods for explicit cleanup

### Type Safety
- Compile-time enforcement of trivially copyable types where appropriate
- Proper handling of non-trivial types with custom constructors/destructors

## Testing

### Test File: examples/datatype/collection/OrderedMapTest.cpp
Comprehensive test suite covering:
1. **Basic Operations**: Add, find, get, update, delete with ordering verification
2. **Stress Tests**: 1000+ elements with random insertion, verifying order maintained
3. **Non-Trivial Types**: Memory management with complex objects
4. **Edge Cases**: Duplicate keys, reverse insertion, boundary conditions
5. **String Keys**: Lexicographical ordering
6. **OrderedManagedMap**: Pointer management and ordering
7. **Performance**: Characteristics of sorted array approach

### Build Integration
- Added to examples/datatype/collection/CMakeLists.txt
- Follows same pattern as existing MapTest.cpp

## Existing Files (Already Present)
- **inc/hgl/type/OrderedValueSet.h**: Ordered set for trivially copyable types
- **inc/hgl/type/OrderedManagedSet.h**: Ordered set for non-trivial types

## API Compatibility
The OrderedMap API is designed to be similar to:
- Map<K,V> (unordered map)
- UnorderedValueMap<K,V>
- UnorderedManagedMap<K,V>

Key differences:
- Elements are always in sorted order
- Iteration yields sorted results
- Better for range queries
- Slightly slower insertions due to maintaining order

## Usage Examples

### OrderedValueMap Example
```cpp
OrderedValueMap<int, std::string> map;
map.Add(3, "three");
map.Add(1, "one");
map.Add(2, "two");

// Iteration yields: [1,"one"], [2,"two"], [3,"three"]
map.EnumKV([](const int& key, std::string& value) {
    std::cout << key << ": " << value << std::endl;
});
```

### OrderedManagedMap Example
```cpp
OrderedManagedMap<int, MyClass> objMap;
objMap.Add(5, new MyClass("five"));
objMap.Add(1, new MyClass("one"));

// Elements are stored in sorted order: 1, 5
// Destructor automatically deletes all objects
```

## Summary
✅ Created OrderedValueMap for trivially copyable types
✅ Created OrderedManagedMap for non-trivial types with pointer management
✅ Implemented using sorted arrays with binary search (consistent with existing code)
✅ Comprehensive test suite covering all scenarios
✅ Full API compatibility with existing Map classes
✅ Proper memory management and type safety
