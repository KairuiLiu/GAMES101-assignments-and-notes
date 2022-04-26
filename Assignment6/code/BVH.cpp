#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;

    // root = recursiveBuild(primitives);
    root = recursiveBuildSAH(primitives);

    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}

BVHBuildNode* BVHAccel::recursiveBuildSAH(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    Bounds3 bounds;

    // 计算每个Object的BoundingBox并合并成大BoundingBox
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    // 该范围内只有一个物体, 那么这个Node就是叶子节点
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    // 该范围内只有两个物体, 他们各自是叶子节点
    else if (objects.size() == 2) {
        node->left = recursiveBuildSAH(std::vector{objects[0]});
        node->right = recursiveBuildSAH(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        // 获取重心范围
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        // 判断重心范围盒子哪条边长 1/2/3
        int dim = centroidBounds.maxExtent();
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        auto beginning = objects.begin();
        auto ending = objects.end();

        // another SAH RELEASE 
        int len = objects.size(), l = 0, r = len-1;
        while(r-l>=2){
            int m = (r+l)>>1;
            int ml = m-1;
            int mr = m+1;
            float costml, costmr;
            switch (dim) {
                case 0: 
                    costml = ml * ((objects[ml])->getBounds().Centroid().x - centroidBounds.pMin.x) + (len-1-ml) * (centroidBounds.pMax.x - (objects[ml])->getBounds().Centroid().x); 
                    costmr = mr * ((objects[mr])->getBounds().Centroid().x - centroidBounds.pMin.x) + (len-1-mr) * (centroidBounds.pMax.x - (objects[mr])->getBounds().Centroid().x); 
                    break;
                case 1: 
                    costml = ml * ((objects[ml])->getBounds().Centroid().y - centroidBounds.pMin.y) + (len-1-ml) * (centroidBounds.pMax.y - (objects[ml])->getBounds().Centroid().y); 
                    costmr = mr * ((objects[mr])->getBounds().Centroid().y - centroidBounds.pMin.y) + (len-1-mr) * (centroidBounds.pMax.y - (objects[mr])->getBounds().Centroid().y); 
                    break;
                case 2: 
                    costml = ml * ((objects[ml])->getBounds().Centroid().z - centroidBounds.pMin.z) + (len-1-ml) * (centroidBounds.pMax.z - (objects[ml])->getBounds().Centroid().z); 
                    costmr = mr * ((objects[mr])->getBounds().Centroid().z - centroidBounds.pMin.z) + (len-1-mr) * (centroidBounds.pMax.z - (objects[mr])->getBounds().Centroid().z); 
                    break;
            }
            if(costml<costmr)r=m;
            else l=m;
        }
        auto middling = objects.begin()+((l+r)>>1);

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuildSAH(leftshapes);
        node->right = recursiveBuildSAH(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}


Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection
    Intersection isect;
    if(node == nullptr)return isect;
    auto invDir=Vector3f(1.0/ray.direction.x,1.0/ray.direction.y,1.0/ray.direction.z);
    std::array<int, 3UL> dirIsNeg = {int(ray.direction.x>0),int(ray.direction.y>0),int(ray.direction.z>0)};
    Intersection interL = BVHAccel::getIntersection(node->left, ray);
    Intersection interR = BVHAccel::getIntersection(node->right, ray);
    isect = interL.distance<interR.distance?interL:interR;
    if(node->left==nullptr&&node->right==nullptr)isect = node->object->getIntersection(ray);
    return isect;
}