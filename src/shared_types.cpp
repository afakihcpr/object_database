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

#include <shared_types.h>

namespace object_database {
template <typename T>
SharedType<T>
PrivateToShared(const T& p, std::shared_ptr<SharedTypeAllocator<T>> allocator)
{
  // incurring a copy here
  return static_cast<SharedType<T>>(p);
}

template <>
String
PrivateToShared<std::string>(
  const std::string& p, std::shared_ptr<SharedTypeAllocator<std::string>> alloc)
{
  String s(*alloc);
  s.assign(p.c_str(), p.size());
  return s;
}

template <>
std::string
SharedToPrivate<std::string>(const String& s)
{
  std::string p;
  p.assign(s.c_str(), s.size());
  return p;
}

// Explicit Instantiation
template int PrivateToShared<int>(const int&,
                                  std::shared_ptr<SharedTypeAllocator<int>>);

template double PrivateToShared<double>(
  const double&, std::shared_ptr<SharedTypeAllocator<double>>);

template float PrivateToShared<float>(
  const float&, std::shared_ptr<SharedTypeAllocator<float>>);

template short PrivateToShared<short>(
  const short&, std::shared_ptr<SharedTypeAllocator<short>>);

template char PrivateToShared<char>(const char&,
                                    std::shared_ptr<SharedTypeAllocator<char>>);

template unsigned long PrivateToShared<unsigned long>(
  const unsigned long&, std::shared_ptr<SharedTypeAllocator<unsigned long>>);

template unsigned int PrivateToShared<unsigned int>(
  const unsigned int&, std::shared_ptr<SharedTypeAllocator<unsigned int>>);

template bool PrivateToShared<bool>(const bool&,
                                    std::shared_ptr<SharedTypeAllocator<bool>>);

template std::pair<int, int> PrivateToShared<std::pair<int, int>>(
  const std::pair<int, int>&,
  std::shared_ptr<SharedTypeAllocator<std::pair<int, int>>>);

} //  namespace object_database
