#ifndef CONFIGMANAGER_H_
#define CONFIGMANAGER_H_

#include <string>
#include <map>
#include "df/df_node.hpp"

class	Configmanager
{
	public:

		static Configmanager	&get_instance();

		std::string						assetsdir;
		std::map<std::string, Df_node*>	_nodemap;


		Configmanager();
		~Configmanager();

		Df_node	*get(std::string const &);
};

#endif
