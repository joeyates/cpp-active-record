#include <iostream>
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

void Connection::connect(OptionsHash options) {
  sqlite_initialize(options["database"]);
  //sqlite_create();
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
  const char * sqry = "CREATE TABLE people (id INTEGER PRIMARY KEY, name TEXT, surname TEXT);";
  char * szError;
  int nResult = sqlite3_exec(db_, sqry, 0, 0, &szError);
  if(nResult != SQLITE_OK) {
    fprintf(stderr, "Couldn't run query '%s': %s\n", sqry, szError);
    sqlite3_free(szError);
    return false;
  }
  return true;
}

Row Connection::select_one(string query) {
  sqlite3_stmt *ppStmt = 0;
  int prepare_result = sqlite3_prepare_v2(db_, query.c_str(), query.size(), &ppStmt, 0);
  // TODO: check prepare_result
  int step_result = sqlite3_step(ppStmt);
  if (step_result != SQLITE_ROW)
    throw "No data";
  return Row(ppStmt);
}

RowSet Connection::select_values(string query, AttributeList parameters) {
  sqlite3_stmt *ppStmt = 0;
  int prepare_result = sqlite3_prepare_v2(db_, query.c_str(), query.size(), &ppStmt, 0);
  // TODO: check prepare_result
  for (int i = 0; i < parameters.size(); ++i) {
    switch (parameters[i].first) {
    case integer: {
      int value = boost::get<int>(parameters[i].second);
      sqlite3_bind_int(ppStmt, i + 1, value);
      break;
    }
    case text: {
      string value = boost::get<std::string>(parameters[i].second);
      sqlite3_bind_text(ppStmt, i + 1, value.c_str(), value.size(), 0);
      break;
    }
    default:
      throw "Type not implemented"; // TODO
    }
  }
  RowSet results;
  while (sqlite3_step(ppStmt) == SQLITE_ROW) {
    results.push_back(Row(ppStmt));
  }
  return results;
}

void connect(OptionsHash options) {
  current_connection.connect(options);
}

} // namespace ActiveRecord
