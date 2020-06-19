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

#ifndef OBJECT_DATABASE_OBJECT_DATABASE_H
#define OBJECT_DATABASE_OBJECT_DATABASE_H

// Standard
#include <chrono>
#include <iostream>
#include <map>
#include <string>

// Boost
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

// Local
#include <object_dict.h>
#include <type_introspection.h>

namespace object_database {

class DictRetriever;

class ObjectDatabase
{

public:
  explicit ObjectDatabase(const std::string& mapped_file_name,
                          const size_t mapping_size = 20 /* MB */);

  template <class T, class U>
  bool Insert(const std::string& dict_id, const T& key, const U& val)
  {
    if (!mapping_valid_) {
      return false;
    }

    auto& obj = GetDict<T, U>(dict_id)->Insert(key, val);
    /** @todo check if obj to private is the same as val, if yes reset obj to
     * val
     */
    return true;
  }

  template <typename T, typename U>
  U Read(const std::string& dict_id, const T& key) const
  {
    return GetDict<T, U>(dict_id)->Read(key);
  }

  template <typename T, typename U>
  SharedType<U>& GetSharedNoLock(const std::string& dict_id, const T& key)
  {
    return GetDict<T, U>(dict_id)->GetSharedNoLock(key);
  }

  template <typename T, typename U>
  void LockDict(const std::string& dict_id)
  {
    return GetDict<T, U>(dict_id)->Lock();
  }

  template <typename T, typename U>
  void UnLockDict(const std::string& dict_id)
  {
    return GetDict<T, U>(dict_id)->UnLock();
  }

  template <typename T, typename U>
  void ReadLockDict(const std::string& dict_id)
  {
    return GetDict<T, U>(dict_id)->ReadLock();
  }

  template <typename T, typename U>
  void ReadUnLockDict(const std::string& dict_id)
  {
    return GetDict<T, U>(dict_id)->ReadUnLock();
  }

private:
  void CreateMapping(const size_t mapping_size = 20 /* MB */);

  template <typename T, typename U>
  std::shared_ptr<ObjectDict<T, U>> GetDict(const std::string& dict_id)
  {
    auto it = object_dicts_.find(dict_id);
    if (it != object_dicts_.end()) {
      return std::dynamic_pointer_cast<ObjectDict<T, U>>(it->second);
    } else {
      return CreateDict<T, U>(dict_id);
    }
  }

  template <typename T, typename U>
  std::shared_ptr<ObjectDict<T, U>> CreateDict(const std::string& dict_id)
  {
    Map<T, U>* m_map =
      file_mapping_->find_or_construct<Map<T, U>>(dict_id.c_str())(
        SharedTypeLess<T>(), file_mapping_->get_segment_manager());
    auto emplaced = object_dicts_.emplace(
      dict_id,
      std::dynamic_pointer_cast<ObjectDictInterface>(
        std::make_shared<ObjectDict<T, U>>(
          m_map,
          MakeSharedTypeAllocator<T>(file_mapping_->get_segment_manager()),
          MakeSharedTypeAllocator<U>(file_mapping_->get_segment_manager()))));

    dicts_info_->Insert(dict_id, std::make_pair(TypeId<T>::id, TypeId<U>::id));
    return std::dynamic_pointer_cast<ObjectDict<T, U>>(emplaced.first->second);
  }

  void LoadDicts();

  template <typename T, typename U>
  std::shared_ptr<const ObjectDict<T, U>> GetDict(
    const std::string& dict_id) const
  {
    auto it = object_dicts_.find(dict_id);
    if (it != object_dicts_.end()) {
      return std::dynamic_pointer_cast<const ObjectDict<T, U>>(it->second);
    } else {
      throw std::runtime_error(std::string("Selected Dict ID [") + dict_id +
                               "] does not exist");
    }
  }

  using Dicts = std::map<std::string, std::shared_ptr<ObjectDictInterface>>;
  using InfoType = std::pair<int, int>;
  std::string mapped_file_name_;
  std::unique_ptr<bi::managed_mapped_file> file_mapping_;
  Dicts object_dicts_;
  bool mapping_valid_ = false;
  const std::string dicts_info_id_ = "odb_dicts_info";
  std::unique_ptr<ObjectDict<std::string, InfoType>> dicts_info_;
  friend class DictRetriever;
};

class DictRetriever
{
public:
  DictRetriever(const std::string& dict_id, ObjectDatabase* odb)
    : dict_id_(dict_id)
    , odb_(odb)
  {
  }

  template <typename T, typename U>
  void Apply()
  {
    Map<T, U>* m_map =
      odb_->file_mapping_->find<Map<T, U>>(dict_id_.c_str()).first;
    if (not m_map) {
      throw std::runtime_error(std::string("Selected Dict ID [") + dict_id_ +
                               "] does not exist");
    }
    odb_->object_dicts_.emplace(
      dict_id_, std::dynamic_pointer_cast<ObjectDictInterface>(
                  std::make_shared<ObjectDict<T, U>>(
                    m_map, MakeSharedTypeAllocator<T>(
                             odb_->file_mapping_->get_segment_manager()),
                    MakeSharedTypeAllocator<U>(
                      odb_->file_mapping_->get_segment_manager()))));
  }

private:
  std::string dict_id_;
  ObjectDatabase* odb_;
};

} // namespace object_databse
#endif // OBJECT_DATABASE_OBJECT_DATABASE_H
