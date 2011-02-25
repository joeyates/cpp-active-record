#include <active_record/type.h>
#include <active_record/base.h>

//////////////////////////////////////////////

using namespace ActiveRecord;

class Greeting: public ActiveRecord::Base< Greeting > {
 public:
  AR_CONSTRUCTORS( Greeting )
  static Table table( Connection * connection ) {
    Table td( connection, "greetings" );
    td.fields() = fields
                  ( "salute",   ActiveRecord::text )
                  ( "thing",    ActiveRecord::text )
                  ( "language", ActiveRecord::text );
    return td;
  }
};

template <>
string ActiveRecord::Base< Greeting >::class_name = "Greeting";

extern ActiveRecord::Connection ActiveRecord::connection;

//////////////////////////////////////////////

#include <active_record/connection.h>
#include <active_record/query.h>
#include <iostream>

int main( int argc, const char *argv[] ) {
  string database_file   = "greetings.sqlite3";
  string remove_database = "rm -f " + database_file;
  system( remove_database.c_str() );
  // Prepare
  connection.connect( options
                      ( "adapter", "sqlite" )
                      ( "database", database_file ) );
  Greeting::setup( &connection );
  connection.update_database();

  // Insert data
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

  vector< Greeting > greetings = ActiveRecord::Query< Greeting >()
    .order( "language" )
    .limit( 5 )
    .all();
  for( int i = 0; i < greetings.size(); ++i ) {
    Greeting greeting = greetings[ i ];
    cout << "In " << greeting.text( "language" ) << " you say: ";
    cout << "'" << greeting.text( "salute" ) << " " << greeting.text( "thing" ) << "!'" << endl;
  }
}
