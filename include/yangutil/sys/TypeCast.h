/*
*   Copyright © Ben D.
*   dbapps2@gmail.com
*
*   This is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef TYPE_CAST_H__
#define TYPE_CAST_H__
#include <iostream>
#include <sstream>
using namespace std;

/**
 * @brief Allows types cast
 */
class TypeCast
{
    public:
        /**
         * @brief Converts an int to a string
         * @return the int converted to string
         */
        static string toString(int);

        /**
         * @brief Converts a char to a String
         * @return the converted to string
         */
        static string toString(char);
        static string toString(long);
        static string toString(float);

        static int toInt(string);
};

#endif
