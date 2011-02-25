#include <active_record/type.h>
#include <active_record/base.h>

//////////////////////////////////////////////

using namespace ActiveRecord;

class Greeting: public ActiveRecord::Base< Greeting > {
 public:
  AR_CONSTRUCTORS( Greeting )
  static void table() {
    Table td;
    td.table_name = "greetings";
    td.fields() = fields
                  ( "salute",   ActiveRecord::text )
                  ( "thing",    ActiveRecord::text )
                  ( "language", ActiveRecord::text );
    return td;
  }
};

template <>
string ActiveRecord::Base< Greeting >::class_name = "Greeting";

//////////////////////////////////////////////

#include <active_record/connection.h>
#include <active_record/query.h>
#include <iostream>

int main( int argc, const char *argv[] ) {
  // Prepare
  ActiveRecord::Connection connection;
  connection.connect( options
    ( "adapter", "sqlite" )
    ( "database", "greetings.sqlite3" ) );
  Greeting::setup( &connection );

  // Insert data
  Greeting hello, ciao;
  hello[ "salute" ]   = "Hello";
  hello[ "thing" ]    = "World";
  hello[ "language" ] = "English";
  hello.save();
  ciao[ "salute" ]   = "Ciao";
  ciao[ "thing" ]    = "Mondo";
  ciao[ "language" ] = "Italian";
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
