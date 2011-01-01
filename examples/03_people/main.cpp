#include <active_record/types.h>
#include <active_record/base.h>

//////////////////////////////////////////////
// User model(s)

using namespace ActiveRecord;

class Person: public ActiveRecord::Base< Person> {
 public:
  Person() : ActiveRecord::Base< Person>() {} // Necessary
  Person( int id ) : ActiveRecord::Base< Person >( id ) {} // Necessary
  // Callback to set table_name and override any defaults
  static void set_table_data( TableData &td ) {
    td.table_name = "people";
    td.fields.push_back( Field( "name", ActiveRecord::text ) );
    td.fields.push_back( Field( "surname", ActiveRecord::text ) );
    td.fields.push_back( Field( "age", ActiveRecord::integer ) );
    td.fields.push_back( Field( "height", ActiveRecord::floating_point ) );
    td.update_database = true;
  }
};

// Necessary: Define the name of the table, so the class knows it's own name
template <>
string ActiveRecord::Base< Person >::class_name = "Person";

//////////////////////////////////////////////

#include <active_record/connection.h>
#include <active_record/query.h>
#include <iostream>

int main( int argc, const char *argv[] ) {
  // Prepare
  ActiveRecord::Connection connection;
  connection.connect( options
    ( "adapter", "sqlite" )
    ( "database", "people.sqlite3" ) );
  Person::setup( &connection ); // Must be called for each model

  // Insert data
  Person joe, john, petra;
  joe[ "name" ]    = "Joe";
  joe[ "surname" ] = "Yates";
  joe[ "age" ]     = 45;
  joe[ "height" ]  = 1.80;
  joe.save();
  john[ "name" ]    = "John";
  john[ "surname" ] = "Smith";
  john[ "age" ]     = 43;
  john[ "height" ]  = 1.90;
  john.save();
  petra[ "name" ]    = "Petra";
  petra[ "surname" ] = "Smith";
  petra[ "age" ]     = 25;
  petra[ "height" ]  = 1.70;
  petra.save();

  // Load by id
  Person loaded_joe( 1 );
  cout << "Joe's surname: " << loaded_joe.text( "surname" ) << endl;

  // Update
  loaded_joe[ "surname" ] = "Bates";
  loaded_joe.save();

  // Run a query
  vector< Person > people = ActiveRecord::Query< Person >()
    .where( "surname = ?", "Smith" )
    .where( "age <= ?", 45 )
    .order( "name" )
    .limit( 5 )
    .all();
  int count = people.size();
  cout << "Result count: " << count << endl;
  for( int i = 0; i < count; ++i ) {
    int id        = people[ i ].integer(" id" );
    int age       = people[ i ].integer( "age" );
    double height = people[ i ].floating_point("height");
    string name   = people[ i ].text("name");
    cout << name << " (" << id << "), age: " << age << ", height: " << height << endl;
  }
}
