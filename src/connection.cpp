#include <active_record/connection.h>

#include <sstream>
#include <active_record/exception.h>
#include <active_record/active_record.h>

namespace ActiveRecord {

// Connection

// Tables
void Connection::set_table( const string &class_name, const Table &table ) {
  log( "Connection::set_table" );
  log( class_name );
  tables_[ class_name ] = table;
}

Table & Connection::get_table( const string &class_name ) {
  return tables_[ class_name ];
}

void Connection::update_database() {
  log( "Connection::update_database" );
  tables_.update_database();
}

bool Connection::table_exists( const string &table_name ) {
  AttributeList parameters;
  parameters.push_back( table_name );
  RowSet rows = select_all( "SELECT name FROM sqlite_master WHERE type='table' AND name = ?;",
                            parameters );
  return ( rows.size() ? true : false );
}

//////////////////////////////////
// Transactions

void Connection::begin_transaction() {
  execute( "BEGIN TRANSACTION" );
}

void Connection::commit() {
  execute( "COMMIT" );
}

} // namespace ActiveRecord

