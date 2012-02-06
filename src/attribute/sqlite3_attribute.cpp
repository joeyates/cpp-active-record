#include <active_record/attribute.h>

namespace ActiveRecord {

Attribute Attribute::from_field(sqlite3_stmt *pStmt, int i) {
  const char * type = sqlite3_column_decltype(pStmt, i);
  if(type == NULL) {
    // http://www.sqlite.org/capi3ref.html#sqlite3_column_decltype
    // This is either an actual NULL value or SQLite doesn't know the
    // declared type of the column.
    const char * value = (const char *) sqlite3_column_text(pStmt, i);
    if(value != 0)
      return value;
    else
      return Attribute();
  } else if(strcasecmp( type, "INTEGER") == 0) {
    return sqlite3_column_int( pStmt, i );
  } else if(strcasecmp( type, "FLOAT") == 0) {
    return sqlite3_column_double( pStmt, i);
  } else if(strcasecmp( type, "TEXT") == 0) {
    const char * value = (const char *) sqlite3_column_text(pStmt, i);
    if(value != 0)
      return value;
    else
      return Attribute(); // TEXT hasn't been set
  } else if(strcasecmp(type, "DATE") == 0) {
    const char * value = (const char *) sqlite3_column_text(pStmt, i);
    if(value != 0)
      return Date::parse(value);
    else
      return Attribute(); // DATE hasn't been set
  } else {
    stringstream error;
    error << "Unhandled data type: " << type;
    throw ActiveRecordException(error.str(), __FILE__, __LINE__);
  }
}

}
