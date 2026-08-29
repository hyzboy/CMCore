/**
 * TypedBlockAllocatorTest.cpp
 *
 * Random multi-material block allocator test
 *
 * Simulates multiple "material" types (trivially-copyable structs of
 * different sizes), each bound to its own TypedBlockAllocator<T>, all
 * sharing the same BlockPool.
 * Test scenarios:
 *   - BlockPool basic operations (sentinel block 0, contiguous multi-block
 *     acquire/release, data read/write, offset calculation)
 *   - Multi-material interleaved random acquire/release stress test
 *     (globally unique block indices across types, data integrity)
 *   - ReleaseAll bulk reclaim (whole segment returned to the BlockPool, then
 *     reusable by other types)
 *   - Acquire returns the 0 sentinel when the pool is exhausted
 *   - Detach gives up ownership
 */

#include<hgl/type/TypedBlockAllocator.h>
#include<iostream>
#include<vector>
#include<set>
#include<cstddef>

using namespace hgl;
using namespace std;

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            tests_passed++; \
            std::cout << "  [PASS] " << message << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "  [FAIL] " << message << "  (line " << __LINE__ << ")" << std::endl; \
        } \
    } while(0)

// ---------------- Deterministic PRNG (mulberry32) ----------------

static uint32 g_rng = 0x9E3779B9u;

static uint32 next_rnd()
{
    uint32 z = (g_rng += 0x6D2B79F5u);
    z = (z ^ (z >> 15)) * (z | 1u);
    z ^= z + ((z ^ (z >> 7)) * (z | 61u));
    return z ^ (z >> 14);
}

// ---------------- Material structs (16/32/48/64 bytes, all 16-aligned) ----------------

struct MaterialBase16 { uint32 color; float data[3];  };   // 16 bytes, 1 block
struct MaterialPBR32  { uint32 color; float data[7];  };   // 32 bytes, 2 blocks
struct MaterialTex48  { uint32 color; float data[11]; };   // 48 bytes, 3 blocks
struct MaterialBig64  { uint32 color; float data[15]; };   // 64 bytes, 4 blocks

static_assert(sizeof(MaterialBase16) == 16, "MaterialBase16 must be 16 bytes");
static_assert(sizeof(MaterialPBR32)  == 32, "MaterialPBR32 must be 32 bytes");
static_assert(sizeof(MaterialTex48)  == 48, "MaterialTex48 must be 48 bytes");
static_assert(sizeof(MaterialBig64)  == 64, "MaterialBig64 must be 64 bytes");

// ---------------- Common acquire/release helpers ----------------

// Assertions inside hot loops count silently to avoid huge output; summarized at the end.
template<typename T>
static void typed_acquire(TypedBlockAllocator<T> &ta, vector<uint32> &held, set<uint32> &global,
                          uint64 &checks, uint64 &fails)
{
    uint32 idx = ta.Acquire();

    if(idx == 0)            // pool exhausted; skip this op (not an error)
        return;

    checks++;
    if(!global.insert(idx).second)          // globally unique block index across types
        fails++;

    T *p = ta.GetData(idx);

    checks++;
    if(p == nullptr)
    {
        fails++;
    }
    else
    {
        checks++;
        if((reinterpret_cast<size_t>(p) & 15) != 0)     // object address 16-byte aligned
            fails++;

        p->color = idx;             // write block index as a marker for later integrity checks
        p->data[0] = (float)idx;
    }

    held.push_back(idx);
}

template<typename T>
static void typed_release(TypedBlockAllocator<T> &ta, vector<uint32> &held, set<uint32> &global,
                          uint32 pos, uint64 &checks, uint64 &fails)
{
    uint32 idx = held[pos];

    checks++;
    if(!ta.Release(idx))
        fails++;

    global.erase(idx);
    held[pos] = held.back();
    held.pop_back();
}

// ---------------- TEST 1: BlockPool basics ----------------

