#ifndef _ACTIVE_RECORD_TABLE_H_
#define _ACTIVE_RECORD_TABLE_H_

#include <string>
#include <vector>
#include <active_record/field.h>
#include <active_record/type.h>

using namespace std;

namespace ActiveRecord {
class Table;
}

ostream& operator<<(ostream &cout, const ActiveRecord::Table& table);

namespace ActiveRecord {

class Connection;
class PostgresqlConnection;
class Sqlite3Connection;

class Table {
  friend ostream& ::operator<<(ostream& cout, const Table& e);

  public:

  // Static members
  static string primary_key(
    PostgresqlConnection* connection, const string& table_name
  );
  static string primary_key(
    Sqlite3Connection* connection, const string& table_name
  );

  // Constructors
  Table(Connection* connection = NULL, const string& table_name = "")
    : connection_(connection),
      table_name_(table_name),
      primary_key_("id"),
      timestamps_(false) {}

  Connection* connection() const { return connection_; }
  void primary_key(const string& primary_key) { primary_key_ = primary_key; }
  string primary_key() const { return primary_key_; }
  void timestamps(bool timestamps) { timestamps_ = timestamps; }
  bool timestamps() const { return timestamps_; }
  string table_name() const { return table_name_; }
  void fields(const GenericFieldList &f);
  Fields& fields() { return fields_; }
  void add_field(const Field &field);
  void remove_field(const Field &field);

  private:

  void assert_connection(const char* file, int line);

  private:

  Connection* connection_;
  string      table_name_;
  string      primary_key_;
  bool        timestamps_;
  Fields      fields_;
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TABLE_H_
