#include <sstream>
#include "player.hpp"
#include "renderer.hpp"
#include "hud.hpp"
#include <iostream>

Hud::Hud(float d, bool *c, Player *p) : Animation(d, c), plr(p)
{

}

Hud::~Hud()
{

}

bool	Hud::tick(float)
{
	return (true);
}

void	Hud::draw()
{
	std::ostringstream	str;
	vec<float, 2>		l;
	vec<float, 2>		s;
	vec<float, 4>		c = {{1.0f, 0.5f, 0.0f, 1.0f}};

	std::cout << "hudd" << std::endl;

	if (_rdr->cam)
	{
		l = *_rdr->cam;
		l[0] -= _rdr->screensize[0] / 2.0f - 32.0f;
		l[1] += _rdr->screensize[1] / 2.0f - 5 * 32.0f;
		s = 0.5;
		str << "Score " << plr->score;
		_rdr->draw_text(str.str(), l, s, c);
		str.str("");
		l[1] += 32.0f;

		str << "Dose " << plr->dose;
		_rdr->draw_text(str.str(), l, s, c);
		str.str("");
		l[1] += 32.0f;

		str << "Tolerance " << plr->tolerance;
		_rdr->draw_text(str.str(), l, s, c);
		str.str("");
		l[1] += 32.0f;

		str << "Power " << plr->firerate;
		_rdr->draw_text(str.str(), l, s, c);
	}
}
