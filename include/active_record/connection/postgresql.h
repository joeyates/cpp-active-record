#ifdef AR_POSTGRES

#ifndef _ACTIVE_RECORD_CONNECTION_POSTGRESQL_H_
#define _ACTIVE_RECORD_CONNECTION_POSTGRESQL_H_

#include <postgresql/libpq-fe.h>
#include <active_record/connection.h>

namespace ActiveRecord {

class PostgresqlConnection: public Connection {
  struct ParameterAllocations {
    int param_count = 0;
    char** param_values = nullptr;
    Oid* param_types = nullptr;
    int* param_lengths = nullptr;
    int* param_formats = nullptr;
  };

  public:

  PostgresqlConnection();
  virtual ~PostgresqlConnection();

  private:

  PostgresqlConnection(const PostgresqlConnection& other) {};
  PostgresqlConnection operator=(const PostgresqlConnection& other) {};

  public:

  /*
   * connection - an existing database connect to be used to issue
   *   the CREATE DATABASE command
   * options:
   *   database - the name of the database to be created
   *   owner - (optional)
   *   template - (optional)
   */
  static bool create_database(
    PostgresqlConnection& connection,
    OptionsHash options
  );
  static void drop_database(
    PostgresqlConnection& connection,
    const std::string& database_name
  );
  static bool database_exists(
    PostgresqlConnection& connection,
    const std::string &database_name
  );

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
  virtual long insert(
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

  PGresult* prepare(
    const std::string &query,
    const AttributeList& parameters,
    ParameterAllocations& pa
  );
  void bind_parameters(
    const AttributeList& parameters,
    ParameterAllocations& pa
  );
  void cleanup(ParameterAllocations& pa);
  bool is_error(PGresult* exec_result);
  void log_error(PGresult* exec_result);

  private:

  PGconn* pgconn_;
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_CONNECTION_POSTGRESQL_H_

#endif // def AR_POSTGRES
