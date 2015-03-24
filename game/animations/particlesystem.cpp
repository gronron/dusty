#include "math/vec_util.hpp"
#include "mt/mt19937.hpp"
#include "configmanager.hpp"
#include "particlesystem.hpp"
#include <iostream>

Particlesystem::Particlesystem(float d, bool *c, std::string const &name, Body *b) : Animation(d, c), bd(b), attractor(false), spawnrate(0.0f), spawntime(0.0f), time(0.0f)
{
	Dfnode const	*nd = Configmanager::get_instance().get("particle.df");

	nd = nd->safe_get(name, Dfnode::BLOCK, 6);
	attractor = nd->safe_get("attractor", Dfnode::INT, 1)->i[0];
	spawnrate = nd->safe_get("spawnrate", Dfnode::FLOAT, 1)->f[0];
	spawntime = 1.0f / spawnrate;
	cmin = *(vec<float, 3> *)nd->safe_get("cmin", Dfnode::FLOAT, 3)->f;
	cmax = *(vec<float, 3> *)nd->safe_get("cmax", Dfnode::FLOAT, 3)->f;
	fade = *(vec<float, 2> *)nd->safe_get("fade", Dfnode::FLOAT, 2)->f;
	speed = *(vec<float, 2> *)nd->safe_get("speed", Dfnode::FLOAT, 2)->f;
}

Particlesystem::Particlesystem(float d, std::string const &name, vec<float, 2> const &v) : Animation(d, 0), bd(0), attractor(false), spawnrate(0.0f), spawntime(0.0f), time(0.0f)
{
	unsigned int	n;

	Dfnode const	*nd = Configmanager::get_instance().get("particle.df");

	nd = nd->safe_get(name, Dfnode::BLOCK, 6);
	spawnrate = 0.0f;
	spawntime = 0.0f;
	attractor = nd->safe_get("attractor", Dfnode::INT, 1)->i[0];
	n = nd->safe_get("number", Dfnode::INT, 1)->i[0];
	cmin = *(vec<float, 3> *)nd->safe_get("cmin", Dfnode::FLOAT, 3)->f;
	cmax = *(vec<float, 3> *)nd->safe_get("cmax", Dfnode::FLOAT, 3)->f;
	fade = *(vec<float, 2> *)nd->safe_get("fade", Dfnode::FLOAT, 2)->f;
	speed = *(vec<float, 2> *)nd->safe_get("speed", Dfnode::FLOAT, 2)->f;

	for (unsigned int i = 0; i < n; ++i)
	{
		Particle	prtcl;

		prtcl.color[0] = (float)MT().genrand_real1(cmin[0], cmax[0]);
		prtcl.color[1] = (float)MT().genrand_real1(cmin[1], cmax[1]);
		prtcl.color[2] = (float)MT().genrand_real1(cmin[2], cmax[2]);
		prtcl.color[3] = 1.0f;

		prtcl.fade =  (float)MT().genrand_real1(fade[0], fade[1]);
		prtcl.loc = v;
		prtcl.spd[0] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
		prtcl.spd[1] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
		prtcl.spd = Sgl::unit(prtcl.spd) * (float)MT().genrand_real1(speed[0], speed[1]);

		if (attractor)
		{
			prtcl.loc += prtcl.spd * prtcl.fade;
			prtcl.spd = -prtcl.spd;
			prtcl.color[3] = 0.0f;
		}

		_particlelist.push_back(prtcl);
	}
}

Particlesystem::~Particlesystem()
{

}

void	Particlesystem::post_instanciation(Renderer *r)
{
	Animation::post_instanciation(r);
	tex = _rdr->load_texture("particle.png");
}

bool								Particlesystem::tick(float delta)
{
	std::list<Particle>::iterator	i;

	std::cout << "pst" << std::endl;
	if (spawnrate != 0.0f)
	{
		for (time += delta; time > spawntime; time -= spawntime)
		{
			Particle	prtcl;

			prtcl.color[0] = (float)MT().genrand_real1(cmin[0], cmax[0]);
			prtcl.color[1] = (float)MT().genrand_real1(cmin[1], cmax[1]);
			prtcl.color[2] = (float)MT().genrand_real1(cmin[2], cmax[2]);
			prtcl.color[3] = 1.0f;

			prtcl.fade =  (float)MT().genrand_real1(fade[0], fade[1]);
			prtcl.loc = bd->loc;
			prtcl.spd[0] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
			prtcl.spd[1] = (float)(MT().genrand_real1() * 2.0f - 1.0f);
			prtcl.spd = Sgl::unit(prtcl.spd) * (float)MT().genrand_real1(speed[0], speed[1]);

			if (attractor)
			{
				prtcl.loc += prtcl.spd * prtcl.fade;
				prtcl.spd = -prtcl.spd;
				prtcl.color[3] = 0.0f;
			}

			_particlelist.push_back(prtcl);
		}
	}

	for(i = _particlelist.begin(); i != _particlelist.end();)
	{
		i->loc += i->spd * delta;
		if (attractor)
		{
			i->color[3] += delta / i->fade;
			if (i->color[3] > 1.0f)
				i = _particlelist.erase(i);
			else
				++i;
		}
		else
		{
			i->color[3] -= delta / i->fade;
			if (i->color[3] <= 0.0f)
				i = _particlelist.erase(i);
			else
				++i;
		}
	}
	if (spawnrate == 0.0f && _particlelist.empty())
		return (false);
	else
		return (true);
}

void								Particlesystem::draw()
{
	std::list<Particle>::iterator	i;
	vec<float, 2>					s;
	s = 0.075f;

	std::cout << "psd" << std::endl;
	for(i = _particlelist.begin(); i != _particlelist.end(); ++i)
		_rdr->draw_texture(tex, i->loc, s, i->color);
}

void	Particlesystem::stop()
{
	spawnrate = 0.0f;
	if (cleared)
	{
		*cleared = true;
		cleared = 0;
		bd = 0;
	}
}

void	Particlesystem::reverse()
{
	attractor = !attractor;
	for(std::list<Particle>::iterator i = _particlelist.begin(); i != _particlelist.end(); ++i)
		i->spd = -i->spd;
}
