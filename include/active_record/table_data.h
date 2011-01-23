#ifndef _ACTIVE_RECORD_TABLE_DATA_H_
#define _ACTIVE_RECORD_TABLE_DATA_H_

#include <string>
#include <vector>
#include <active_record/types.h>
using namespace std;

namespace ActiveRecord {

class Connection;

class Field {
 public:
  Field( const string &name, ActiveRecord::Type type ) : name_( name ), type_( type ) {}
  inline const string& name() { return name_; }
  inline ActiveRecord::Type type() { return type_; }
 private:
  string name_;
  ActiveRecord::Type type_;
};

struct TableData {
  TableData() {
    primary_key = "id";
    timestamps  = false;
  }
  Connection *  connection;
  string        primary_key;
  string        table_name;
  bool          timestamps;
  vector< Field > fields;
};

typedef pair< ActiveRecord::Type, string > TypeNamePair;
typedef map< ActiveRecord::Type, string >  TypeNameMap;
typedef map< string, TableData >           TableSet;
typedef map< string, TableData >           Schema;

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TABLE_DATA_H_
