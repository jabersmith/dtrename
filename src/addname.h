/* 

addname.h

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

#ifndef DTRENAME_ADDNAME_H_
#define DTRENAME_ADDNAME_H_


char *add_name_to_list (char *oldList, size_t oldLen,
			char *newName, long pos, int insert,
			size_t *newLen);

#endif  /* DTRENAME_ADDNAME_H_ */
