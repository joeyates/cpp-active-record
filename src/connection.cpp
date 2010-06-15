#include <active_record/connection.h>

namespace ActiveRecord {

// Mm, nice, globals!
// TODO: thread safety
Connection current_connection;
map<string, TableData> tables;

Connection::Connection() {}

Connection::Connection(const Connection& other) {
}

Connection Connection::operator=(const Connection& other) {
  return *this;
}

void Connection::connect(Options options) {
  sqlite_initialize(options["database"]);
  sqlite_create();
  TableData td;
  td.connection = this;
  td.primary_key = "id";
  td.table_name = "people";
  tables[td.table_name] = td;
}

bool Connection::sqlite_initialize(string database_path_name) {
  int nResult = sqlite3_open(database_path_name.c_str(), &db_);
  if(nResult) {
    fprintf(stderr, "Can't open database '%s': %s\n", database_path_name.c_str(), sqlite3_errmsg(db_));
    sqlite3_close(db_);
    return false;
  }
}

bool Connection::sqlite_create() {
  const char * sqry = "CREATE TABLE foo (bar INTEGER);";
  char * szError;
  int nResult = sqlite3_exec(db_, sqry, 0, 0, &szError);
  if(nResult != SQLITE_OK) {
    fprintf(stderr, "Couldn't run query '%s': %s\n", sqry, szError);
    sqlite3_free(szError);
    return false;
  }
  return true;
}

  /*
http://www.sqlite.org/c3ref/column_blob.html
int sqlite3_prepare_v2(
  sqlite3 *db,           
  const char *zSql,      
  int nByte,             
  sqlite3_stmt **ppStmt, 
  const char **pzTail    
);
*/

} // namespace ActiveRecord
