#include "level.hpp"
#include "leveldisplayer.hpp"

Leveldisplayer::Leveldisplayer(Graphicengine *ge, float d, Level *l) : Animation(ge, d), lvl(l)
{
	tex = _ge->load_texture("particle.png");
}

Leveldisplayer::~Leveldisplayer()
{

}

bool	Leveldisplayer::tick(float)
{
	return (true);
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

	for (i = lvl->_map.begin(); i != lvl->_map.end(); ++i)
		_ge->draw_texture(tex, i->loc, s, i->t ? a : b);
}
