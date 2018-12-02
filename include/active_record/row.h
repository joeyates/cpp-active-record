#ifndef _ACTIVE_RECORD_ROW_H_
#define _ACTIVE_RECORD_ROW_H_

#include <cstdint>
#include <list>
#include <string>
#include <iostream>
#include <boost/assign.hpp>
#include <boost/variant.hpp>
#ifdef AR_POSTGRES
#include <postgresql/libpq-fe.h>
#endif // def AR_POSTGRES
#ifdef AR_SQLITE
#include <sqlite3.h>
#endif // def AR_SQLITE

#include <active_record/type.h>
#include <active_record/date.h>
#include <active_record/attribute.h>

namespace ActiveRecord {

class Row {
  friend std::ostream& operator<<(std::ostream& cout, const Row& row);

  public:

  Row() {};
#ifdef AR_POSTGRES
  Row(PGresult* exec_result, int row);
#endif // def AR_POSTGRES
#ifdef AR_SQLITE
  Row(sqlite3_stmt* pStmt);
#endif // def AR_SQLITE

  bool has_data() { return attributes_.size() > 0; };
  Type::Type get_type(const std::string& name);
  bool is_type(const std::string& name, Type::Type type);
  std::string get_text(const std::string& name);
  int64_t get_integer(const std::string& name);
  double get_floating_point(const std::string& name);
  Date get_date(const std::string& name);
  std::string to_string() const;
  const AttributeHash& attributes() { return attributes_; };

  private:

  AttributeHash attributes_;
};

typedef std::list<Row> RowSet;

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_ROW_H_
