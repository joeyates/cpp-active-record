#ifndef _ACTIVE_RECORD_TABLE_H_
#define _ACTIVE_RECORD_TABLE_H_

#include <string>
#include <vector>
#include <active_record/field.h>
#include <active_record/types.h>
using namespace std;

namespace ActiveRecord {

class Connection;

struct Table {
  Table() {
    primary_key = "id";
    timestamps  = false;
  }
  Connection *  connection;
  string        primary_key;
  string        table_name;
  bool          timestamps;
  Fields        fields;
};

typedef map< string, Table >               TableSet;
typedef map< string, Table >               Schema;

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TABLE_H_
