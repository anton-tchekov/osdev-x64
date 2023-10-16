#ifndef __CTYPE_H__
#define __CTYPE_H__

static inline int isprint(int c)
{
	return c >= 32 && c <= 126;
}

static inline int isdigit(int c)
{
	return c >= '0' && c <= '9';
}

static inline int isxdigit(int c)
{
	return isdigit(c) ||
		(c >= 'A' && c <= 'F') ||
		(c >= 'a' && c <= 'f');
}

static inline int isspace(int c)
{
	return c == ' ' || c == '\t' || c == '\n';
}

static inline int isupper(int c)
{
	return c >= 'A' && c <= 'Z';
}

static inline int islower(int c)
{
	return c >= 'a' && c <= 'z';
}

static inline int isalpha(int c)
{
	return islower(c) || isupper(c);
}

static inline int isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}

static inline int toupper(int c)
{
	if(islower(c))
	{
		c = c - 'a' + 'A';
	}

	return c;
}

static inline int tolower(int c)
{
	if(isupper(c))
	{
		c = c - 'A' + 'a';
	}

	return c;
}

#endif /* __CTYPE_H__ */
