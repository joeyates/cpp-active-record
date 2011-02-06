#include "test_helper.h"

template <>
string ActiveRecord::Base<Person>::class_name = "Person";

ActiveRecord::Connection connection;
string database_file = "./test.sqlite3";

void delete_database() {
  string remove_database = "rm -f " + database_file;
  system( remove_database.c_str() );
}

void connect_database() {
  connection.connect( options
                     ( "adapter", "sqlite" )
                     ( "database", database_file ) );
}

void assert_file_exists( const string &file_name ) {
  struct stat buffer;
  int status = lstat( file_name.c_str(), &buffer);
  ASSERT_EQ( status, 0 );
}

void assert_file_non_zero_length( const string &file_name ) {
  struct stat buffer;
  int status = lstat( file_name.c_str(), &buffer);
  ASSERT_EQ( status, 0 );
  ASSERT_GT( buffer.st_size, 0 );
}

void assert_field_name( Table &td, int field_index, const string &name ) {
  ASSERT_STREQ( td.fields()[ field_index ].name().c_str(), name.c_str() );
}

void assert_field_type( Table &td, int field_index, ActiveRecord::Type type ) {
  ASSERT_EQ( td.fields()[ field_index ].type(), type );
}

void assert_field( Table &td, int field_index, const string &name, ActiveRecord::Type type ) {
  assert_field_name( td, field_index, name );
  assert_field_type( td, field_index, type );
}
