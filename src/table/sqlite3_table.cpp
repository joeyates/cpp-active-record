#include <string>
#include <active_record/connection/sqlite3.h>

using namespace std;

namespace ActiveRecord {

extern TypeNameMap type_name;

string Table::primary_key(
  Sqlite3Connection* connection,
  const string& table_name
) {
  connection->primary_key(table_name);
}

/*
TODO: SQLite does not implement DROP COLUMN - needs some hoop jumping
void Table::remove_field(const Field &field) {
  assert_connection();
  stringstream ss;
  ss << "ALTER TABLE " << table_name_;
  ss << " DROP " << field.name();
  ss << ";";
  connection_->execute(ss.str());
}
*/

} // namespace ActiveRecord
