#ifndef _ACTIVE_RECORD_CONNECTION_H_
#define _ACTIVE_RECORD_CONNECTION_H_

#include <active_record/types.h>
#include <active_record/row.h>
#include <active_record/query.h>
#include <cstdio>
#include <sqlite3.h>

namespace ActiveRecord {

class Connection {
 public:
  Connection();
  void    connect(OptionsHash options);
  bool    execute(string query);
  Row     select_value(string query);
  Row     select_one(string query);
  RowSet  select_values(string query, AttributeList parameters = AttributeList());
  RowSet  select_all(string query);
 private:
  Connection(const Connection& other);
  Connection operator=(const Connection& other);
 private:
  bool    sqlite_initialize(string database_path_name);
  bool    sqlite_create();
  sqlite3 * db_;
};

/*
 Connect the default database
 */
void connect(OptionsHash options);

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_CONNECTION_H_
