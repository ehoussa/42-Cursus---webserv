#include <stdio.h>
#include "webserv.h"

void	*ft_memset(void *b, int c, size_t len)
{
	size_t			i;
	unsigned char	*s;

	s = (unsigned char *)b;
	i = 0;
	while (i < len)
		s[i++] = (unsigned char)c;
	return (b);
}



char	*ft_strdup(std::string const & s1)
{
	int		i;
	char	*str;

	str = new char[s1.length() + 1];
	if (!str)
		return (NULL);
	i = 0;
	while (s1[i] != '\0')
	{
		str[i] = s1[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}