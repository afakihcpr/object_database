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

#include <boost/lexical_cast.hpp>
#include <object_database.h>
#include <type_introspection.h>

namespace odb = object_database;

template <typename T>
T
GetCommandLineOption(int argc, char** argv, const std::string& key,
                     const T default_val)
{
  const auto begin = argv;
  const auto end = argv + argc;
  auto** itr = std::find(begin, end, key);
  if (itr != end && ++itr != end) {
    return *itr;
  }
  return default_val;
}

template <>
float
GetCommandLineOption(int argc, char** argv, const std::string& key,
                     const float default_val)
{
  const auto s = GetCommandLineOption<char*>(argc, argv, key, nullptr);
  return s == nullptr ? default_val : std::stof(s);
}

template <>
int
GetCommandLineOption(int argc, char** argv, const std::string& key,
                     const int default_val)
{
  const auto s = GetCommandLineOption<char*>(argc, argv, key, nullptr);
  return s == nullptr ? default_val : std::stoi(s);
}

template <>
bool
GetCommandLineOption(int argc, char** argv, const std::string& key,
                     const bool default_val)
{
  const auto begin = argv;
  const auto end = argv + argc;
  return std::find(begin, end, key) == end ? default_val : !default_val;
}

struct OdbInsert
{
  OdbInsert(std::shared_ptr<odb::ObjectDatabase> db, const std::string& dict_id,
            const std::string& k, const std::string& v)
    : db_(db)
    , dict_id_(dict_id)
    , k_(k)
    , v_(v)
  {
  }

  template <typename T, typename U>
  void Apply()
  {
    db_->Insert(dict_id_, boost::lexical_cast<T>(k_),
                boost::lexical_cast<U>(v_));
    std::cout << "Inserted " << std::endl;
  }

  std::shared_ptr<odb::ObjectDatabase> db_;
  std::string dict_id_;
  std::string k_;
  std::string v_;
};

struct OdbRead
{
  OdbRead(std::shared_ptr<const odb::ObjectDatabase> db,
          const std::string& dict_id, const std::string& k)
    : db_(db)
    , dict_id_(dict_id)
    , k_(k)
  {
  }

  template <typename T, typename U>
  void Apply()
  {
    const auto v = db_->Read<T, U>(dict_id_, boost::lexical_cast<T>(k_));
    v_ = boost::lexical_cast<std::string>(v);
  }

  std::shared_ptr<const odb::ObjectDatabase> db_;
  std::string dict_id_;
  std::string k_;
  std::string v_;
};

int
main(int argc, char* argv[])
{

  const std::string file_name =
    GetCommandLineOption(argc, argv, "-f", "/tmp/test_db");

  const auto insert = GetCommandLineOption(argc, argv, "-i", false);
  const std::string dict = GetCommandLineOption(argc, argv, "-d", "test_dict");
  const auto k_type = GetCommandLineOption(argc, argv, "-k", 1);
  const std::string k_val = GetCommandLineOption(argc, argv, "-K", "1");
  const auto v_type = GetCommandLineOption(argc, argv, "-v", 1);
  const std::string v_val = GetCommandLineOption(argc, argv, "-V", "1");

  auto db = std::make_shared<odb::ObjectDatabase>(file_name);
  if (insert) {
    OdbInsert odb_insert(db, dict, k_val, v_val);
    odb::ApplyOnTypes<odb::max_type_id, odb::max_type_id>(k_type, v_type,
                                                          odb_insert);
  } else {
    OdbRead odb_read(db, dict, k_val);
    odb::ApplyOnTypes<odb::max_type_id, odb::max_type_id>(k_type, v_type,
                                                          odb_read);
    std::cout << "Read value: " << odb_read.v_ << std::endl;
  }
  return 0;
}
