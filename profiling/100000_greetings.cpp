#include <active_record/base.h>
#include <active_record/connection.h>
#include <active_record/query.h>

using namespace ActiveRecord;

class Greeting: public ActiveRecord::Base< Greeting > {
 public:
  AR_CONSTRUCTORS( Greeting )
  static Table table( Connection * connection ) {
    Table td( connection, "greetings" );
    td.fields() = fields  ( "salute",   ActiveRecord::text )
                          ( "thing",    ActiveRecord::text )
                          ( "language", ActiveRecord::text );
    return td;
  }
};

AR_DECLARE( Greeting )

string database_file = "greetings.sqlite3";

void delete_database() {
  string remove_database = "rm -f " + database_file;
  system( remove_database.c_str() );
}

void ar_insert() {
  Greeting hello( attributes
                  ( "salute", "Hello" )
                  ( "thing", "World" )
                  ( "language", "English" ) );
  hello.save();
  Greeting ciao( attributes
                 ( "salute", "Ciao" )
                 ( "thing", "Mondo" )
                 ( "language", "Italian" ) );
  ciao.save();
}

void ar_setup() {
  delete_database();
  connection.connect( options
                      ( "adapter", "sqlite" )
                      ( "database", database_file ) );
  Greeting::setup( &connection );
  connection.update_database();
  ar_insert();
}

void ar_close() {
  connection.disconnect();
}

void ar_query_retrieve() {
  int t = 0;
  for( int i = 0; i < 100000; ++i ) {
    Query< Greeting > q;
    vector< Greeting > greetings = q.all();
    t += greetings.size();
  }
  cout << t << endl;
}

void ar_connection_retrieve() {
  int t = 0;
  for( int i = 0; i < 100000; ++i ) {
    RowSet rows = connection.select_all( "SELECT * FROM greetings;" );
    t += rows.size();
  }
  cout << t << endl;
}

void sqlite_retrieve() {
  sqlite3 * db;
  int nResult = sqlite3_open( database_file.c_str(), &db );
  int t = 0;
  for( int i = 0; i < 100000; ++i ) {
    sqlite3_stmt *pStmt = 0;
    string query = "SELECT * FROM greetings;";
    int prepare_result = sqlite3_prepare_v2( db, query.c_str(), query.size(), &pStmt, 0 );
    while( sqlite3_step( pStmt ) == SQLITE_ROW ) {
      int count = sqlite3_column_count( pStmt );
      vector< string > row;
      for( int i = 0; i < count; ++i ) {
        const char * value = ( const char * ) sqlite3_column_text( pStmt, i );
        row.push_back( value );
        ++t;
      }
    }
    sqlite3_finalize( pStmt );
  }
  cout << t << endl;
  sqlite3_close( db );
}

int main( int argc, char **argv ) {
  ar_setup();
  ar_query_retrieve();
  ar_close();

  //sqlite_retrieve();

  return 0;
}
