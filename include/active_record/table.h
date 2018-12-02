#ifndef _ACTIVE_RECORD_TABLE_H_
#define _ACTIVE_RECORD_TABLE_H_

#include <string>
#include <vector>
#include <active_record/field.h>
#include <active_record/type.h>

namespace ActiveRecord {

class Connection;
class PostgresqlConnection;
class Sqlite3Connection;

class Table {
  public:

  // Static members
  static std::string primary_key(
    PostgresqlConnection* connection, const std::string& table_name
  );
  static std::string primary_key(
    Sqlite3Connection* connection, const std::string& table_name
  );

  // Constructors
  Table(Connection* connection = NULL, const std::string& table_name = ""):
    connection_(connection),
    table_name_(table_name),
    primary_key_("id"),
    timestamps_(false) {}

  Connection* connection() const { return connection_; }
  void primary_key(const std::string& primary_key) {
    primary_key_ = primary_key;
  }
  std::string primary_key() const { return primary_key_; }
  void timestamps(bool timestamps) { timestamps_ = timestamps; }
  bool timestamps() const { return timestamps_; }
  std::string table_name() const { return table_name_; }
  void fields(const GenericFieldList& f);
  Fields& fields() { return fields_; }
  void add_field(const Field& field);
  void remove_field(const Field& field);

  std::string to_string() const {
    std::stringstream s;
    s << table_name_ << ": ";

    for(auto it = fields_.begin(); it != fields_.end(); ++it) {
      if(it != fields_.begin()) {
        s << ", ";
      }

      s << *it;
    }

    return s.str();
  }

  private:

  void assert_connection(const char* file, int line);

  private:

  Connection* connection_;
  std::string table_name_;
  std::string primary_key_;
  bool timestamps_;
  Fields fields_;
};

std::ostream& operator<<(std::ostream &cout, const ActiveRecord::Table& table);

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TABLE_H_
