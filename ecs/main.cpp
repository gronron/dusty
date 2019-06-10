#include "ecs.hpp"

struct Position
{
    float x;
    float y;
    float z;
};

struct Speed
{
    float x;
    float y;
    float z;
};

struct Acceleration
{
    float x;
    float y;
    float z;
};

class Creator final : public System
{
public:
    void update()
    {
        for (size_t i = 0; i < 1000000; ++i)
            EntitiesRegistry::create(this, {});
    }

    void creation_callback(Entity const creator, Entity const created) override
    {
		created.add<Position>();
		if (created.index() % 2)
		{
			created.add<Speed>();
			created.add<Acceleration>();
		}
    }
};

class Destructor final : public System
{
public:
    void update()
    {
        for_each([](Entity const entity, Position const &)
        {
            EntitiesRegistry::destroy(entity);
        });
    }
};

class Accelerator final : public System
{
public:
    void update()
    {
        for_each([](Speed & speed, Acceleration const & acc)
        {
            speed.x += acc.x;
            speed.y += acc.y;
            speed.z += acc.z;
        });
    }
};

class Mover final : public System
{
public:
    void update()
    {
        for_each([](Position & pos, Speed const & speed)
        {
            pos.x += speed.x;
            pos.y += speed.y;
            pos.z += speed.z;
        });
    }
};

int main()
{
    Creator     crt;
    Destructor  dtc;
    Accelerator acc;
    Mover       mv;

    crt.update();

    EntitiesRegistry::end_frame_process();

    for (size_t i = 0; i < 2000; ++i)
    {
        acc.update();
        mv.update();
		EntitiesRegistry::end_frame_process();
    }

    dtc.update();

    EntitiesRegistry::end_frame_process();

	return 0;
}
