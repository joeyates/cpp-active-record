#ifndef _ACTIVE_RECORD_TABLE_SET_H_
#define _ACTIVE_RECORD_TABLE_SET_H_

#include <active_record/table.h>

namespace ActiveRecord {

class Connection;

class TableSet: public map<string, Table> {
  public:

  static void create_table(Table& td);
  static void update_table(Table& td);

  // Data Definition / Database Structure
  void update_database();
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TABLE_SET_H_
