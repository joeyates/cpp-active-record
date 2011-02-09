#ifndef _ACTIVE_RECORD_TABLE_SET_H_
#define _ACTIVE_RECORD_TABLE_SET_H_

#include <active_record/table.h>

namespace ActiveRecord {

class TableSet : public map< string, Table > {
 public:
  static TableSet  schema( Connection * connection );
  static Table     table_data( Connection * connection, const string &table_name );
  static void      create_table( Table &td );
  static void      update_table( Table &td );
 public:
  // Data Definition / Database Structure
  void      update_database();
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TABLE_SET_H_
