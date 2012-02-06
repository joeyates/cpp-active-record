#ifndef _ACTIVE_RECORD_TYPE_H_
#define _ACTIVE_RECORD_TYPE_H_

#include <iostream> // For debugging
#include <map>
#include <string>

using namespace std;

namespace ActiveRecord {

namespace Type {
// Attributes are typed
enum Type {
  unknown        = -1,
  integer        = 0,
  long_long      = 1,
  text           = 2,
  floating_point = 3,
  date           = 4
};

} // namespace Type

typedef pair< ActiveRecord::Type::Type, string >    TypeNamePair;
typedef map< ActiveRecord::Type::Type, string >     TypeNameMap;

Type::Type index_to_type( int index );
Type::Type to_type( const string &type_name );

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TYPE_H_
