/* 

testaddname.c

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
#include <string.h>
#include <assert.h>

#include <addname.h>

#define ASSERT(oldList, oldLen, newName, pos, insert, expectedList, expectedLen) \
  do { \
    char   *newList; \
    size_t  newLen;  \
    newList = add_name_to_list(oldList, oldLen, newName, pos, insert, &newLen); \
    assert(newLen == expectedLen); \
    assert(!memcmp(newList,expectedList, newLen)); \
  } while (0);

#define INSERT(oldList, oldLen, newName, pos, expectedList, expectedLen) \
        ASSERT(oldList, oldLen, newName, pos, 1, expectedList, expectedLen)

#define OVERWRITE(oldList, oldLen, newName, pos, expectedList, expectedLen) \
        ASSERT(oldList, oldLen, newName, pos, 0, expectedList, expectedLen)


int main (int argc, char **argv)
{
    /* {"", 0} is a list of zero names.  Inserting a new name into  */
    /* it should result in a list of one name, whose length should  */
    /* be one greater than the length of the name, since we count   */
    /* the terminating zero-bytes in the length of the list.        */
    INSERT("", 0, "foo", 0, "foo\0", 4)
    
    /* A zero-length list doesn't have anything to overwrite with   */
    /* the new name, so we return the same zero-length list.        */
    OVERWRITE("", 0, "foo", 0, "", 0)
  

    /* {"", 1} is a list of one name, which is zero bytes long.      */
    /* If we overwrite it, we should get a one-name list with length */
    /* one greater than the length of the name, as above.  If we     */
    /* insert into the list, we should get that plus an additional   */
    /* null-byte, either before or after it.                         */
    OVERWRITE("", 1, "foo", 0, "foo\0", 4)
    INSERT("", 1, "foo", 0, "foo\0\0", 5)
    INSERT("", 1, "foo", 1, "\0foo\0", 5)

    OVERWRITE("bar\0", 4, "foo", 0, "foo\0", 4)
    INSERT("bar\0", 4, "foo", 0, "foo\0bar\0", 8)
    INSERT("bar\0", 4, "foo", 1, "bar\0foo\0", 8)

    OVERWRITE("foo\0bar\0baz\0", 12, "quux", 0, "quux\0bar\0baz\0", 13)
    OVERWRITE("foo\0bar\0baz\0", 12, "quux", 1, "foo\0quux\0baz\0", 13)
    OVERWRITE("foo\0bar\0baz\0", 12, "quux", 2, "foo\0bar\0quux\0", 13)
    OVERWRITE("foo\0bar\0baz\0", 12, "quux", 3, "foo\0bar\0baz\0", 12)

    INSERT("foo\0bar\0baz\0", 12, "quux", 0, "quux\0foo\0bar\0baz\0", 17)
    INSERT("foo\0bar\0baz\0", 12, "quux", 1, "foo\0quux\0bar\0baz\0", 17)
    INSERT("foo\0bar\0baz\0", 12, "quux", 2, "foo\0bar\0quux\0baz\0", 17)
    INSERT("foo\0bar\0baz\0", 12, "quux", 3, "foo\0bar\0baz\0quux\0", 17)
    INSERT("foo\0bar\0baz\0", 12, "quux", 4, "foo\0bar\0baz\0\0quux\0", 18)

    OVERWRITE("\0bar\0baz\0", 9, "quux", 0, "quux\0bar\0baz\0", 13)
    OVERWRITE("\0bar\0baz\0", 9, "quux", 1, "\0quux\0baz\0", 10)
    OVERWRITE("foo\0\0baz\0", 9, "quux", 1, "foo\0quux\0baz\0", 13)
    OVERWRITE("foo\0\0baz\0", 9, "quux", 2, "foo\0\0quux\0", 10)
    OVERWRITE("foo\0bar\0\0", 9, "quux", 1, "foo\0quux\0\0", 10)
    OVERWRITE("foo\0bar\0\0", 9, "quux", 2, "foo\0bar\0quux\0", 13)

    return(EXIT_SUCCESS);
}