static void test_block_pool_basic()
{
    cout << "\n========================================" << endl;
    cout << "TEST 1: BlockPool basic operations" << endl;
    cout << "========================================\n" << endl;

    MemoryAllocator ma;
    BlockPool pool;

    TEST_ASSERT(pool.Init(&ma, 100, 16), "small pool init succeeded");
    TEST_ASSERT(pool.GetBlockSize() == 16, "block size is 16");
    TEST_ASSERT(pool.GetMaxCount() == 100, "total block count is 100");
    TEST_ASSERT(pool.GetFreeCount() == 99, "block 0 is reserved");

    TEST_ASSERT(pool.Acquire(0) == 0, "Acquire(0) returns 0");
    TEST_ASSERT(pool.Release(0) == false, "Release(0) fails");

    uint32 a = pool.Acquire(3);
    TEST_ASSERT(a != 0, "acquired 3 blocks");
    TEST_ASSERT(a >= 1, "block index >= 1 (block 0 never used)");

    uint32 b = pool.Acquire(5);
    TEST_ASSERT(b != 0 && b != a, "acquired 5 more blocks, non-overlapping");
    TEST_ASSERT(pool.GetFreeCount() == 99 - 8, "free count decremented correctly");

    TEST_ASSERT(pool.Release(a), "released a");
    TEST_ASSERT(pool.Release(b), "released b");
    TEST_ASSERT(pool.Release(a) == false, "double release fails");
    TEST_ASSERT(pool.GetFreeCount() == 99, "all blocks returned");

    // Data read/write and offset calculation
    uint32 c = pool.Acquire(1);
    uint32 *p = pool.GetData<uint32>(c);
    TEST_ASSERT(p != nullptr, "GetData valid");
    TEST_ASSERT(pool.GetBase() != nullptr, "GetBase non-null");
    TEST_ASSERT(pool.GetData(c) == (void *)((uint8 *)pool.GetBase() + c * 16), "GetData offset = base + index*block_size");

    if(p)
    {
        *p = 0xDEADBEEFu;
        TEST_ASSERT(*pool.GetData<uint32>(c) == 0xDEADBEEFu, "write/read round-trip");
    }
    pool.Release(c);
}

// ---------------- TEST 2: Multi-material interleaved random stress ----------------

static void test_multi_material_interleaved(BlockPool *pool)
{
    cout << "\n========================================" << endl;
    cout << "TEST 2: multi-material interleaved random stress" << endl;
    cout << "========================================\n" << endl;

    TypedBlockAllocator<MaterialBase16> ta0;  ta0.Init(pool, 16);
    TypedBlockAllocator<MaterialPBR32>  ta1;  ta1.Init(pool, 8);
    TypedBlockAllocator<MaterialTex48>  ta2;  ta2.Init(pool, 6);
    TypedBlockAllocator<MaterialBig64>  ta3;  ta3.Init(pool, 4);

    TEST_ASSERT(ta0.GetSlotBlocks() == 1, "16B struct occupies 1 block");
    TEST_ASSERT(ta1.GetSlotBlocks() == 2, "32B struct occupies 2 blocks");
    TEST_ASSERT(ta2.GetSlotBlocks() == 3, "48B struct occupies 3 blocks");
    TEST_ASSERT(ta3.GetSlotBlocks() == 4, "64B struct occupies 4 blocks");

    vector<uint32> held[4];
    set<uint32> global;
    uint64 stress_checks = 0;
    uint64 stress_fails = 0;

    const int OPS = 40000;

    for(int i = 0; i < OPS; i++)
    {
        uint32 r = next_rnd();
        int which = (int)(r & 3);
        bool acquire = ((r >> 2) & 1) != 0;

        if(acquire || held[which].empty())
        {
            switch(which)
            {
            case 0: typed_acquire(ta0, held[0], global, stress_checks, stress_fails); break;
            case 1: typed_acquire(ta1, held[1], global, stress_checks, stress_fails); break;
            case 2: typed_acquire(ta2, held[2], global, stress_checks, stress_fails); break;
            case 3: typed_acquire(ta3, held[3], global, stress_checks, stress_fails); break;
            }
        }
        else
        {
            uint32 pos = next_rnd() % (uint32)held[which].size();
            switch(which)
            {
            case 0: typed_release(ta0, held[0], global, pos, stress_checks, stress_fails); break;
            case 1: typed_release(ta1, held[1], global, pos, stress_checks, stress_fails); break;
            case 2: typed_release(ta2, held[2], global, pos, stress_checks, stress_fails); break;
            case 3: typed_release(ta3, held[3], global, pos, stress_checks, stress_fails); break;
            }
        }
    }

    cout << "    held: 16B=" << held[0].size() << " 32B=" << held[1].size()
         << " 48B=" << held[2].size() << " 64B=" << held[3].size() << endl;
    cout << "    batches: 16B=" << ta0.GetBatchCount() << " 32B=" << ta1.GetBatchCount()
         << " 48B=" << ta2.GetBatchCount() << " 64B=" << ta3.GetBatchCount() << endl;

    // Data integrity: every still-held block must read back its written marker (counted silently)
    auto verify = [&](auto &ta, auto &held_vec){
        for(uint32 idx : held_vec)
        {
            auto *p = ta.GetData(idx);
            stress_checks++;
            if(p == nullptr)
                stress_fails++;
            else
            {
                stress_checks++;
                if(p->color != idx)
                    stress_fails++;
            }
        }
    };
    verify(ta0, held[0]);
    verify(ta1, held[1]);
    verify(ta2, held[2]);
    verify(ta3, held[3]);

    size_t total_held = held[0].size() + held[1].size() + held[2].size() + held[3].size();
    TEST_ASSERT(global.size() == total_held, "global set size matches total held across allocators");
    TEST_ASSERT(stress_fails == 0, "all internal stress checks passed (" << stress_checks << " checks)");

    uint32 free_before = pool->GetFreeCount();
    ta0.ReleaseAll();
    ta1.ReleaseAll();
    ta2.ReleaseAll();
    ta3.ReleaseAll();
    uint32 free_after = pool->GetFreeCount();

    TEST_ASSERT(free_after >= free_before, "pool free count rises after releasing all");
}

