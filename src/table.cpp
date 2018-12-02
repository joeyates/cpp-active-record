#include <active_record/table.h>

#include <sstream>

#include <active_record/active_record.h>
#include <active_record/connection.h>
#include <active_record/exception.h>

namespace ActiveRecord {

void Table::add_field(const Field& field) {
  assert_connection(__FILE__, __LINE__);

  std::stringstream ss;
  ss << "ALTER TABLE " << table_name_;
  ss << " ADD " << field.name() << " " << type_string(field.type());
  ss << ";";

  log(ss.str());

  connection_->execute(ss.str());
}

void Table::remove_field(const Field& field) {
  assert_connection(__FILE__, __LINE__);
  connection_->remove_field(table_name_, field.name());
}

void Table::fields(const GenericFieldList& f) {
  fields_.assign(f.begin(), f.end());
}

////////////////////////////////
// private

void Table::assert_connection(const char* file, int line) {
  if(connection_ != nullptr) {
    return;
  }

  throw ActiveRecordException("No connection", file, line);
}

std::ostream& operator<<(std::ostream& cout, const ActiveRecord::Table& table) {
  cout << table.to_string();

  return cout;
}

} // namespace ActiveRecord
