#include <active_record/row.h>

#include <sqlite3.h>

namespace ActiveRecord {

Row::Row(sqlite3_stmt* pStmt) {
  int count = sqlite3_column_count(pStmt);
  for(int i = 0; i < count; ++i) {
    string name = sqlite3_column_name(pStmt, i);
    attributes_[name] = Attribute::from_field(pStmt, i);
  }
}

}
