#include <active_record/field.h>

namespace ActiveRecord {

std::ostream& operator<<(std::ostream& cout, const ActiveRecord::Field& field) {
  cout << field.name() << " " << ActiveRecord::type_string(field.type());

  return cout;
}

} // namespace ActiveRecord
