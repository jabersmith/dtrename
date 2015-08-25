/* 

addname.c

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
#include <stdio.h>
#include <string.h>

#include <addname.h>


/*  The EWMH has this to say about the format of _NET_DESKTOP_NAMES:
 *
 *    The names of all virtual desktops. This is a list of NULL-terminated
 *    strings in UTF-8 encoding [UTF8]. This property MAY be changed by a 
 *    Pager or the Window Manager at any time.
 *
 *    Note: The number of names could be different from _NET_NUMBER_OF_DESKTOPS.
 *    If it is less than _NET_NUMBER_OF_DESKTOPS, then the desktops with high
 *    numbers are unnamed. If it is larger than _NET_NUMBER_OF_DESKTOPS, then
 *    the excess names outside of the _NET_NUMBER_OF_DESKTOPS are considered 
 *    to be reserved in case the number of desktops is increased.
 *
 *    Rationale: The name is not a necessary attribute of a virtual desktop. 
 *    Thus the availability or unavailability of names has no impact on 
 *    virtual desktop functionality. Since names are set by users and users 
 *    are likely to preset names for a fixed number of desktops, it doesn't 
 *    make sense to shrink or grow this list when the number of available
 *    desktops changes.
 *
 *  So the list is a single block of memory, with 0-n strings separated by
 *  zero-bytes.  If there are three desktops, named "foo", "bar", and "foobar",
 *  then the property is of length 15, and the bytes are
 *  {'f','o','o','\0','b','a','r','\0','f','o','o','b','a','r','\0'}.
 *
 *  (Since there's no intrinsic way to mark the end of the list -- it's
 *  actually legal to have an embedded zero-length string, unlike the
 *  Windows double-null-terminated string -- you must use the length
 *  of the property, as returned by XGetWindowProperty(), to know where
 *  the list ends.)
 *
 *
 *  This function takes one of these lists, with its length, and a single 
 *  new name, and returns a new version of the list and its length (in the
 *  newLen output parameter).  The pos parameter indicates where in the list
 *  of names, counting from zero, the new name should be written.  If insert
 *  is true, the returned list will be one longer than the original list; the
 *  new name will be inserted in the pos position, and all later names will be
 *  moved down the list. If insert is false, then the returned list will have
 *  the same number of entries as the original; the name at the pos position
 *  in the original list will be replaced by the new name.
 *
 */
char *add_name_to_list (char *oldList, size_t oldLen,
			char *newName, long pos, int insert,
			size_t *newLen)
{
    char *newList;
    char *oldp = NULL;
    char *newp = NULL;
    int id;

    /* allocate a buffer large enough for the original list + the       */
    /* new name.  We'll use it all in the insert case; in the overwrite */
    /* case, we won't use it all, but we won't know exactly how much we */
    /* need until we've finished the scan.                              */
    newList = malloc(oldLen + strlen(newName) + 1);
    if (newList == NULL)
    {
        fprintf(stderr, "Failed to allocate memory.\n");
	return NULL;
    }

    oldp = oldList;
    newp = newList;
    id = 0;

    /* walk through the list, one name at a time, copying into the new list */
    while (((oldp - oldList) < oldLen) ||
	   (insert && (id <= pos)))
    {
        /* copy the appropriate name -- either the newName, if it's time   */
        /* to do so, or the name from the original list that we've reached */
	if (id == pos)
	  strcpy(newp, newName);
	else
	  strcpy(newp, oldp);

	/* advance the pointer into the new list past the name we just */
	/* copied, including its terminating null.                     */
	while (*newp != '\0')
	  newp++;
	newp++;

	/* consume the old name in that position, too, except if we're */
	/* inserting and we're currently at the insert point.          */
	if (!insert || (id != pos))
	{
	      /* consume old name for this desktop */
	      while (*oldp != '\0')
		oldp++;
	      oldp++;
	}
	    
	id++;
    }

    *newLen = newp - newList;
    return newList;
}

