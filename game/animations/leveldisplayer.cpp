#include "graphicengine.hpp"
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

void				Leveldisplayer::draw()
{
	vec<float, 4>	a, b;
	vec<float, 2>	s;
	vec<float, 2>	position;
	a = 1.0f;
	b = 0.33f;
	b[3] = 1.0f;
	s = 0.25f;

	for (unsigned int j = 0; j < lvl->y; ++j)
	{
		for (unsigned int i = 0; i < lvl->x; ++i)
		{
			position[0] = i * 64.0f;
			position[1] = j * 64.0f;
			_ge->draw_texture(tex, position, s, lvl->map[j][i] ? a : b);
		}
	}
}
