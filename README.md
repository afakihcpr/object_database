# Object Database

Object Database is a C++14 library providing a [key-value store](https://en.wikipedia.org/wiki/Key%E2%80%93value_database) using a shared memory-mapped file, where the key and value can be objects of various types and can be accessed directly by the clients from Shared Memory. The database allows multiple synchronized read/write from different processes.

# Project Status
  - Work In Progress

# Features
  - 

# Dependencies
   - Boost interprocess, filesystem

# Example Usage
## C++ API
```cpp
odb::ObjectDatabase db("/tmp/example.odb");
// Write
const auto success = db.Insert<std::string, double>("my_dict", "my_key", 49.5);
// Read
const auto my_value = db.Read<std::string, double>("my_dict", "my_key");
// Get a const reference directly to the value. To get a reference the client has to lock the dict for the lifetime of the reference
db.ReadLockDict("my_dict");
const auto& my_ref_value = db.GetSharedNoLock<std::string, double>("my_dict", "my_key");
db.ReadUnLockDict("my_dict");
// Get a non-const reference directly to the value. To get a reference the client has to lock the dict for the lifetime of the reference
db.LockDict("my_dict");
auto& my_ref_value = db.GetSharedNoLock<std::string, double>("my_dict", "my_key");
db.UnLockDict("my_dict");
```
## Command Line Interface
```bash
# insert
odb_cli -i -f filename -d <dict_id> -k <key_type_id> -K <key> -v <value_type_id> -V <value>
# read
odb_cli -f filename -d <dict_id> -k <key_type_id> -K <key> -v <value_type_id>
```
`key_type_id` and `value_type_id` are value id for defined types as follows:

|  Type | id  |
|---|:-:|
|  `int` | 1  |
| `char`  | 2 |
|  `bool` | 3 |
| `unsigned int` | 4 |
| `double` | 5 |
| `float` | 6 |
| `short` | 7 |
| `unsigned long` | 8|
|  `string` | 9  |

# Adding Custom Types

# TODO
- Provide callbacks for when a dict is changed
- 