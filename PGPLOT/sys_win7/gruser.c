/* Provide a working getlogin_r for systems which lack it.

   Copyright (C) 2005-2007, 2009-2010 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Paul Eggert, Derek Price, and Bruno Haible.  */

//#include <config.h>

/* Specification.  */
#include <unistd.h>

#include <errno.h>
#include <string.h>

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#else
# if !HAVE_DECL_GETLOGIN
extern char *getlogin (void);
# endif
#endif

/* See unistd.in.h for documentation.  */
int
getlogin_r (char *name, size_t size)
{
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
  /* Native Windows platform.  */
  DWORD sz;

  /* When size > 0x7fff, the doc says that GetUserName will fail.
     Actually, on Windows XP SP3, it succeeds.  But let's be safe,
     for the sake of older Windows versions.  */
  if (size > 0x7fff)
    size = 0x7fff;
  sz = size;
  if (!GetUserName (name, &sz))
    {
      if (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
        /* In this case, the doc says that sz contains the required size, but
           actually, on Windows XP SP3, it contains 2 * the required size.  */
        return ERANGE;
      else
        return ENOENT;
    }
  return 0;
#else
  /* Platform with a getlogin() function.  */
  char *n;
  size_t nlen;

  errno = 0;
  n = getlogin ();
  if (!n)
    /* ENOENT is a reasonable errno value if getlogin returns NULL.  */
    return (errno != 0 ? errno : ENOENT);

  nlen = strlen (n);
  if (size <= nlen)
    return ERANGE;
  memcpy (name, n, nlen + 1);
  return 0;
#endif
}



/*
 **GRUSER -- get user name (POSIX)
 *+
 *     SUBROUTINE GRUSER(STRING, L)
 *     CHARACTER*(*) STRING
 *     INTEGER L
 *
 * Return the name of the user running the program.
 *
 * Arguments:
 *  STRING : receives user name, truncated or extended with
 *           blanks as necessary.
 *  L      : receives the number of characters in VALUE, excluding
 *           trailing blanks.
 *--
 * 08-Nov-1994
 *-----------------------------------------------------------------------
 */

#ifdef PG_PPU
#define GRUSER gruser_
#else
#define GRUSER gruser
#endif

char *getlogin();

void GRUSER(string, length, maxlen)
     char *string;
     int *length;
     int maxlen;
{
  int i;
/*
 * Get the login name of the PGPLOT user.
 */
  //char *user ;//= getlogin();
  int getlogin_r() ;

  char buffer[1024];
  int ret = getlogin_r(buffer, sizeof(buffer));

/*
 * If the user name is not available substitute an empty string.
 */
  if(!ret) strcpy(buffer,"");
/*
 * Copy the user name to the output string.
 */
  for(i=0; i<maxlen && buffer[i]; i++)
    string[i] = buffer[i];
/*
 * Return the un-padded length of the user name string.
 */
  *length = i;
/*
 * Pad to the end of the output string with spaces.
 */
  for( ; i<maxlen; i++)
    string[i] = ' ';
  return;
}
