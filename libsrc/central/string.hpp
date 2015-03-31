
char				*tab_to_space(char *str)
{
	char			*a;
	unsigned int	i = 0;

	while (str[i++]);
	a = new char[i * 4];
	for (i = 0; *str; ++str)
	{
		if (*str == '\t')
		{
			do
				a[i++] = ' ';
			while (i % 4);
		}
		else
			a[i++] = *str;
	}
	return (a);
}

char					to_c_char(char c)
{
	static char const	*c_char = "a\ab\bt\tn\nv\vf\fr\r0\0";

	for (unsigned int i = 0; c_char[i]; i += 2)
	{
		if (c_char[i] == c)
			return (c_char[i + 1]);
	}
	return (c);
}

char				*to_c_string(char *str)
{
	char			*a;
	unsigned int	i = 0;
	bool			quote = false;
	
	if (*str == '"')
	{
		str++;
		quote = true;
	}
	while (str[i++]);
	a = new char[i + 1];
	while (i = 0; *str && (*str != '"' || !quote); ++str)
	{
		if (*str == '\\')
		{
			if (*++str)
				a[i++] = to_c_char(*str);
			else
			{
				delete [] a;
				return (0);
			}
		}
		else
			a[i++] = *str;
	}
	a[i] = '\0';
	return (a);
}

char		*from_c_string(char *str)
{
	char	*a = new char[size * 2 + 1];
	
	return (a);
}
