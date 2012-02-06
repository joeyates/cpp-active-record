#include <active_record/sqlite3_connection.h>

namespace ActiveRecord {

Sqlite3Connection::Sqlite3Connection() : db_( NULL ) {}

Sqlite3Connection::~Sqlite3Connection() {
  disconnect();
}

Sqlite3Connection::Sqlite3Connection( const Sqlite3Connection& other ) {}

Sqlite3Connection Sqlite3Connection::operator=( const Sqlite3Connection& other ) {
  return *this;
}

/////////////////////////////////////////////////////
// Connection

void Sqlite3Connection::connect( OptionsHash options ) {
  log( options[ "database" ] );
  sqlite_initialize( options[ "database" ] );
}

void Sqlite3Connection::disconnect() {
  if( db_ != NULL) {
    sqlite3_close( db_ );
    db_ = NULL;
  }
}

bool Sqlite3Connection::connected() {
  return db_ != NULL ? true : false;
}

/////////////////////////////////////////////////////
// Database Structure

bool Sqlite3Connection::table_exists( const string &table_name ) {
  AttributeList parameters;
  parameters.push_back( table_name );
  RowSet rows = select_all( "SELECT name FROM sqlite_master WHERE type='table' AND name = ?;",
                            parameters );
  return ( rows.size() ? true : false );
}

bool Sqlite3Connection::execute( const string &query,
                          const AttributeList &parameters ) {
  log( "Sqlite3Connection::execute" );
  log( query );
  sqlite3_stmt *ppStmt = prepare( query, parameters );
  sqlite3_step( ppStmt );
  sqlite3_finalize( ppStmt );
  return true;
}

long Sqlite3Connection::insert( const string &query,
                         const AttributeList &parameters ) {
  log( "Sqlite3Connection::insert" );
  log( query );
  sqlite3_stmt *ppStmt = prepare( query, parameters );
  sqlite3_step( ppStmt );
  sqlite3_finalize( ppStmt );
  return sqlite3_last_insert_rowid( db_ );
}

Row Sqlite3Connection::select_one( const string &query,
                            const AttributeList &parameters ) {
  sqlite3_stmt *ppStmt = prepare( query, parameters );
  int step_result = sqlite3_step( ppStmt );
  if( step_result != SQLITE_ROW ) {
    return Row();
  }
  Row row( ppStmt );
  sqlite3_finalize( ppStmt );
  return row;
}

RowSet Sqlite3Connection::select_all( const string &query,
                               const AttributeList &parameters ) {
  sqlite3_stmt *ppStmt = prepare( query, parameters );
  RowSet results;
  while( sqlite3_step( ppStmt ) == SQLITE_ROW ) {
    results.push_back( Row( ppStmt ) );
  }
  sqlite3_finalize( ppStmt );
  return results;
}

AttributeList Sqlite3Connection::select_values( const string &query,
					 const AttributeList &parameters ) {
  sqlite3_stmt *ppStmt = prepare( query, parameters );
  AttributeList results;
  while( sqlite3_step( ppStmt ) == SQLITE_ROW ) {
    results.push_back( Attribute::from_field( ppStmt, 0 ) );
  }
  sqlite3_finalize( ppStmt );
  return results;
}

////////////////////////////////////////
// Private

sqlite3_stmt * Sqlite3Connection::prepare( const string &query,
                                    const AttributeList &parameters ) {
  if( db_ == NULL )
    throw ActiveRecordException( "Database not connected", __FILE__, __LINE__ );
  sqlite3_stmt *ppStmt = 0;
  int prepare_result = sqlite3_prepare_v2( db_, query.c_str(), query.size(), &ppStmt, 0 );
  if( prepare_result != SQLITE_OK ) {
    stringstream error;
    error << "SQL error: \"" << sqlite_error( prepare_result ) << "\" ";
    error << "in \"" << query << "\"";
    bool added = false;
    for( AttributeList::const_iterator it = parameters.begin(); it != parameters.end(); ++it ) {
      error << ", ";
      if( ! added )
        error << "[";
      error << *it;
      added = true;
    }
    if( added )
      error << "]";
    log( error.str() );
    throw ActiveRecordException( error.str(), __FILE__, __LINE__ );
  }
  bind_parameters( ppStmt, parameters );
  return ppStmt;
}

void Sqlite3Connection::bind_parameters( sqlite3_stmt *ppStmt,
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
    case date: {
      Date value = boost::get< Date >( *it );
      string s   = value.to_string();
      sqlite3_bind_text( ppStmt, i + 1, s.c_str(), s.size(), 0 );
      break;
    }
    default: {
      throw ActiveRecordException( "Type not implemented", __FILE__, __LINE__ );
    }
    }
    ++i;
  }
}

