#include <active_record/attribute.h>

namespace ActiveRecord {

Attribute Attribute::from_field(sqlite3_stmt* pStmt, int i) {
  const char* type = sqlite3_column_decltype(pStmt, i);
  if(type == nullptr) {
    // http://www.sqlite.org/capi3ref.html#sqlite3_column_decltype
    // This is either an actual NULL value or SQLite doesn't know the
    // declared type of the column.
    auto value = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, i));
    if(value == nullptr) {
      return Attribute();
    }
    return value;
  }

  // TYPE_LIST
  if(strcasecmp(type, "INTEGER") == 0) {
    return static_cast<int64>(sqlite3_column_int(pStmt, i));
  }

  if(strcasecmp(type, "FLOAT") == 0) {
    return sqlite3_column_double(pStmt, i);
  }

  if(strcasecmp(type, "TEXT") == 0) {
    auto value = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, i));
    if(value == nullptr) {
      return Attribute(); // TEXT hasn't been set
    }
    return value;
  }

  if(strcasecmp(type, "DATE") == 0) {
    auto value = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, i));
    if(value == nullptr) {
      return Attribute(); // DATE hasn't been set
    }
    return Date::parse(value);
  }

  std::stringstream error;
  error << "Unhandled data type: " <<type;
  throw ActiveRecordException(error.str(), __FILE__, __LINE__);
}

} // namespace ActiveRecord
