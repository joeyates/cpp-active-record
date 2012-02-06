#include <active_record/field.h>

namespace ActiveRecord {
extern TypeNameMap type_name;
} // namespace ActiveRecord

ostream & operator<<( ostream &cout, const ActiveRecord::Field& field ) {
  cout << field.name_ << " " << ActiveRecord::type_name[ field.type_ ];
  return cout;
}
