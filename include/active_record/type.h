#ifndef _ACTIVE_RECORD_TYPE_H_
#define _ACTIVE_RECORD_TYPE_H_

#include <iostream> // For debugging
#include <map>
#include <string>

using namespace std;

namespace ActiveRecord {

namespace Type {
// Attributes are typed
// TYPE_LIST
enum Type {
  unknown        = -1,
  integer        = 0,
  text           = 1,
  floating_point = 2,
  date           = 3
};

} // namespace Type

typedef pair<ActiveRecord::Type::Type, string> TypeNamePair;
typedef map<ActiveRecord::Type::Type, string> TypeNameMap;

Type::Type index_to_type(int index);
Type::Type to_type(const string& type_name);
const char* type_string(const Type::Type& type);

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TYPE_H_
