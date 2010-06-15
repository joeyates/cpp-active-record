#ifndef ACTIVE_RECORD_CONNECTION_H
#define ACTIVE_RECORD_CONNECTION_H

#include <active_record/table_data.h>
#include <active_record/query.h>
#include <cstdio>
#include <sqlite3.h>

namespace ActiveRecord {

class Row {
};

class RowSet {
};

class Connection {
 public:
  Connection();
  Connection(const Connection& other);
  Connection operator=(const Connection& other);
  void    connect(Options options);
  bool    execute(string query);
  Row     select_value(string query);
  RowSet  select_values(string query);
  RowSet  select_all(string query);
 private:
  bool    sqlite_initialize(string database_path_name);
  bool    sqlite_create();
  sqlite3 * db_;
};

} // namespace ActiveRecord

#endif // ndef ACTIVE_RECORD_CONNECTION_H
