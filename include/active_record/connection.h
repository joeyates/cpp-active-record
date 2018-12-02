#ifndef _ACTIVE_RECORD_CONNECTION_H_
#define _ACTIVE_RECORD_CONNECTION_H_

#include <cstdint>
#include <cstdio>

#include <active_record/active_record.h>
#include <active_record/options.h>
#include <active_record/type.h>
#include <active_record/table.h>
#include <active_record/table_set.h>
#include <active_record/row.h>

namespace ActiveRecord {

class Table;

class Connection {
  public:

  /* options:
   *   database - database name (or file name for SQLite3)
   *   username - (optional)
   *   port     - (optional)
   *   host     - (optional)
   */
  virtual void connect(OptionsHash options) = 0;
  virtual void disconnect()                   = 0;
  virtual bool connected()                    = 0;

  // Tables/Models
  void set_table(const std::string& class_name, const Table& table);
  Table& get_table(const std::string& class_name);
  void update_database();

  // Database Structure
  virtual bool table_exists(const std::string& table_name)       = 0;

  // Transactions
  void begin_transaction();
  void commit();

  // Queries
  virtual bool execute(
    const std::string& query,
    const AttributeList &parameters = AttributeList()
  ) = 0;
  virtual int64_t insert(const std::string& query,
  const AttributeList &parameters = AttributeList())             = 0;
  virtual Attribute select_value(const std::string& query,
  const AttributeList &parameters = AttributeList())             = 0;
  virtual AttributeList select_values(
    const std::string &query,
    const AttributeList &parameters = AttributeList()
  ) = 0;
  virtual Row select_one(
    const std::string& query,
    const AttributeList &parameters = AttributeList()
  ) = 0;
  virtual RowSet select_all(
    const std::string& query,
    const AttributeList &parameters = AttributeList()
  ) = 0;
  virtual TableSet schema()                                      = 0;
  virtual Table table_data(const std::string& table_name)        = 0;
  virtual std::string primary_key(const std::string& table_name) = 0;
  virtual void remove_field(
    const std::string& table_name,
    const std::string &field_name
  ) = 0;

  private:

  TableSet tables_;
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_CONNECTION_H_
