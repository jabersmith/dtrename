/* 

testaddname.c

Copyright (C) 2015 John Abernathy Smith II

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <addname.h>

int main (int argc, char **argv)
{
  char   *newList;
  size_t  newLen;

  /* {"", 0} is a list of zero names.  Inserting a new name into  */
  /* it should result in a list of one name, whose length should  */
  /* be one greater than the length of the name, since we count   */
  /* the terminating zero-bytes in the length of the list.        */
  newList = add_name_to_list("", 0, "foo", 0, 1, &newLen);
  assert(newLen == 4);
  assert(!memcmp(newList,"foo\0", 4));

  /* Overwriting a member of the zero-length list doesn't make any */
  /* sense, ... */
  

  /* {"", 1} is a list of one name, which is zero bytes long.      */
  /* If we overwrite it, we should get a one-name list with length */
  /* one greater than the length of the name, as above.  If we     */
  /* insert into the list, we should get that plus an additional   */
  /* null-byte, either before or after it.                         */

  newList = add_name_to_list("", 1, "foo", 0, 0, &newLen);
  assert(newLen == 4);
  assert(!memcmp(newList,"foo\0", 4));

  newList = add_name_to_list("", 1, "foo", 0, 1, &newLen);
  assert(newLen == 5);
  assert(!memcmp(newList,"foo\0\0", 5));

  newList = add_name_to_list("", 1, "foo", 1, 1, &newLen);
  assert(newLen == 5);
  assert(!memcmp(newList,"\0foo\0", 5));

  return(EXIT_SUCCESS);
}
