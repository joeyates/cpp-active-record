#include <active_record/table.h>

#include <active_record/active_record.h>
#include <active_record/exception.h>
#include <active_record/connection.h>
#include <sstream>
using namespace std;

namespace ActiveRecord {

extern TypeNameMap type_name;

void Table::add_field( const Field &field ) {
  log( "add_field" );
  assert_connection( __FILE__, __LINE__ );
  stringstream ss;
  ss << "ALTER TABLE " << table_name_;
  ss << " ADD " << field.name() << " " << type_name[ field.type() ];
  ss << ";";
  log( ss.str() );
  connection_->execute( ss.str() );
}

/*
SQLite does not implement DROP COLUMN - needs some hoop jumping
void Table::remove_field( const Field &field )
{
  assert_connection();
  stringstream ss;
  ss << "ALTER TABLE " << table_name_;
  ss << " DROP " << field.name();
  ss << ";";
  connection_->execute( ss.str() );
}
*/

void Table::fields( const GenericFieldList &f ) {
  fields_.assign( f.begin(), f.end() );
}

////////////////////////////////
// private

void Table::assert_connection( const char * file, int line ) {
  if( connection_ != NULL )
    return;
  throw ActiveRecordException( "No connection", file, line );
}

} // namespace ActiveRecord

ostream & operator<<( ostream &cout, const ActiveRecord::Table &table ) {
  cout << table.table_name_ << ": ";
  for( ActiveRecord::Fields::const_iterator it = table.fields_.begin();
       it != table.fields_.end();
       ++it ) {
    if( it != table.fields_.begin() )
      cout << ", ";
    cout << *it;
  }
  return cout;
}