//////////////////////
// SQLite-specific

bool Sqlite3Connection::sqlite_initialize( string database_path_name ) {
  int nResult = sqlite3_open( database_path_name.c_str(), &db_ );
  if( nResult ) {
    stringstream error;
    error << "Can't open database '" << database_path_name << "'";
    error << sqlite3_errmsg( db_ );
    sqlite3_close( db_ );
    throw ActiveRecordException( error.str(), __FILE__, __LINE__ );
  }
  return true;
}

string Sqlite3Connection::sqlite_error( int error_code ) {
  string error;
  switch( error_code ) {
  case SQLITE_ERROR:      error = "SQL error or missing database";               break;
  case SQLITE_INTERNAL:   error = "Internal logic error in SQLite";              break;
  case SQLITE_PERM:       error = "Access permission denied";                    break;
  case SQLITE_ABORT:      error = "Callback routine requested an abort";         break;
  case SQLITE_BUSY:       error = "The database file is locked";                 break;
  case SQLITE_LOCKED:     error = "A table in the database is locked";           break;
  case SQLITE_NOMEM:      error = "A malloc() failed";                           break;
  case SQLITE_READONLY:   error = "Attempt to write a readonly database";        break;
  case SQLITE_INTERRUPT:  error = "Operation terminated by sqlite3_interrupt()"; break;
  case SQLITE_IOERR:      error = "Some kind of disk I/O error occurred";        break;
  case SQLITE_CORRUPT:    error = "The database disk image is malformed";        break;
  case SQLITE_NOTFOUND:   error = "Unknown opcode in sqlite3_file_control()";    break;
  case SQLITE_FULL:       error = "Insertion failed because database is full";   break;
  case SQLITE_CANTOPEN:   error = "Unable to open the database file";            break;
  case SQLITE_PROTOCOL:   error = "Database lock protocol error";                break;
  case SQLITE_EMPTY:      error = "Database is empty";                           break;
  case SQLITE_SCHEMA:     error = "The database schema changed";                 break;
  case SQLITE_TOOBIG:     error = "String or BLOB exceeds size limit";           break;
  case SQLITE_CONSTRAINT: error = "Abort due to constraint violation";           break;
  case SQLITE_MISMATCH:   error = "Data type mismatch";                          break;
  case SQLITE_MISUSE:     error = "Library used incorrectly";                    break;
  case SQLITE_NOLFS:      error = "Uses OS features not supported on host";      break;
  case SQLITE_AUTH:       error = "Authorization denied";                        break;
  case SQLITE_FORMAT:     error = "Auxiliary database format error";             break;
  case SQLITE_RANGE:      error = "2nd parameter to sqlite3_bind out of range";  break;
  case SQLITE_NOTADB:     error = "File opened that is not a database file";     break;
  case SQLITE_ROW:        error = "sqlite3_step() has another row ready";        break;
  case SQLITE_DONE:       error = "sqlite3_step() has finished executing";       break;
  default:                error = "Unknown error";                               break;
  }
  return error;
}

} // namespace ActiveRecord

