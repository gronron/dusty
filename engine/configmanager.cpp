/******************************************************************************
Copyright (c) 2015, Geoffrey TOURON
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of dusty nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

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
		Df_node	*node = df_parse(configdir + name.c_str());

		if (node)
			_nodemap[name] = node;
		else
			exit(EXIT_FAILURE);
		return (node);
	}
}
