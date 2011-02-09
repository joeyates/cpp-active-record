#include <active_record/table_set.h>
#include <active_record/type.h>
#include <active_record/connection.h>

namespace ActiveRecord {

extern TypeNameMap type_name;

TableSet TableSet::schema( Connection * connection )
{
  TableSet s;
  RowSet rows = connection->select_all( "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;" );
  for( RowSet::iterator it = rows.begin(); it != rows.end(); ++it ) {
    Type type    = it->get_type( "name" );
    string table = it->get_text( "name" );
    s[ table ]   = table_data( connection, table );
  }
  return s;
}

Table TableSet::table_data( Connection * connection, const string &table_name )
{
  stringstream row_query;
  row_query << "PRAGMA table_info( \"" << table_name << "\" );";
  string query         = row_query.str();
  sqlite3_stmt *ppStmt = 0;
  int prepare_result   = sqlite3_prepare_v2( connection->db_, query.c_str(), query.size(), &ppStmt, 0 );
  Table td( connection, table_name );
  while( sqlite3_step( ppStmt ) == SQLITE_ROW ) {
    // cid | name |    type | notnull | dflt_value | pk
    // 0   |  bar | INTEGER |       0 |            | 0
    const char * name       = ( const char * ) sqlite3_column_text( ppStmt, 1 );
    const char * type_name  = ( const char * ) sqlite3_column_text( ppStmt, 2 );
    ActiveRecord::Type type = ActiveRecord::to_type( type_name );
    if( type == ActiveRecord::unknown ) {
      cerr << "Unknown type: " << type_name << endl;
      throw "Unknown type";
    }
    td.fields().push_back( Field( name, type ) );
  }
  return td;
}

void TableSet::create_table( Table &td )
{
  stringstream ss;
  ss << "CREATE TABLE " << td.table_name();
  ss << " (";
  ss << td.primary_key() << " INTEGER PRIMARY KEY";
  for( Fields::const_iterator it = td.fields().begin(); it != td.fields().end(); ++it ) {
    ss << ", " << it->name() << " " << type_name[ it->type() ];
  }
  if( td.timestamps() ) {
    ss << ", created_at TEXT";
    ss << ", updated_at TEXT";
  }
  ss << ");";
  td.connection()->execute( ss.str() );
}

void TableSet::update_table( Table &required )
{
  Table existing = table_data( required.connection(), required.table_name() );
  Fields missing = required.fields() - existing.fields();
  Fields remove  = existing.fields() - required.fields();
  for( Fields::iterator it = missing.begin(); it != missing.end(); ++it )
    existing.add_field( *it );
  for( Fields::iterator it = remove.begin(); it != remove.end(); ++it ) {
    cerr << __FILE__ << "(" << __LINE__ << "): Table::remove_field not yet implemented" << endl;
    throw "Table::remove_field not yet implemented";
    //existing.remove_field( *it );
  }
}

////////////////////////////////////
// Data Definition / Database Structure

void TableSet::update_database()
{
  for( TableSet::iterator it = begin(); it != end(); ++it ) {
    Table td = it->second;
    if( td.connection()->table_exists( td.table_name() ) )
      update_table( td );
    else
      create_table( td );
  }
}

} // namespace ActiveRecord
