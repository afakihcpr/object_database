/**
This file is part of Object Database.
Copyright (C) 2020  Adel H. Fakih adel.fakih@gmail.com

    Object Database is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Object Database.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <type_introspection.h>


namespace object_database {

const int TypeId<int>::id = 1;
const int TypeId<char>::id = 2;
const int TypeId<bool>::id = 3;
const int TypeId<unsigned int>::id = 4;
const int TypeId<double>::id = 5;
const int TypeId<float>::id = 6;
const int TypeId<short>::id = 7;
const int TypeId<unsigned long>::id = 8;
const int TypeId<std::string>::id = 9;

}
