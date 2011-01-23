#include <iostream>
#include <active_record/connection.h>

namespace ActiveRecord {

extern TypeNameMap type_name;
extern TableSet    tables;

Connection::Connection() {}

Connection::Connection( const Connection& other )
{
}

Connection Connection::operator=( const Connection& other )
{
  return *this;
}

void Connection::connect( OptionsHash options )
{
  sqlite_initialize( options[ "database" ] );
}

////////////////////////////////////
// Data Definition / Database Structure

// TODO: Handle alter table
void Connection::update_database()
{
  for( vector< string >::iterator it = klasses_.begin(); it != klasses_.end(); ++it )
    update_table( *it );
}

void Connection::update_table( const string &klass )
{
  TableData td = tables[ klass ];
  stringstream ss;
  ss << "CREATE TABLE IF NOT EXISTS " << td.table_name;
  ss << " (";
  ss << td.primary_key << " INTEGER PRIMARY KEY";
  for( vector< Field >::iterator it = td.fields.begin(); it != td.fields.end(); ++it ) {
    ss << ", " << it->name() << " " << type_name[ it->type() ];
  }
  if( td.timestamps ) {
    ss << ", created_at TEXT";
    ss << ", updated_at TEXT";
  }
  ss << ");";
  execute( ss.str() );
}

Schema Connection::schema()
{
  Schema s;
  RowSet rows = select_values( "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;" );
  for( RowSet::iterator it = rows.begin(); it != rows.end(); ++it ) {
    Type type = it->get_type( "name" );
    string table = it->get_text( "name" );
    s[ table ] = table_data( table );
  }
  return s;
}

TableData Connection::table_data( const string &table_name )
{
  TableData td;
  stringstream row_query;
  row_query << "PRAGMA table_info( \"" << table_name << "\" );";
  string query = row_query.str();
  sqlite3_stmt *ppStmt = 0;
  int prepare_result = sqlite3_prepare_v2( db_, query.c_str(), query.size(), &ppStmt, 0 );
  while( sqlite3_step( ppStmt ) == SQLITE_ROW ) {
    // cid | name |    type | notnull | dflt_value | pk
    // 0   |  bar | INTEGER |       0 |            | 0
    const char * name      = ( const char * ) sqlite3_column_text( ppStmt, 1 );
    const char * type_name = ( const char * ) sqlite3_column_text( ppStmt, 2 );
    ActiveRecord::Type type = ActiveRecord::to_type( type_name );
    if( type == ActiveRecord::unknown )
      throw "Unknown type";
    td.fields.push_back( Field( name, type ) );
  }
  return td;
}

//////////////////////////////////
// Transactions

void Connection::begin_transaction()
{
  execute( "BEGIN TRANSACTION" );
}

void Connection::commit()
{
  execute( "COMMIT" );
}

bool Connection::execute( const string &query, const AttributeList &parameters )
{
  sqlite3_stmt *ppStmt = 0;
  int prepare_result = sqlite3_prepare_v2( db_, query.c_str(), query.size(), &ppStmt, 0 );
  // TODO: check prepare_result
  bind_parameters( ppStmt, parameters );
  sqlite3_step( ppStmt );
  return true;
}

Row Connection::select_one( const string &query, const AttributeList &parameters )
{
  sqlite3_stmt *ppStmt = 0;
  int prepare_result = sqlite3_prepare_v2( db_, query.c_str(), query.size(), &ppStmt, 0 );
  // TODO: check prepare_result
  bind_parameters( ppStmt, parameters );
  int step_result = sqlite3_step( ppStmt );
  if( step_result != SQLITE_ROW )
    throw "No data";
  return Row( ppStmt );
}

RowSet Connection::select_values( const string &query, const AttributeList &parameters )
{
  sqlite3_stmt *ppStmt = 0;
  int prepare_result = sqlite3_prepare_v2( db_, query.c_str(), query.size(), &ppStmt, 0 );
  // TODO: check prepare_result
  bind_parameters( ppStmt, parameters );
  RowSet results;
  while( sqlite3_step( ppStmt ) == SQLITE_ROW ) {
    results.push_back( Row( ppStmt ) );
  }
  return results;
}

////////////////////////////////////////
// Private

bool Connection::sqlite_initialize( string database_path_name )
{
  int nResult = sqlite3_open( database_path_name.c_str(), &db_ );
  if( nResult ) {
    fprintf( stderr, "Can't open database '%s': %s\n", database_path_name.c_str(), sqlite3_errmsg( db_ ) );
    sqlite3_close( db_ );
    return false;
  }
}

void Connection::bind_parameters( sqlite3_stmt *ppStmt, const AttributeList &parameters )
{
  for( int i = 0; i < parameters.size(); ++i ) {
    switch( parameters[ i ].which() ) {
    case integer: {
      int value = boost::get< int >( parameters[ i ] );
      sqlite3_bind_int( ppStmt, i + 1, value );
      break;
    }
    case text: {
      string value = boost::get< std::string >( parameters[ i ] );
      sqlite3_bind_text( ppStmt, i + 1, value.c_str(), value.size(), 0 );
      break;
    }
    case floating_point: {
      double value = boost::get< double >( parameters[ i ] );
      sqlite3_bind_double( ppStmt, i + 1, value );
      break;
    }
    default:
      throw "Type not implemented";
    }
  }
}

} // namespace ActiveRecord
