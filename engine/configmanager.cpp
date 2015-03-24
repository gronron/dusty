#include <cstdlib>
#include "df/df_parser.hpp"
#include "configmanager.hpp"

Configmanager				&Configmanager::get_instance()
{
	static Configmanager	instance;

	return (instance);
}

Configmanager::Configmanager()
{
	assetsdir = get("config.df")->safe_get("assetsdir", Df_node::STRING, 1)->cstr[0];
}

Configmanager::~Configmanager()
{
	for (std::map<std::string, Df_node*>::iterator i = _nodemap.begin(); i != _nodemap.end(); ++i)
		delete i->second;
}

Df_node											*Configmanager::get(std::string const &name)
{
	std::map<std::string, Df_node*>::iterator	i;

	if ((i = _nodemap.find(name)) != _nodemap.end())
		return (i->second);
	else
	{
		Df_node	*node = df_parse(assetsdir + name.c_str());

		if (node)
			_nodemap[name] = node;
		else
			exit(EXIT_FAILURE);
		return (node);
	}
}
