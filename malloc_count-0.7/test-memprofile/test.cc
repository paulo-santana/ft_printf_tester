/******************************************************************************
 * test-memprofile/test.cc
 *
 * Example to write a memory profile.
 *
 ******************************************************************************
 * Copyright (C) 2013 Timo Bingmann <tb@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "memprofile.h"

#include <vector>
#include <set>

int main()
{
    MemProfile mp("memprofile.txt", 0.1, 1024);

    {
        std::vector<int> v;
        for (size_t i = 0; i < 10000000; ++i)
            v.push_back(i);
    }

    {
        std::set<int> v;
        for (size_t i = 0; i < 200000; ++i)
            v.insert(i);
    }

    return 0;
}
