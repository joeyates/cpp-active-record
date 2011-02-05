#ifndef _ACTIVE_RECORD_TABLE_H_
#define _ACTIVE_RECORD_TABLE_H_

#include <string>
#include <vector>
#include <active_record/field.h>
#include <active_record/type.h>
using namespace std;

namespace ActiveRecord {

class Connection;

class Table {
 public:
  Table() :
    primary_key_( "id"),
    timestamps_( false )
  {}
  Table( Connection * connection, const string &table_name ) :
    connection_( connection ),
    table_name_( table_name )
  {}
 public:
  Connection * connection()                             { return connection_; }
  void         primary_key( const string &primary_key ) { primary_key_ = primary_key; }
  string       primary_key()                            { return primary_key_; }
  void         timestamps( bool timestamps )            { timestamps_ = timestamps; }
  bool         timestamps()                             { return timestamps_; }
  string       table_name()                             { return table_name_; }
  Fields &     fields()                                 { return fields_; }
  void         add_field( const Field &field );
  void         remove_field( const Field &field );
 private:
  Connection * connection_;
  string       primary_key_;
  string       table_name_;
  bool         timestamps_;
  Fields       fields_;
};

typedef map< string, Table >               TableSet;
typedef map< string, Table >               Schema;

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TABLE_H_
