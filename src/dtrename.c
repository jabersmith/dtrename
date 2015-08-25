/* 

dtrename

A command line tool to change the names of virtual desktops in
an EWMH/NetWM compatible X Window Manager.

Copyright (C) 2015 John Abernathy Smith II

This file is part of dtrename.

dtrename is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

dtrename is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with dtrename.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <config.h>
#include <addname.h>

static void usage(int status);

static void consume_arguments(int argc, char **argv,
			      long *pos, int *insert,
			      char **newName);

static unsigned char *get_property(Display *disp, Window win,
				   Atom xa_prop_type, char *name,
				   int *ret_format, unsigned long *ret_nitems);

static char *get_string_property (Display *disp, Window win, 
				  char *name, unsigned long *size);

static long get_long_property (Display *disp, Window win,
			       char *name);

static void set_string_property (Display *disp, Window win,
				 char *name,
				 char *value, size_t size);


#define HELP PACKAGE_STRING "\n" \
"Usage: dtrename [-d POS] [-i] NAME\n" \
"Set the name of the current virtual desktop\n" \
"  -d          set the name of the POS'th desktop, counting\n" \
"              from 0, instead of the current desktop\n" \
"  -i          insert a new desktop name into the current list,\n" \
"              rather than overwriting the current desktop's name\n" \
"\n" \
"Report bugs to: " PACKAGE_BUGREPORT "\n" \
"\n"


static void usage(int status)
{
    fputs(HELP, stdout);
    exit(status);
}


static void consume_arguments(int argc, char **argv,
			      long *pos, int *insert,
			      char **newName)
{
    int c;
  
    if ((argc < 2) || (argv[1] == NULL))
      usage(EXIT_FAILURE);

    /* handle a couple of long options -- don't use */
    /* GNU getopt_long() for portability's sake     */
    if (strcmp(argv[1], "--version") == 0)  
    {
      fputs(VERSION, stdout);
      exit(EXIT_SUCCESS);
    }
    else if (strcmp(argv[1], "--help") == 0)
      usage(EXIT_SUCCESS);

    /* consume the rest of the arguments with getopt */
    while ((c = getopt(argc, argv, "d:i")) != -1)
      switch (c)
      {
      case 'd':
	errno = 0;
	*pos = strtol(optarg, NULL, 10);
	if (errno != 0)
	  usage(EXIT_FAILURE);
	break;
      case 'i':
	*insert = 1;
	break;
      case '?':
      default:
	usage(EXIT_FAILURE);
      }

    if (optind >= argc)
      usage(EXIT_FAILURE);

    *newName = argv[optind];
}



static unsigned char *get_property(Display *disp, Window win,
				   Atom xa_prop_type, char *name,
				   int *ret_format, unsigned long *ret_nitems)
{
    Atom xa_prop_name;
    Atom xa_ret_type;
    unsigned long ret_bytes_after;
    unsigned char *ret_prop;
    
    xa_prop_name = XInternAtom(disp, name, False);
    
    if (XGetWindowProperty(disp, win, xa_prop_name, 0,
			   4096, False,
			   xa_prop_type, &xa_ret_type, ret_format,     
			   ret_nitems, &ret_bytes_after, &ret_prop) != Success)
    {
        fprintf(stderr,"Can't get %s property.\n", name);
        return NULL;
    }
  
    if (xa_ret_type != xa_prop_type)
    {
        fprintf(stderr,"Invalid type of %s property.\n", name);
        XFree(ret_prop);
        return NULL;
    }

    return ret_prop;
}


static long get_long_property (Display *disp, Window win,
			       char *name)
{
    unsigned char *ret_prop;
    int ret_format;
    unsigned long ret_nitems;
    long value;

    ret_prop = get_property(disp, win, XA_CARDINAL, name,
			    &ret_format, &ret_nitems);
    if (ret_prop == NULL)
      return -1;

    value = *((long *) ret_prop);
    XFree (ret_prop);
    return value;
}


static char *get_string_property (Display *disp, Window win,
				  char *name, size_t *size)
{
    Atom xa_prop_type;
    unsigned char *ret_prop;
    int ret_format;
    unsigned long ret_nitems;
    size_t tmp_size;
    char *ret;
 
    xa_prop_type = XInternAtom(disp, "UTF8_STRING", False);

    ret_prop = get_property(disp, win, xa_prop_type, name,
			    &ret_format, &ret_nitems);
    if (ret_prop == NULL)
      return NULL;

    /* return a null-terminated copy, to make string handling easier */
    tmp_size = (ret_format / 8) * ret_nitems;
    ret = malloc(tmp_size + 1);
    if (ret == NULL)
    {
        fprintf(stderr, "Can't allocate memory.\n");
	return NULL;
    }
    memcpy(ret, ret_prop, tmp_size);
    ret[tmp_size] = '\0';

    if (size)
    {
        *size = tmp_size;
    }
    
    XFree(ret_prop);
    return ret;
}


static void set_string_property (Display *disp, Window win,
				 char *name,
				 char *value, size_t size)
{
    XChangeProperty(disp, win, 
		    XInternAtom(disp, name, False), 
		    XInternAtom(disp, "UTF8_STRING", False), 
		    8, PropModeReplace,
		    (unsigned char *) value, size);
}



int main (int argc, char **argv)
{
    int ret = EXIT_SUCCESS;
    Display *disp;
    Window root;
    char *newName;
    long pos = -1;
    int  insert = 0;
    char *oldList;
    size_t oldLen;
    char *newList;
    size_t newLen;

    consume_arguments(argc, argv, &pos, &insert, &newName);

    disp = XOpenDisplay(NULL);
    if (disp == NULL)
    {
        fputs("Can't open display.\n", stderr);
        exit(EXIT_FAILURE);
    }

    root = DefaultRootWindow(disp);

    if (pos < 0)
    {
        pos = get_long_property(disp, root, "_NET_CURRENT_DESKTOP");
	if (pos < 0)
	{
	  fprintf(stderr, "Can't get _NET_CURRENT_DESKTOP.\n");
	  exit(EXIT_FAILURE);
	}
    }

    oldList = get_string_property(disp, root, "_NET_DESKTOP_NAMES",
				  &oldLen);
    if (oldList == NULL)
    {
        fprintf(stderr, "Can't get _NET_DESKTOP_NAMES.\n");
        exit(EXIT_FAILURE);
    }

    newList = add_name_to_list(oldList, oldLen, newName, pos, insert,
			       &newLen);
    if (newList == NULL)
    {
        ret = EXIT_FAILURE;
	goto F1;
    }

    set_string_property(disp, root, "_NET_DESKTOP_NAMES",
			newList, newLen);
    
    XCloseDisplay(disp);

    free(newList);
F1: free(oldList);
    exit(ret);
}


