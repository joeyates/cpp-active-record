#include <sstream>
#include <active_record/table.h>
#include <active_record/connection.h>
using namespace std;

namespace ActiveRecord {

extern TypeNameMap type_name;

void Table::add_field( const Field &field ) {
  assert_connection();
  stringstream ss;
  ss << "ALTER TABLE " << table_name_;
  ss << " ADD " << field.name() << " " << type_name[ field.type() ];
  ss << ";";
  connection_->execute( ss.str() );
}

/*
SQLite does not implement DROP COLUMN - needs some loop jumping
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

void Table::assert_connection() {
  if( connection_ != NULL )
    return;
  cerr <<  "No connection" << endl;
  throw "No connection";
}

} // namespace ActiveRecord
