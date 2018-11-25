#include <active_record/field.h>

ostream& operator<<(ostream& cout, const ActiveRecord::Field& field) {
  cout << field.name_ << " " << ActiveRecord::type_string(field.type_);
  return cout;
}
