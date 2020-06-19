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

#ifndef OBJECT_DATABASE_TYPE_INTROSPECTION_H
#define OBJECT_DATABASE_TYPE_INTROSPECTION_H
// Standard
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace object_database {

template <typename T>
struct TypeId
{
};

template <int I>
struct Type
{
};

template <>
struct TypeId<int>
{
  static const int id;
};
template <>
struct Type<1>
{
  using type = int;
};

template <>
struct TypeId<char>
{
  static const int id;
};
template <>
struct Type<2>
{
  using type = char;
};

template <>
struct TypeId<bool>
{
  static const int id;
};
template <>
struct Type<3>
{
  using type = bool;
};

template <>
struct TypeId<unsigned int>
{
  static const int id;
};
template <>
struct Type<4>
{
  using type = unsigned int;
};

template <>
struct TypeId<double>
{
  static const int id;
};
template <>
struct Type<5>
{
  using type = double;
};

template <>
struct TypeId<float>
{
  static const int id;
};
template <>
struct Type<6>
{
  using type = float;
};

template <>
struct TypeId<short>
{
  static const int id;
};
template <>
struct Type<7>
{
  using type = short;
};

template <>
struct TypeId<unsigned long>
{
  static const int id;
};
template <>
struct Type<8>
{
  using type = unsigned long;
};

template <>
struct TypeId<std::string>
{
  static const int id;
};
template <>
struct Type<9>
{
  using type = std::string;
};

constexpr int max_type_id = 9;

namespace detail {
template <typename F, int I, int J>
struct ApplyOnTypesAux
{
  static void ApplyOnTypesImpl(int i, int j, F&& f)
  {
    if (i != I) {
      ApplyOnTypesAux<F, I - 1, J>::ApplyOnTypesImpl(i, j, std::forward<F>(f));
    } else if (j != J) {
      ApplyOnTypesAux<F, I, J - 1>::ApplyOnTypesImpl(i, j, std::forward<F>(f));
    } else {

      f.template Apply<typename Type<I>::type, typename Type<J>::type>();
    }
  }
};

template <typename F, int K>
struct ApplyOnTypesAux<F, 0, K>
{
  static void ApplyOnTypesImpl(int, int, F&&) {}
};

template <int K, typename F>
struct ApplyOnTypesAux<F, K, 0>
{
  static void ApplyOnTypesImpl(int, int, F&&) {}
};
} // namespace detail

template <int I, int J, class F>
void
ApplyOnTypes(int i, int j, F&& f)
{
  detail::ApplyOnTypesAux<F, I, J>::ApplyOnTypesImpl(i, j, std::forward<F>(f));
}

} // namespace object_database

#endif // OBJECT_DATABASE_TYPE_INTROSPECTION_H
