#include <active_record/row.h>

namespace ActiveRecord {

Row::Row(PGresult* exec_result, int row) {
  int field_count = PQnfields(exec_result);
  for(int column = 0; column <field_count; ++column) {
    std::string name = PQfname(exec_result, column);
    attributes_[name] = Attribute::from_field(exec_result, row, column);
  }
}

} // namespace ActiveRecord