// ---------------- TEST 3: ReleaseAll bulk reclaim ----------------

static void test_release_all_reclaim(BlockPool *pool)
{
    cout << "\n========================================" << endl;
    cout << "TEST 3: ReleaseAll bulk reclaim" << endl;
    cout << "========================================\n" << endl;

    TypedBlockAllocator<MaterialBig64> ta;
    ta.Init(pool, 32);          // each batch = 32 slots * 4 blocks = 128 blocks

    vector<uint32> held;
    for(int i = 0; i < 32; i++)
    {
        uint32 idx = ta.Acquire();
        TEST_ASSERT(idx != 0, "Acquire succeeded");
        held.push_back(idx);
    }

    TEST_ASSERT(ta.GetBatchCount() == 1, "one full batch forms a single contiguous segment");
    uint32 free_before = pool->GetFreeCount();

    ta.ReleaseAll();

    TEST_ASSERT(ta.GetBatchCount() == 0, "batch count cleared after ReleaseAll");
    TEST_ASSERT(ta.GetFreeCount() == 0, "free list cleared after ReleaseAll");
    TEST_ASSERT(pool->GetFreeCount() == free_before + 128, "128 blocks returned to the pool");

    // blocks are reusable by other types after being returned
    TypedBlockAllocator<MaterialBase16> tb;
    tb.Init(pool, 8);
    uint32 idx = tb.Acquire();
    TEST_ASSERT(idx != 0, "reclaimed blocks reusable by another type");
}

// ---------------- TEST 4: Pool exhaustion sentinel ----------------

static void test_exhaustion_sentinel()
{
    cout << "\n========================================" << endl;
    cout << "TEST 4: pool exhaustion sentinel" << endl;
    cout << "========================================\n" << endl;

    MemoryAllocator ma;
    BlockPool pool;
    TEST_ASSERT(pool.Init(&ma, 64, 16), "64-block pool init succeeded");

    TypedBlockAllocator<MaterialBig64> ta;   // 4 blocks per slot
    ta.Init(&pool, 16);                      // needs 16*4 = 64 blocks, but only 63 available

    TEST_ASSERT(ta.Acquire() == 0, "Acquire returns 0 sentinel when the pool is exhausted");
}

// ---------------- TEST 5: Detach gives up ownership ----------------

static void test_detach()
{
    cout << "\n========================================" << endl;
    cout << "TEST 5: Detach gives up ownership" << endl;
    cout << "========================================\n" << endl;

    MemoryAllocator ma;
    BlockPool pool;
    pool.Init(&ma, 256, 16);

    TypedBlockAllocator<MaterialBase16> ta;
    ta.Init(&pool, 8);

    uint32 idx = ta.Acquire();
    TEST_ASSERT(idx != 0, "Acquire succeeded");

    ta.Detach();

    TEST_ASSERT(ta.GetBatchCount() == 0, "batch count cleared after Detach");
    TEST_ASSERT(ta.GetFreeCount() == 0, "free list cleared after Detach");
    TEST_ASSERT(ta.GetData(idx) == nullptr, "GetData returns nullptr after Detach");
    // Note: pool still alive; ta destructor safely skips ReleaseAll because pool was cleared
}

// ---------------- Main ----------------

int os_main(int, os_char **)
{
    cout << "\n=====================================" << endl;
    cout << "TypedBlockAllocator random multi-material test" << endl;
    cout << "=====================================" << endl;

    test_block_pool_basic();

    {
        MemoryAllocator ma;
        BlockPool pool;
        TEST_ASSERT(pool.Init(&ma, 65536, 16), "65536-block pool init succeeded");
        test_multi_material_interleaved(&pool);
        test_release_all_reclaim(&pool);
    }

    test_exhaustion_sentinel();
    test_detach();

    cout << "\n========================================" << endl;
    cout << "TEST SUMMARY" << endl;
    cout << "========================================" << endl;
    cout << "Passed: " << tests_passed << endl;
    cout << "Failed: " << tests_failed << endl;

    if(tests_failed == 0)
        cout << "\nALL TESTS PASSED!" << endl;
    else
        cout << "\nSOME TESTS FAILED!" << endl;

    return tests_failed > 0 ? 1 : 0;
}
