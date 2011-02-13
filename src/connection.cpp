#include <iostream>
#include <sstream>
#include <active_record/connection.h>
#include <active_record/exception.h>

namespace ActiveRecord {

extern TypeNameMap type_name;
extern TableSet    tables;

Connection::Connection() {}

Connection::Connection( const Connection& other ) {}

Connection Connection::operator=( const Connection& other ) {
  return *this;
}

void Connection::connect( OptionsHash options ) {
  sqlite_initialize( options[ "database" ] );
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

bool Connection::execute( const string &query,
                          const AttributeList &parameters ) {
  sqlite3_stmt *ppStmt = prepare( query, parameters );
  sqlite3_step( ppStmt );
  return true;
}

long Connection::insert( const string &query,
                         const AttributeList &parameters ) {
  sqlite3_stmt *ppStmt = prepare( query, parameters );
  sqlite3_step( ppStmt );
  return sqlite3_last_insert_rowid( db_ );
}

Row Connection::select_one( const string &query,
                            const AttributeList &parameters ) {
  sqlite3_stmt *ppStmt = prepare( query, parameters );
  int step_result = sqlite3_step( ppStmt );
  if( step_result != SQLITE_ROW ) {
    cerr << "No data" << endl;
    throw "No data";
  }
  return Row( ppStmt );
}

RowSet Connection::select_all( const string &query,
                               const AttributeList &parameters ) {
  sqlite3_stmt *ppStmt = prepare( query, parameters );
  RowSet results;
  while( sqlite3_step( ppStmt ) == SQLITE_ROW ) {
    results.push_back( Row( ppStmt ) );
  }
  return results;
}

////////////////////////////////////////
// Private

bool Connection::sqlite_initialize( string database_path_name ) {
  int nResult = sqlite3_open( database_path_name.c_str(), &db_ );
  if( nResult ) {
    fprintf( stderr, "Can't open database '%s': %s\n", database_path_name.c_str(), sqlite3_errmsg( db_ ) );
    sqlite3_close( db_ );
    return false;
  }
}

sqlite3_stmt * Connection::prepare( const string &query,
                                    const AttributeList &parameters ) {
  sqlite3_stmt *ppStmt = 0;
  int prepare_result = sqlite3_prepare_v2( db_, query.c_str(), query.size(), &ppStmt, 0 );
  if( prepare_result != 0 ) {
    cerr << __FILE__ << "(" << __LINE__ << "): SQL error: '" << query << "'" << endl;
    throw ActiveRecordException( "Query preparation failed" );
  }
  bind_parameters( ppStmt, parameters );
  return ppStmt;
}

void Connection::bind_parameters( sqlite3_stmt *ppStmt,
                                  const AttributeList &parameters ) {
  int i = 0;
  for( AttributeList::const_iterator it = parameters.begin();
       it != parameters.end();
       ++it ) {
    switch( it->which() ) {
    case integer: {
      int value = boost::get< int >( *it );
      sqlite3_bind_int( ppStmt, i + 1, value );
      break;
    }
    case text: {
      string value = boost::get< std::string >( *it );
      sqlite3_bind_text( ppStmt, i + 1, value.c_str(), value.size(), 0 );
      break;
    }
    case floating_point: {
      double value = boost::get< double >( *it );
      sqlite3_bind_double( ppStmt, i + 1, value );
      break;
    }
    default: {
      cerr <<  "Type not implemented" << endl;
      throw "Type not implemented";
    }
    }
    ++i;
  }
}

} // namespace ActiveRecord
