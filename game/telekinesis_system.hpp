#pragma once

class TelekinesisSystem
{
public:

	using Function = void (*)(Effect const &, unsigned int const count ,Body const * const);

	struct	Effect
	{
		vec<float, 4>	origin;
		union
		{
			vec<float, 4>	destination;
			vec<float, 4>	direction;
		};
		float			power;
		float			decay;
		Function		function;
	};
	
	std::vector<Effetc>	_effects;	
	std::vector<Body *>	_bodies;
	
	TelekinesisSystem();
	~TelekinesisSystem();
	
	void	add_body(Body * const);
	void	remove_body(Body * const);
	
	int start_effect(Effect const &);
	update_effect(int const, Effect const &);
	stop_effect(int const);
	
	void	update(float const delta);
};
