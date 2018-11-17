#ifndef _ACTIVE_RECORD_ROW_H_
#define _ACTIVE_RECORD_ROW_H_

#include <list>
#include <string>
#include <iostream>
#include <boost/assign.hpp>
#include <boost/variant.hpp>
#include <sqlite3.h>
#include <postgresql/libpq-fe.h>

#include <active_record/type.h>
#include <active_record/date.h>
#include <active_record/attribute.h>

using namespace std;

namespace ActiveRecord {

class Row {
  friend ostream& operator<<(ostream& cout, const Row& row);

  public:

  Row() {};
  Row(sqlite3_stmt* ppStmt);
  Row(PGresult* exec_result, int row);

  bool has_data() { return attributes_.size() > 0; };
  Type::Type get_type(const string& name);
  bool is_type(const string& name, Type::Type type);
  string get_text(const string& name);
  int get_integer(const string& name);
  double get_floating_point(const string &name);
  Date get_date(const string& name);
  string to_string() const;
  const AttributeHash& attributes() { return attributes_; };

  private:

  AttributeHash attributes_;
};

typedef list<Row> RowSet;

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_ROW_H_
