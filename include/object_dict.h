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

#ifndef OBJECT_DATABASE_OBJECT_DICT_H
#define OBJECT_DATABASE_OBJECT_DICT_H

// Standard
#include <map>
#include <memory>
#include <string>
#include <utility>

// Boost
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

// Local
#include <shared_types.h>

namespace object_database {
class ObjectDictInterface
{
public:
  virtual size_t size() const = 0;
};

template <typename Key, typename Value>
class ObjectDict : public ObjectDictInterface
{
  using SharedValue = SharedType<Value>;
  using SharedKey = SharedType<Key>;
  using KeyAllocator = SharedTypeAllocator<Key>;
  using ValueAllocator = SharedTypeAllocator<Value>;

public:
  ObjectDict(Map<Key, Value>* map, std::shared_ptr<KeyAllocator> key_allocator,
             std::shared_ptr<ValueAllocator> value_allocator)
    : map_(map)
    , key_allocator_(key_allocator)
    , value_allocator_(value_allocator)
  {
  }
  SharedValue& Insert(const Key& id, const Value& val = Value())
  {
    boost::interprocess::scoped_lock<
      boost::interprocess::interprocess_upgradable_mutex>
      lock(mutex_);
    return map_
      ->insert(std::pair<const SharedKey, SharedValue>(
        PrivateToShared(id, key_allocator_),
        PrivateToShared(val, value_allocator_)))
      .first->second;
  }

  SharedValue& GetSharedNoLock(const Key& id)
  {
    return map_->at(PrivateToShared(id, key_allocator_));
  }

  void Lock() { mutex_.lock(); }

  void UnLock() { mutex_.unlock(); }

  void ReadLock() { mutex_.lock_sharable(); }

  void ReadUnLock() { mutex_.unlock_sharable(); }

  Value Read(const Key& id) const
  {
    mutex_.lock_sharable();
    return SharedToPrivate<Value>(
      map_->at(PrivateToShared(id, key_allocator_)));
    mutex_.unlock_sharable();
  }

  std::map<Key, Value> ToPrivateMap()
  {
    std::map<Key, Value> map;

    for (const auto& p : *map_) {
      map[SharedToPrivate<Key>(p.first)] = SharedToPrivate<Value>(p.second);
    }
    return map;
  }

  size_t size() const { return map_->size(); }
private:
  Map<Key, Value>* map_;
  std::shared_ptr<KeyAllocator> key_allocator_;
  std::shared_ptr<ValueAllocator> value_allocator_;
  mutable boost::interprocess::interprocess_upgradable_mutex mutex_;
};

} // namespace object_database

#endif // OBJECT_DATABASE_OBJECT_DICT_H
