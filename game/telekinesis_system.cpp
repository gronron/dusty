#include "telekinesis_system.hpp"

void	TelekinesisSystem::add_body(Body * const body)
{
	_bodies.push_back(body);
}

void	TelekinesisSystem::remove_body(Body * const)
{
	_bodies.remove(body);
}

int	TelekinesisSystem::start_effect(Effect const &effect)
{
	_effects.push_back(effect);
}

void	TelekinesisSystem::update_effect(int const effect_id, Effect const &effect)
{
	
}

void	TelekinesisSystem::stop_effect(int const effect_id)
{
	
}

void	TelekinesisSystem::update(float const delta)
{
	for (unsigned int i = 0; i < _effects.size(); ++i)
	{
		_effects[i].function(_effects[i], _bodies);
	}
}
