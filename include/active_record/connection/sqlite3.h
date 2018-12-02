#ifdef AR_SQLITE
#ifndef _ACTIVE_RECORD_SQLITE3_CONNECTION_H_
#define _ACTIVE_RECORD_SQLITE3_CONNECTION_H_

#include <cstdint>
#include <sqlite3.h>

#include <active_record/connection.h>

namespace ActiveRecord {

class Sqlite3Connection: public Connection {
  struct ParameterAllocations {
    int param_count = 0;
    char** param_values = nullptr;
  };

  public:

  Sqlite3Connection();
  virtual ~Sqlite3Connection();

  virtual void connect(OptionsHash options);
  virtual void disconnect();
  virtual bool connected();

  // Database Structure
  virtual bool table_exists(const std::string& table_name);
  // Queries
  virtual bool execute(
    const std::string& query,
    const AttributeList& parameters = AttributeList()
  );
  virtual int64_t insert(
    const std::string& query,
    const AttributeList& parameters = AttributeList()
  );
  virtual Attribute select_value(
    const std::string& query,
    const AttributeList& parameters = AttributeList()
  );
  virtual AttributeList select_values(
    const std::string& query,
    const AttributeList& parameters = AttributeList()
  );
  virtual Row select_one(
    const std::string& query,
    const AttributeList& parameters = AttributeList()
  );
  virtual RowSet select_all(
    const std::string& query,
    const AttributeList& parameters = AttributeList()
  );
  virtual std::string primary_key(const std::string& table_name);
  virtual TableSet schema();
  virtual Table table_data(const std::string& table_name);
  virtual void remove_field(
    const std::string& table_name,
    const std::string& field_name
  );

  private:

  Sqlite3Connection(const Sqlite3Connection& other) {};
  Sqlite3Connection operator=(const Sqlite3Connection& other) {};

  bool sqlite_initialize(std::string& database_path_name);
  static std::string sqlite_error(int error_code);
  sqlite3_stmt* prepare(
    const std::string& query,
    const AttributeList& parameters,
    ParameterAllocations& pa
  );
  void bind_parameters(
    sqlite3_stmt* ppStmt,
    const AttributeList& parameters,
    ParameterAllocations& pa
  );
  void cleanup(ParameterAllocations& pa);

  private:

  sqlite3* db_;
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_SQLITE3_CONNECTION_H_
#endif // def AR_SQLITE
