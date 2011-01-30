#include <sstream>
#include <active_record/table.h>
#include <active_record/connection.h>
using namespace std;

namespace ActiveRecord {

extern TypeNameMap type_name;

void Table::add_field( const Field &field )
{
  stringstream ss;
  ss << "ALTER TABLE " << table_name;
  ss << " ADD " << field.name() << " " << type_name[ field.type() ];
  ss << ";";
  connection->execute( ss.str() );
}

void Table::remove_field( const Field &field )
{
  stringstream ss;
  ss << "ALTER TABLE " << table_name;
  ss << " REMOVE " << field.name();
  ss << ";";
  connection->execute( ss.str() );
}

} // namespace ActiveRecord
