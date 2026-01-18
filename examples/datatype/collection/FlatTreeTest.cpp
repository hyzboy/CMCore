#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <hgl/type/FlatTree.h>

using Mat4 = glm::mat4;

struct SceneNode
{
    Mat4 local;   // local transform
    Mat4 world;   // cached world transform
    std::string name;

    SceneNode() : local(1.0f), world(1.0f) {}
    SceneNode(const std::string &n, const Mat4 &l=Mat4(1.0f)) : local(l), world(1.0f), name(n) {}
};

int main()
{
    hgl::FlatTree<SceneNode> scene;

    auto root = scene.AddRoot(SceneNode("root", glm::translate(Mat4(1.0f), glm::vec3(0.0f,0.0f,0.0f))));
    auto a = scene.AddChild(root, SceneNode("a", glm::translate(Mat4(1.0f), glm::vec3(1.0f,0.0f,0.0f))));
    auto b = scene.AddChild(root, SceneNode("b", glm::translate(Mat4(1.0f), glm::vec3(-1.0f,0.0f,0.0f))));
    auto a1 = scene.AddChild(a, SceneNode("a1", glm::translate(Mat4(1.0f), glm::vec3(0.0f,2.0f,0.0f))));
    auto a2 = scene.AddChild(a, SceneNode("a2", glm::translate(Mat4(1.0f), glm::vec3(0.0f,-2.0f,0.0f))));

    // compute world transforms by traversing and propagating parent
    auto compute_world = [&](size_t idx, const SceneNode &node){
        auto p = scene.GetParent(idx);
        if(!p.has_value())
        {
            // root
            scene.GetNode(idx).value.world = scene.GetNode(idx).value.local;
        }
        else
        {
            scene.GetNode(idx).value.world = scene.GetNode(p.value()).value.world * scene.GetNode(idx).value.local;
        }
    };

    // First compute top-down
    scene.TraversePreOrder(root, [&](size_t idx, const SceneNode &){ compute_world(idx, scene.GetNode(idx).value); });

    std::cout << "World transforms after initial compute:\n";
    scene.TraversePreOrder(root, [&](size_t idx, const SceneNode &n){
        const auto &w = scene.GetNode(idx).value.world;
        std::cout << "  " << idx << ": " << n.name << " world[3]=(" << w[3][0] << "," << w[3][1] << "," << w[3][2] << ")\n";
    });

    // Now change 'a' local transform and recompute subtree
    std::cout << "\nModify local transform of node 'a' (translate +5 on X)...\n";
    scene.GetNode(a).value.local = glm::translate(scene.GetNode(a).value.local, glm::vec3(5.0f,0.0f,0.0f));

    // recompute world only for subtree of 'a'
    // first ensure 'a' world updated from parent
    compute_world(a, scene.GetNode(a).value);
    // then traverse subtree of a and update
    scene.TraversePreOrder(a, [&](size_t idx, const SceneNode &){ if(idx!=a) compute_world(idx, scene.GetNode(idx).value); });

    std::cout << "World transforms after modifying 'a':\n";
    scene.TraversePreOrder(root, [&](size_t idx, const SceneNode &n){
        const auto &w = scene.GetNode(idx).value.world;
        std::cout << "  " << idx << ": " << n.name << " world[3]=(" << w[3][0] << "," << w[3][1] << "," << w[3][2] << ")\n";
    });

    return 0;
}
