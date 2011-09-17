ActiveRecord - a C++ implementation of the Active Record pattern.

Quick Start Guide
=================

Here's a Hello World! example, that saves a record to the database, and retrieves it:

```c++
#include <active_record/type.h>
#include <active_record/base.h>
#include <active_record/connection.h>
#include <active_record/query.h>
#include <iostream>

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

AR_DECLARE( Greeting )

int main( int argc, const char *argv[] ) {
  Connection connection;
  connection.connect( options
                      ( "adapter", "sqlite" )
                      ( "database", "greetings.sqlite3" ) );
  Greeting::setup( &connection );
  connection.update_database();
  Greeting greeting( attributes
                  ( "salute", "Hello" )
                  ( "thing", "World" )
                  ( "language", "English" ) );
  greeting.save();

  Greeting greeting1( 1 );
  cout << "In " << greeting1[ "language" ] << " you say:" << endl;
  cout << "'" << greeting1[ "salute" ] << " " << greeting1[ "thing" ] << "!'" << endl;

  return 0;
}
```

Installation
============

## Install Dependencies
 - [Boost](http://www.boost.org/)
 - [GoogleTest](http://code.google.com/p/googletest/)
 - [Rake Builder](https://github.com/joeyates/rake-builder)

    gem install rake-builder

## Download

    git clone git://github.com/joeyates/cpp-active-record.git

## Install

    rake build
    sudo rake install

For fuller installation instructions, see the file called INSTALL

Other information
=================

* Usage
    * See the examples directory
* Documentation
    * See the doc directory
* Testing
    * This project uses Google Test.
