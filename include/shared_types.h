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

#ifndef OBJECT_DATABASE_SHARED_TYPES_H
#define OBJECT_DATABASE_SHARED_TYPES_H

// Standard
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>

// Boost
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>

namespace object_database {
namespace bi = boost::interprocess;

using CharAllocator =
  bi::allocator<char, bi::managed_mapped_file::segment_manager>;
using String = bi::basic_string<char, std::char_traits<char>, CharAllocator>;

template <typename T>
struct SharedTypeAux
{
  using type = T;
  struct Allocator
  {
    Allocator(bi::managed_mapped_file::segment_manager*) {}
  };
  struct Less
  {
    bool operator()(const T& lhs, const T& rhs) const { return lhs < rhs; }
  };
};
template <>
struct SharedTypeAux<std::string>
{
  using type = String;
  using Allocator = CharAllocator;
  struct Less
  {
    bool operator()(const String& lhs, const String& rhs) const
    {
      return lhs.compare(rhs) > 0 ? true : false;
    }
  };
};

template <typename T>
using SharedType = typename SharedTypeAux<T>::type;

template <typename T>
using SharedTypeLess = typename SharedTypeAux<T>::Less;

template <typename T>
using SharedTypeAllocator = typename SharedTypeAux<T>::Allocator;

template <typename T>
std::shared_ptr<SharedTypeAllocator<T>>
MakeSharedTypeAllocator(
  bi::managed_mapped_file::segment_manager* segment_manager)
{
  return std::make_shared<SharedTypeAllocator<T>>(segment_manager);
}

template <typename T>
SharedType<T> PrivateToShared(
  const T& p, std::shared_ptr<SharedTypeAllocator<T>> allocator);

template <>
String PrivateToShared<std::string>(const std::string& p,
                                    std::shared_ptr<CharAllocator> alloc);

template <typename T>
T
SharedToPrivate(const SharedType<T>& s)
{
  return static_cast<T>(s);
}

template <>
std::string SharedToPrivate<std::string>(const String& s);

template <typename T, typename U>
using PairAllocator =
  bi::allocator<std::pair<const SharedType<T>, SharedType<U>>,
                bi::managed_mapped_file::segment_manager>;

template <typename T, typename U>
using Map =
  bi::map<SharedType<T>, SharedType<U>, SharedTypeLess<T>, PairAllocator<T, U>>;

} // namespace object_database

#endif // OBJECT_DATABASE_SHARED_TYPES_H
