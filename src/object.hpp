#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <memory>
#include <vector>

#include "collider.hpp"
#include "math.hpp"
#include "opengl.hpp"
#include "shader.hpp"
#include "util.hpp"

using namespace std;

enum State
{
    Fixed,
    Normal,
    Falling,
};

class Object
{
public:
    State state = State::Normal;

    vec3 pos;
    vec3 velocity = vec3(0.f, 0.f, 0.f);

    unique_ptr<Collider> collider = nullptr;

public:
    void transit_state(State new_state)
    {
        switch (new_state)
        {
            case Fixed: velocity = vec3(0.f, 0.f, 0.f); break;
            case Normal: velocity.z = 0.f; break;
            case Falling: velocity.z -= gravity_acc; break;
        }
        state = new_state;
    }

    void jump()
    {
        if (state == State::Normal)
        {
            transit_state(State::Falling);
            velocity.z = jump_speed;
        }
    }

    [[nodiscard]] virtual const vector<GLfloat>* get_vertices() const
    {
        return nullptr;
    }
};

class ObjectManager : private NonCopy<ObjectManager>
{
private:
    vector<Object*> objects {};

    vector<GLfloat> vertices {};

public:
    void add_object(Object* obj)
    {
        objects.push_back(obj);
    }

    void update()
    {
        vertices.clear();
        for (const Object* obj : objects)
        {
            const vector<GLfloat>* _vertices = obj->get_vertices();
            if (_vertices != nullptr)
            {
                vertices.insert(vertices.end(), _vertices->begin(), _vertices->end());
            }
        }

        // FIXME
    }

    const vector<Object*>& get_objects()
    {
        return objects;
    }
};

#endif
