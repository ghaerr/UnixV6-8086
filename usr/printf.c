/*
 *
 * formatted output
 * format string : "%-5d"
 *  d - decimal
 *	o - octal
 *	x - hexadecimal
 *	c - char
 *	s - string
 *	u - unsigned decimal
 */
#include <stdarg.h>

extern int write(int fd, void *buf, int len);

static int ljflg, fw, pr, prflg;
static char xbuf[21], pad;
static char *xp;

#define putchar(c) { *xp++ = c; }

static char *itoa(int x)
{
	int sf = 0;
	char *cp = &xbuf[20];
	*cp-- = 0;
	*cp = '0';
	if (x == 0)
		return (cp);
	if (x < 0)
	{
		sf++;
		x = -x;
	}
	while (x > 0)
	{
		*cp-- = x % 10 + '0';
		x /= 10;
	}
	cp++;
	if (sf)
		*--cp = '-';
	return (cp);
}

static char *utoa(unsigned x, int r)
{
	char *hx = "0123456789abcdef";

	char *cp = &xbuf[20];
	*cp-- = 0;
	*cp = '0';
	if (x == 0)
		return (cp);
	while (x > 0)
	{
		*cp-- = hx[x % r];
		x /= r;
	}
	return (cp + 1);
}

static int strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

static void putsr(char *cp)
{
	int sl = strlen(cp);
	int i;
	if (pr < sl && prflg)
		sl = pr;
	i = fw - sl;
	if (sl < fw && !ljflg)
	{
		while (i--)
			putchar(pad);
	}
	while (*cp)
	{
		putchar(*cp++);
		if (--pr <= 0 && prflg)
			break;
	}
	if (sl < fw && ljflg)
	{
		while (i--)
			putchar(pad);
	}
}

int vsprintf(char *buffer, const char *format, va_list arglist)
{
	const char *per, *cp;
	const char *fr = format;

	xp = buffer;

	while (*fr)
	{
		if (*fr != '%')
		{
			putchar(*fr++);
			continue;
		}
		
		ljflg = fw = pr = prflg = 0; pad = ' ';
		per = ++fr;

		if (*fr == '-')
		{
			ljflg++;
			fr++;
		}
		if (*fr == '0')
		{
			pad = '0';
			fr++;
		}
		while ('0' <= *fr && *fr <= '9')
		{
			fw *= 10;
			fw += *fr++ - '0';
		}
		if (*fr == '.')
		{
			fr++;
			prflg++;
			while ('0' <= *fr && *fr <= '9')
			{
				pr *= 10;
				pr += *fr++ - '0';
			}
		}
		switch (*fr)
		{
		case 'd':
			putsr(itoa(va_arg(arglist, int)));
			break;

		case 'o':
			putsr(utoa(va_arg(arglist, unsigned), 8));
			break;
		case 'x':
			putsr(utoa(va_arg(arglist, unsigned), 16));
			break;
		case 'c':
			xbuf[0] = (char)(va_arg(arglist, int));
			xbuf[1] = 0;
			putsr(xbuf);
			break;

		case 's':
			putsr(va_arg(arglist, char *));
			break;

		case 'u':
			putsr(utoa(va_arg(arglist, unsigned), 10));
			break;

		default:
			fr++;
			while (per < fr)
				putchar(*per++);
			fr--;
		}
		fr++;
	}
	putchar(0);
	return xp - buffer - 1;
}

int sprintf(char *buffer, const char *format, ...)
{
	int cnt; /* Result of SPRINTF for return */
	va_list ap;

	va_start(ap, format);
	cnt = vsprintf(buffer, format, ap); /* prints string to buffer	*/
	va_end(ap);							/* Close va_ functions		*/

	return (cnt); /* Return the conversion count	*/
}

int fprintf(int fd, const char *format, ...)
{
	char str[200];
	int cnt;
	va_list ap;

	va_start(ap, format);
	cnt = vsprintf(str, format, ap);
	va_end(ap);

	write(fd, str, cnt);
	return (cnt);
}

int printf(const char *format, ...)
{
	char str[200];
	int cnt;
	va_list ap;

	va_start(ap, format);
	cnt = vsprintf(str, format, ap);
	va_end(ap);

	write(1, str, cnt);
	return (cnt);
}
