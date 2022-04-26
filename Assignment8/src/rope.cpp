#include <iostream>
#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

namespace CGL {

    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, vector<int> pinned_nodes)
    {
        // TODO (Part 1): Create a rope starting at `start`, ending at `end`, and containing `num_nodes` nodes.
        this->masses.resize(0);
        this->springs.resize(0);
        for(int i=0; i<num_nodes;i++){
            Mass *t = new Mass(start + (end-start)*(i/(num_nodes-1)), node_mass, false);
            this->masses.push_back(t);
            if(i!=0){
                Spring *tt = new Spring(this->masses[i-1],this->masses[i],k);
                this->springs.push_back(tt);
            }
        }
        for (auto &i : pinned_nodes) {
            masses[i]->pinned = true;
        }
    }

    void Rope::simulateEuler(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {   
            // TODO (Part 2): Use Hooke's law to calculate the force on a node
            auto b_a = s->m2->position - s->m1->position;
            auto f = -s->k * b_a.unit() * (b_a.norm()-s->rest_length);
            s->m1->forces += f;
            s->m2->forces += -f;
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                // TODO (Part 2): Add the force due to gravity, then compute the new velocity and position
                m->forces+=gravity;
                // TODO (Part 2): Add global damping
            }
            
            // Reset all forces on each mass
            m->forces = Vector2D(0, 0);
        }
    }

    void Rope::simulateVerlet(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            // TODO (Part 3): Simulate one timestep of the rope using explicit Verlet （solving constraints)
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                Vector2D temp_position = m->position;
                // TODO (Part 3.1): Set the new position of the rope mass
                
                // TODO (Part 4): Add global Verlet damping
            }
        }
    }
}
