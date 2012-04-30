#include <active_record/type.h>
#include <active_record/base.h>

//////////////////////////////////////////////
// User model(s)

using namespace ActiveRecord;

class Person: public ActiveRecord::Base< Person > {
 public:
  AR_CONSTRUCTORS( Person )
  // Callback to set table_name and override any defaults
  static Table table( Connection * connection ) {
    Table td( connection, "people" );
    td.fields() = fields
                  ( "name", ActiveRecord::text )
                  ( "surname", ActiveRecord::text )
                  ( "age", ActiveRecord::integer )
                  ( "height", ActiveRecord::floating_point )
                  ( "dob", ActiveRecord::date );
    return td;
  }
};

AR_DECLARE( Person )

//////////////////////////////////////////////

#include <active_record/connection.h>
#include <active_record/query.h>
#include <iostream>

int main( int argc, const char *argv[] ) {
  string database_file   = "people.sqlite3";
  string remove_database = "rm -f " + database_file;
  system( remove_database.c_str() );
  // Prepare
  Connection connection;
  connection.connect( options
                      ( "adapter", "sqlite" )
                      ( "database", database_file ) );
  Person::setup( &connection ); // Must be called for each model
  connection.update_database();

  // Insert data
  Person joe( attributes
              ( "name", "Joe" )
              ( "surname", "Yates" )
              ( "age", 45 )
              ( "height", 1.80 )
              ( "dob", Date::parse( "1964-01-24" ) ) );
  joe.save();

  Person john;
  john.init( attributes
             ( "name", "John" )
             ( "surname", "Smith" )
             ( "age", 43 )
             ( "height", 1.90 ) ).save();

  Person petra( attributes
                ( "name",    "Petra" )
                ( "surname", "Smith" )
                ( "age",     25 )
                ( "height",  1.70 ) );
  petra.save();

  // Load by id
  Person loaded_joe( 1 );
  cout << "Joe's surname: " << loaded_joe[ "surname" ] << endl;
  cout << "Joe's date of birth: " << loaded_joe[ "dob" ] << endl;

  // Update
  loaded_joe[ "surname" ] = "Bates";
  loaded_joe.save();

  // Run a query
  vector< Person > people = ActiveRecord::Query< Person >( connection )
    .where( "surname = ?", "Smith" )
    .where( "age <= ?", 45 )
    .order( "name" )
    .limit( 5 )
    .all();
  int count = people.size();
  cout << "Result count: " << count << endl;
  for( vector< Person >::iterator it = people.begin(); it != people.end(); ++it ) {
    cout << (*it)[ "name" ] << " ( #" << (*it)[ "id" ] << " ), ";
    cout << "age: " << (*it)[ "age" ] << ", ";
    cout << "height: " << (*it)[ "height" ] << endl;
  }

  return 0;
}
