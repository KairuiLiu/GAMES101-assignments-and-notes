//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"

const float eps = 0.0005f;

void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    
    auto hit_p = intersect(ray);
	if (!hit_p.happened) return Vector3f(0,0,0);
    if(hit_p.m->hasEmission()) return hit_p.m->getEmission();

    auto n_p = hit_p.normal.normalized();
    auto w_o = -ray.direction;
    auto pos_p = hit_p.coords;

    Intersection hit_x; 
    float pdf_l;
    sampleLight(hit_x,pdf_l);
    auto pos_x = hit_x.coords;
    auto xp = (pos_p - pos_x);
    auto w_i_inv = xp.normalized();
    auto w_i_inv2 = normalize(xp);
    auto w_i = (-xp).normalized();
    auto n_x = hit_x.normal;

    Vector3f l_indir(0,0,0), l_dir(0,0,0);

    if(intersect(Ray(pos_p, w_i)).distance - (-xp).norm() > -eps) {
        float cos1 = std::max(dotProduct(w_i, n_p), 0.0f);
        float cos2 = std::max(dotProduct(w_i_inv, n_x), 0.0f);
        l_dir = hit_x.emit * hit_p.m->eval(w_i, w_o, n_p) * cos1 * cos2 / (dotProduct(-xp, -xp) * pdf_l);
    }

    if(get_random_float()<RussianRoulette){
        Vector3f w_indir = hit_p.m->sample(w_o, n_p).normalized();
        auto pdf_indir = hit_p.m->pdf(w_o, w_indir, n_p);
        if(pdf_indir>eps){
            auto nextHit = intersect(Ray(hit_p.coords, w_indir));
            if (nextHit.happened && !nextHit.m->hasEmission()){
                auto cost = std::max(.0f, dotProduct(w_indir, n_p));
                l_indir = 1.0*castRay(Ray(pos_p, w_indir), depth+1) * hit_p.m->eval(w_indir, w_o, n_p) * cost / (pdf_indir * Scene::RussianRoulette);
            }
        }
    }

    return l_indir+l_dir;
}

