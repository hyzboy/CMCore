#include <vector>
#include <unordered_map>
#include <iostream>
#include <cstdint>
#include <cassert>

// 高度图DAG节点
template<typename T> struct DAGNode
{
    T height;
    int children[4];

    DAGNode(float h = 0.0f) : height(h) {
        for (int i = 0; i < 4; ++i) children[i] = -1;
    }
};

// DAG管理器
template<typename T> 
class HeightDAG
{
public:
    std::vector<DAGNode<T>> nodes;
    int root;

    HeightDAG(const std::vector<std::vector<T>> &heightmap, int blockSize = 4) {
        int N = heightmap.size();
        assert(N > 0 && heightmap[0].size() == N && (N & (blockSize - 1)) == 0);

        root = build(heightmap, 0, 0, N, blockSize);
    }

    T pickHeight(int x, int y, int N) const
    {
        int nodeIdx = root;
        int size = N;
        int bx = x, by = y;

        while (true) {
            const DAGNode &node = nodes[nodeIdx];
            if (node.children[0] == -1) {
                return node.height;
            }
            size /= 2;
            int cx = bx / size, cy = by / size;
            int idx = cy * 2 + cx;
            nodeIdx = node.children[idx];
            bx %= size; by %= size;
        }
    }

private:
    struct BlockHash {
        size_t operator()(const std::vector<T> &v) const {
            size_t h = v.size();
            for (T f : v) h = h * 31 + std::hash<T>()(f);
            return h;
        }
    };

    std::unordered_map<std::vector<float>, int, BlockHash> blockMap;

    int build(const std::vector<std::vector<float>> &hm, int x, int y, int size, int blockSize) {
        if (size == blockSize) {
            float h0 = hm[y][x];
            bool same = true;
            for (int i = 0; i < blockSize && same; ++i)
                for (int j = 0; j < blockSize && same; ++j)
                    if (hm[y + i][x + j] != h0) same = false;

            if (same) {
                int idx = nodes.size();
                nodes.emplace_back(h0);
                return idx;
            }
        }

        int half = size / 2;
        std::vector<int> childIdx(4);
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                childIdx[i * 2 + j] = build(hm, x + j * half, y + i * half, half, blockSize);

        float h0 = nodes[childIdx[0]].height;
        bool same = true;
        for (int k = 1; k < 4; ++k) {
            if (nodes[childIdx[k]].height != h0 ||
                nodes[childIdx[k]].children[0] != -1) {
                same = false;
                break;
            }
        }

        if (same && nodes[childIdx[0]].children[0] == -1) {
            int idx = nodes.size();
            nodes.emplace_back(h0);
            return idx;
        }

        int idx = nodes.size();
        nodes.emplace_back();
        for (int k = 0; k < 4; ++k) nodes[idx].children[k] = childIdx[k];
        nodes[idx].height = 0;
        return idx;
    }
};

int main() {
    std::vector<std::vector<float>> heightmap(8, std::vector<float>(8, 1.0f));
    for (int y = 2; y < 6; ++y)
        for (int x = 2; x < 6; ++x)
            heightmap[y][x] = 2.0f;

    HeightDAG dag(heightmap, 2);

    std::cout << "Height at (1,1): " << dag.pickHeight(1, 1, 8) << std::endl;
    std::cout << "Height at (3,3): " << dag.pickHeight(3, 3, 8) << std::endl;
    std::cout << "Height at (7,7): " << dag.pickHeight(7, 7, 8) << std::endl;

    std::cout << "Total DAG nodes: " << dag.nodes.size() << std::endl;
    return 0;
}
