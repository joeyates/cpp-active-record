#include <active_record/exception.h>

namespace ActiveRecord {

std::ostream& operator<<(
  std::ostream& cout, const ActiveRecord::ActiveRecordException& e
) {
  cout << e.to_string();

  return cout;
}

} // namespace ActiveRecord
