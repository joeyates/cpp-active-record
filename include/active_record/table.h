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
  Table( Connection * connection = NULL, const string &table_name = "" ) :
    primary_key_( "id" ),
    timestamps_( false ),
    connection_( connection ),
    table_name_( table_name )
  {}
 public:
  Connection *       connection() const                       { return connection_; }
  void               primary_key( const string &primary_key ) { primary_key_ = primary_key; }
  string             primary_key() const                      { return primary_key_; }
  void               timestamps( bool timestamps )            { timestamps_ = timestamps; }
  bool               timestamps() const                       { return timestamps_; }
  string             table_name() const                       { return table_name_; }
  Fields &           fields()                                 { return fields_; }
  void               add_field( const Field &field );
  void               remove_field( const Field &field );
 private:
  void               assert_connection();
 private:
  Connection * connection_;
  string       primary_key_;
  string       table_name_;
  bool         timestamps_;
  Fields       fields_;
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TABLE_H_
