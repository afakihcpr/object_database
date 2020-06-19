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

#include <object_database.h>

namespace object_database {
ObjectDatabase::ObjectDatabase(const std::string& mapped_file_name,
                               const size_t mapping_size)
  : mapped_file_name_(mapped_file_name)
{
  try {
    boost::filesystem::path p(mapped_file_name);
    const auto parent_path = p.parent_path();
    if (parent_path.empty()) {
      mapped_file_name_ =
        boost::filesystem::current_path().append(mapped_file_name).string();
    } else if (!boost::filesystem::exists(p.parent_path())) {
      boost::filesystem::create_directories(p);
    }
    CreateMapping(mapping_size);
    mapping_valid_ = true;
  } catch (const bi::interprocess_exception& ex) {
    mapping_valid_ = false;
    return;
  }
  LoadDicts();
}

void
ObjectDatabase::CreateMapping(const size_t mapping_size)
{
  file_mapping_ = std::make_unique<bi::managed_mapped_file>(
    bi::open_or_create, mapped_file_name_.c_str(), mapping_size << 20);
}

void
ObjectDatabase::LoadDicts()
{

  Map<std::string, InfoType>* map =
    file_mapping_->find_or_construct<Map<std::string, InfoType>>(
      dicts_info_id_.c_str())(SharedTypeLess<std::string>(),
                              file_mapping_->get_segment_manager());

  dicts_info_ = std::make_unique<ObjectDict<std::string, InfoType>>(
    map,
    MakeSharedTypeAllocator<std::string>(file_mapping_->get_segment_manager()),
    MakeSharedTypeAllocator<InfoType>(file_mapping_->get_segment_manager()));

  for (const auto& p : *map) {
    DictRetriever dr(SharedToPrivate<std::string>(p.first), this);
    ApplyOnTypes<max_type_id, max_type_id>(p.second.first, p.second.second, dr);
  }
}

} //  namespace object_database
