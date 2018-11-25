#include <string>

#include <active_record/connection/sqlite3.h>

namespace ActiveRecord {

string Table::primary_key(
  Sqlite3Connection* connection,
  const std::string& table_name
) {
  return connection->primary_key(table_name);
}

} // namespace ActiveRecord
