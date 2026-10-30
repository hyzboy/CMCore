#include <hgl/type/FlatTreeContainer.h>
#include <hgl/io/MemoryOutputStream.h>
#include <hgl/io/MemoryInputStream.h>
#include <hgl/log/Log.h>
#include <hgl/log/Logger.h>

using namespace hgl;

#define CHECK(expr) do { \
    if (!(expr)) { \
        GLogError(u8"CHECK FAILED: %s (at line %d)", #expr, __LINE__); \
        return 1; \
    } \
} while (0)

struct TestTransform
{
    float posX, posY, posZ;
    float scale;
    int32 id;

    bool operator==(const TestTransform &rhs) const
    {
        return posX == rhs.posX && posY == rhs.posY && posZ == rhs.posZ &&
               scale == rhs.scale && id == rhs.id;
    }
};

int main()
{
    hgl::logger::InitLogger(OS_TEXT("FlatTreeContainerTest"));

    GLogInfo(u8"========== FlatTreeContainerTest Start ==========");

    // 1. 基础构建测试
    FlatTreeContainer<TestTransform> tree;
    CHECK(tree.IsEmpty());
    CHECK(tree.GetCount() == 0);

    // 构建层级树：
    //           root (id: 100)
    //          /             \
    //      childA (101)     childB (102)
    //      /        \
    //   subA1 (103) subA2 (104)

    int32 root = tree.CreateNode({0.0f, 0.0f, 0.0f, 1.0f, 100});
    CHECK(root == 0);
    CHECK(tree.IsRoot(root));
    CHECK(tree.GetDepth(root) == 0);

    int32 childA = tree.AddChild(root, {1.0f, 0.0f, 0.0f, 1.0f, 101});
    int32 childB = tree.AddChild(root, {2.0f, 0.0f, 0.0f, 1.0f, 102});
    CHECK(childA == 1);
    CHECK(childB == 2);

    int32 subA1 = tree.AddChild(childA, {1.1f, 0.0f, 0.0f, 1.0f, 103});
    int32 subA2 = tree.AddChild(childA, {1.2f, 0.0f, 0.0f, 1.0f, 104});
    CHECK(subA1 == 3);
    CHECK(subA2 == 4);

    CHECK(tree.GetCount() == 5);
    CHECK(tree.GetTotalBytes() == 5 * sizeof(TestTransform));

    // 2. 原始数组视角（零拷贝访问）测试
    const TestTransform *rawData = tree.GetData();
    CHECK(rawData != nullptr);
    CHECK(rawData[0].id == 100);
    CHECK(rawData[1].id == 101);
    CHECK(rawData[2].id == 102);
    CHECK(rawData[3].id == 103);
    CHECK(rawData[4].id == 104);

    // 通过 operator[] 修改数据
    tree[3].scale = 2.5f;
    CHECK(rawData[3].scale == 2.5f);

    // 3. 树形视角 (NodeView / ConstNodeView) 测试
    auto nodeA = tree.GetNode(childA);
    CHECK(nodeA.IsValid());
    CHECK(nodeA.GetParent() == root);
    CHECK(nodeA.GetFirstChild() == subA1);
    CHECK(nodeA.GetLastChild() == subA2);
    CHECK(nodeA.GetNextSibling() == childB);
    CHECK(nodeA.GetPrevSibling() == FlatTreeContainer<TestTransform>::INVALID_INDEX);
    CHECK(nodeA.GetDepth() == 1);
    CHECK(nodeA.HasChildren());
    CHECK(nodeA.GetChildCount() == 2);
    CHECK(!nodeA.IsRoot());
    CHECK(!nodeA.IsLeaf());

    auto nodeB = tree.GetNode(childB);
    CHECK(nodeB.GetParent() == root);
    CHECK(nodeB.GetPrevSibling() == childA);
    CHECK(nodeB.GetNextSibling() == FlatTreeContainer<TestTransform>::INVALID_INDEX);
    CHECK(nodeB.IsLeaf());
    CHECK(nodeB.GetChildCount() == 0);

    // 4. 兄弟节点添加测试 (AddSibling)
    int32 subA3 = tree.AddSibling(subA2, {1.3f, 0.0f, 0.0f, 1.0f, 105});
    CHECK(subA3 == 5);
    CHECK(tree.GetNode(childA).GetChildCount() == 3);
    CHECK(tree.GetNode(subA2).GetNextSibling() == subA3);
    CHECK(tree.GetNode(subA3).GetPrevSibling() == subA2);
    CHECK(tree.GetNode(childA).GetLastChild() == subA3);

    // 5. 树形遍历测试
    // 先序 DFS 遍历收集 ID
    ValueArray<int32> preOrderIds;
    tree.TraversePreOrder(root, [&](int32, const TestTransform &data) {
        preOrderIds.Add(data.id);
    });
    // 预期顺序: root(100) -> childA(101) -> subA1(103) -> subA2(104) -> subA3(105) -> childB(102)
    CHECK(preOrderIds.GetCount() == 6);
    CHECK(preOrderIds[0] == 100);
    CHECK(preOrderIds[1] == 101);
    CHECK(preOrderIds[2] == 103);
    CHECK(preOrderIds[3] == 104);
    CHECK(preOrderIds[4] == 105);
    CHECK(preOrderIds[5] == 102);

    // 广度优先 BFS (层序) 遍历收集 ID
    ValueArray<int32> bfsIds;
    tree.TraverseBreadthFirst(root, [&](int32, const TestTransform &data) {
        bfsIds.Add(data.id);
    });
    // 预期顺序: root(100) -> childA(101), childB(102) -> subA1(103), subA2(104), subA3(105)
    CHECK(bfsIds.GetCount() == 6);
    CHECK(bfsIds[0] == 100);
    CHECK(bfsIds[1] == 101);
    CHECK(bfsIds[2] == 102);
    CHECK(bfsIds[3] == 103);
    CHECK(bfsIds[4] == 104);
    CHECK(bfsIds[5] == 105);

    // 后序 DFS 遍历测试
    ValueArray<int32> postOrderIds;
    tree.TraversePostOrder(root, [&](int32, const TestTransform &data) {
        postOrderIds.Add(data.id);
    });
    // subA1(103), subA2(104), subA3(105), childA(101), childB(102), root(100)
    CHECK(postOrderIds.GetCount() == 6);
    CHECK(postOrderIds[0] == 103);
    CHECK(postOrderIds[1] == 104);
    CHECK(postOrderIds[2] == 105);
    CHECK(postOrderIds[3] == 101);
    CHECK(postOrderIds[4] == 102);
    CHECK(postOrderIds[5] == 100);

    // 6. 重置父节点 (Reparenting) 测试
    // 将 subA3 从 childA 移至 childB 下
    CHECK(tree.SetParent(subA3, childB));
    CHECK(tree.GetNode(subA3).GetParent() == childB);
    CHECK(tree.GetNode(subA3).GetDepth() == 2);
    CHECK(tree.GetNode(childA).GetChildCount() == 2);
    CHECK(tree.GetNode(childA).GetLastChild() == subA2);
    CHECK(tree.GetNode(childB).GetChildCount() == 1);
    CHECK(tree.GetNode(childB).GetFirstChild() == subA3);

    // 环检测测试：尝试将 root 的 parent 设为 subA1 (应该失败)
    CHECK(!tree.SetParent(root, subA1));
    // 自身作为父节点检测：应该失败
    CHECK(!tree.SetParent(childA, childA));

    // 7. GPU 拓扑烘焙 (BakeGPUTopology) 测试
    CHECK(tree.IsGPUTopologyDirty());
    CHECK(tree.SyncGPUTopology());
    CHECK(!tree.IsGPUTopologyDirty());

    CHECK(tree.GetGPUTopologyCount() == tree.GetCount());
    CHECK(tree.GetMaxDepth() == 2);
    const auto &depthCounts = tree.GetDepthCounts();
    CHECK(depthCounts.GetCount() == 3);
    CHECK(depthCounts[0] == 1); // Depth 0: root
    CHECK(depthCounts[1] == 2); // Depth 1: childA, childB
    CHECK(depthCounts[2] == 3); // Depth 2: subA1, subA2, subA3

    const auto &depthOffsets = tree.GetDepthOffsets();
    CHECK(depthOffsets[0] == 0);
    CHECK(depthOffsets[1] == 1);
    CHECK(depthOffsets[2] == 3);

    const auto *gpuTopo = tree.GetGPUTopologyData();
    CHECK(gpuTopo[root].parent == FlatTreeContainer<TestTransform>::INVALID_INDEX);
    CHECK(gpuTopo[root].depth == 0);
    CHECK(gpuTopo[childA].parent == root);
    CHECK(gpuTopo[childA].depth == 1);
    CHECK(gpuTopo[subA3].parent == childB);
    CHECK(gpuTopo[subA3].depth == 2);

    // 8. 物理压缩删除子树 (RemoveSubtree) 测试
    // 删除 childA 及其子节点 (subA1, subA2)
    CHECK(tree.RemoveSubtree(childA));
    // 原来 6 个节点，删除了 childA, subA1, subA2，剩余 3 个节点：root, childB, subA3
    CHECK(tree.GetCount() == 3);
    // 验证无内存空洞与重映射
    CHECK(tree[0].id == 100); // root
    CHECK(tree[1].id == 102); // childB (原 index 2 移至 1)
    CHECK(tree[2].id == 105); // subA3 (原 index 5 移至 2)

    CHECK(tree.GetNode(0).IsRoot());
    CHECK(tree.GetNode(1).GetParent() == 0);
    CHECK(tree.GetNode(2).GetParent() == 1);
    CHECK(tree.GetNode(1).GetFirstChild() == 2);

    // 重新烘焙并验证 GPU 拓扑
    tree.SyncGPUTopology();
    CHECK(tree.GetGPUTopologyCount() == 3);
    CHECK(tree.GetMaxDepth() == 2);

    // 9. 线性零拷贝序列化与反序列化测试
    io::MemoryOutputStream mos;
    CHECK(tree.Save(&mos));

    io::MemoryInputStream mis(mos.GetData(), mos.Tell());
    FlatTreeContainer<TestTransform> loadedTree;
    CHECK(loadedTree.Load(&mis));

    CHECK(loadedTree.GetCount() == tree.GetCount());
    for (int32 i = 0; i < loadedTree.GetCount(); ++i)
    {
        CHECK(loadedTree[i] == tree[i]);
        CHECK(loadedTree.GetParent(i) == tree.GetParent(i));
        CHECK(loadedTree.GetFirstChild(i) == tree.GetFirstChild(i));
        CHECK(loadedTree.GetLastChild(i) == tree.GetLastChild(i));
        CHECK(loadedTree.GetNextSibling(i) == tree.GetNextSibling(i));
        CHECK(loadedTree.GetPrevSibling(i) == tree.GetPrevSibling(i));
        CHECK(loadedTree.GetDepth(i) == tree.GetDepth(i));
    }

    loadedTree.SyncGPUTopology();
    CHECK(loadedTree.GetMaxDepth() == tree.GetMaxDepth());

    // 10. 森林多根节点与深度分层压力测试
    {
        FlatTreeContainer<TestTransform> forest;
        // 创建 3 棵独立的树
        int32 r1 = forest.CreateNode({0, 0, 0, 1, 1});
        int32 r2 = forest.CreateNode({0, 0, 0, 1, 2});
        int32 r3 = forest.CreateNode({0, 0, 0, 1, 3});

        // 构造较深的分支
        int32 curr = r1;
        for (int i = 0; i < 50; ++i)
        {
            curr = forest.AddChild(curr, {(float)i, 0, 0, 1, 1000 + i});
        }

        // r2 下添加 100 个直接子节点
        for (int i = 0; i < 100; ++i)
        {
            forest.AddChild(r2, {(float)i, 0, 0, 1, 2000 + i});
        }

        CHECK(forest.GetCount() == 3 + 50 + 100);
        CHECK(forest.GetNode(r2).GetChildCount() == 100);

        forest.SyncGPUTopology();
        CHECK(forest.GetMaxDepth() == 50);

        int32 rootTraverseCount = 0;
        forest.TraverseAllRoots([&](int32, const TestTransform &) {
            ++rootTraverseCount;
        });
        CHECK(rootTraverseCount == forest.GetCount());

        // 删除 r1 整棵深树
        CHECK(forest.RemoveSubtree(r1));
        CHECK(forest.GetCount() == 2 + 100);

        // 删除剩余全部节点
        while (!forest.IsEmpty())
        {
            forest.RemoveSubtree(0);
        }
        CHECK(forest.IsEmpty());
        CHECK(forest.GetCount() == 0);
    }

    GLogInfo(u8"========== FlatTreeContainerTest All Passed Successfully! ==========");
    return 0;
}
