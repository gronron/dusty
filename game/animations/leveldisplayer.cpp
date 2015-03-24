#include "level.hpp"
#include "leveldisplayer.hpp"
#include <iostream>

Leveldisplayer::Leveldisplayer(float d, bool *c, Level *l) : Animation(d, c), lvl(l)
{

}

Leveldisplayer::~Leveldisplayer()
{

}

bool	Leveldisplayer::tick(float)
{
	return (true);
}

void Leveldisplayer::post_instanciation(Renderer *r)
{
	Animation::post_instanciation(r);
	tex = _rdr->load_texture("particle.png");
}

void									Leveldisplayer::draw()
{
	std::vector<Level::Block>::iterator	i;
	vec<float, 4>						a, b;
	vec<float, 2>						s;
	a = 1.0f;
	b = 0.33f;
	b[3] = 1.0f;
	s = 0.25f;

	std::cout << "ldd" << std::endl;
	for (i = lvl->_map.begin(); i != lvl->_map.end(); ++i)
		_rdr->draw_texture(tex, i->loc, s, i->t ? a : b);
}
