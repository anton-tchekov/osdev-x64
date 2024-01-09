#ifndef __CTYPE_H__
#define __CTYPE_H__

static int isprint(int c)
{
	return c >= 32 && c <= 126;
}

static int isdigit(int c)
{
	return c >= '0' && c <= '9';
}

static int isxdigit(int c)
{
	return isdigit(c) ||
		(c >= 'A' && c <= 'F') ||
		(c >= 'a' && c <= 'f');
}

static int isspace(int c)
{
	return c == ' ' || c == '\t' || c == '\n';
}

static int isupper(int c)
{
	return c >= 'A' && c <= 'Z';
}

static int islower(int c)
{
	return c >= 'a' && c <= 'z';
}

static int isalpha(int c)
{
	return islower(c) || isupper(c);
}

static int isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}

static int toupper(int c)
{
	if(islower(c))
	{
		c = c - 'a' + 'A';
	}

	return c;
}

static int tolower(int c)
{
	if(isupper(c))
	{
		c = c - 'A' + 'a';
	}

	return c;
}

#endif
