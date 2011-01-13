#include "test_helper.h"

template <>
string ActiveRecord::Base<Person>::class_name = "Person";

ActiveRecord::Connection connection;
string database_file = "./test.sqlite3";

void connect_database() {
  string remove_database = "rm -f " + database_file;
  system( remove_database.c_str() );
  connection.connect( options
                     ( "adapter", "sqlite" )
                     ( "database", database_file ) );
}
