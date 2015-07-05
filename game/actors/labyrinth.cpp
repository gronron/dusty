#include <cstring>
#include <stack>
#include "new.hpp"
#include "math/vec.hpp"
#include "random/mt19937.hpp"

struct		Block
{
	bool	visited;
	bool	bot;
	bool	right;
};

static void		open_wall(Block **b, vec<int, 2> const &x, vec<int, 2> const &y)
{
	vec<int, 2>	a;

	a = x - y;
	if (a[0] < 0)
		b[x[1]][x[0]].right = true;
	else if (a[0] > 0)
		b[y[1]][y[0]].right = true;
	else if (a[1] < 0)
		b[x[1]][x[0]].bot = true;
	else
		b[y[1]][y[0]].bot = true;
}

static char			**create_map(Block **blck, unsigned int x, unsigned int y)
{
	char			**a;
	unsigned int	i;
	unsigned int	j;
	unsigned int	k;
	unsigned int	l;

	a = new_matrix<char>(x * 4 + 1, y * 4 + 1);
	memset(*a, 1, (x * 4 + 1) * (y * 4 + 1));
	for (l = 0; l < y; ++l)
	{
		for (k = 0; k < x; ++k)
		{
			for (j = 1; j < 4; ++j)
				for (i = 1; i < 4; ++i)
					a[l * 4 + j][k * 4 + i] = 0;
			if (blck[l][k].right)
			{
				for (j = 1; j < 4; ++j)
					a[l * 4 + j][k * 4 + 4] = 0;
			}
			if (blck[l][k].bot)
			{
				for (j = 1; j < 4; ++j)
					a[l * 4 + 4][k * 4 + j] = 0;
			}
		}
	}
	return (a);
}

char				**generate_labyrinth(unsigned int x, unsigned int y)
{
	char			**a;
	Block			**blck;
	vec<int, 2>		b;
	vec<int, 2>		c;
	vec<int, 2>		p[4];
	unsigned int	i;
	unsigned int	j;
	unsigned int	k;
	std::stack<vec<int, 2> >	bstack;
	std::stack<vec<int, 2> >	bhstack;

	blck = new_matrix<Block>(x, y);
	memset(*blck, 0, sizeof(Block) * x * y);
	c = 0;
	while (!blck[c[1]][c[0]].visited)
	{
		blck[c[1]][c[0]].visited = true;
		i = 0;
		if (c[0] - 1 > 0 && !blck[c[1]][c[0] - 1].visited)
		{
			p[i] = c;
			p[i++][0] -= 1;
		}
		if (c[0] + 1 < (int)x && !blck[c[1]][c[0] + 1].visited)
		{
			p[i] = c;
			p[i++][0] += 1;
		}
		if (c[1] - 1 > 0 && !blck[c[1] - 1][c[0]].visited)
		{
			p[i] = c;
			p[i++][1] -= 1;
		}
		if (c[1] + 1 < (int)y && !blck[c[1] + 1][c[0]].visited)
		{
			p[i] = c;
			p[i++][1] += 1;
		}
		if (i)
		{
			b = c;
			k = MT().genrand_int31() % i;
			for (j = 0; j < i; ++j)
			{
				if (j == k)
				{
					open_wall(blck, c, p[j]);
					c = p[j];
				}
				else
				{
					bstack.push(p[j]);
					bhstack.push(b);
				}
			}
		}
		else
		{
			while(!bstack.empty() && blck[bstack.top()[1]][bstack.top()[0]].visited)
			{
				bstack.pop();
				bhstack.pop();
			}
			if (!bstack.empty())
			{
				open_wall(blck, bhstack.top(), bstack.top());
				c = bstack.top();
				bstack.pop();
				bhstack.pop();
			}
		}
	}
	a = create_map(blck, x, y);
	delete_matrix(blck);
	return (a);
}
