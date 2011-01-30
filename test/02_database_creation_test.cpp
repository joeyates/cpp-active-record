#include "test_helper.h"
#include <sys/stat.h>

extern ActiveRecord::Connection connection;
extern TableSet ActiveRecord::tables;
extern string database_file;

class TableTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database();
  }
  virtual void TearDown() {
    delete_database();
  }
};

TEST_F( TableTest, CreatesDatabase ) {
  Table td;
  td.connection = &connection;
  td.table_name = "people";

  connection.add_class( "Person" );
  ActiveRecord::tables[ "Person" ] = td;

  connection.update_database();

  // Tests: check the db file has been created and in non-zero in size
  struct stat buffer;
  int status = lstat( database_file.c_str(), &buffer);
  ASSERT_EQ( status, 0 );
  ASSERT_GT( buffer.st_size, 0 );
}

TEST_F( TableTest, CreatesFields ) {
  Table td;
  td.connection  = &connection;
  td.table_name  = "people";
  td.fields.push_back( Field( "name",    ActiveRecord::text ) );
  td.fields.push_back( Field( "surname", ActiveRecord::text ) );

  connection.add_class( "Person" );
  ActiveRecord::tables[ "Person" ] = td;

  connection.update_database();

  // Tests: Check for the fields in the schema
  Schema schema      = connection.schema();
  Table people_table = schema[ "people" ];
  ASSERT_EQ( people_table.fields.size(), 3 );
  assert_field( people_table, 0, "id",      ActiveRecord::integer );
  assert_field( people_table, 1, "name",    ActiveRecord::text );
  assert_field( people_table, 2, "surname", ActiveRecord::text );
}

TEST_F( TableTest, PrimaryKeyField ) {
  Table td;
  td.connection  = &connection;
  td.primary_key = "hi";
  td.table_name  = "people";
  td.fields.push_back( Field( "height",  ActiveRecord::floating_point ) );

  connection.add_class( "Person" );
  ActiveRecord::tables[ "Person" ] = td;

  connection.update_database();

  // Tests: Check that we can call the primary key whatever we want
  Schema schema      = connection.schema();
  Table people_table = schema[ "people" ];
  ASSERT_EQ( people_table.fields.size(), 2 );
  assert_field( people_table, 0, "hi",     ActiveRecord::integer );
  assert_field( people_table, 1, "height", ActiveRecord::floating_point );
}

TEST_F( TableTest, Timestamps ) {
  Table td;
  td.connection  = &connection;
  td.timestamps  = true;
  td.table_name  = "people";
  td.fields.push_back( Field( "name",    ActiveRecord::text ) );
  td.fields.push_back( Field( "surname", ActiveRecord::text ) );

  connection.add_class( "Person" );
  ActiveRecord::tables[ "Person" ] = td;

  connection.update_database();

  // Tests: Check for the timestamp fields
  Schema schema      = connection.schema();
  Table people_table = schema[ "people" ];
  ASSERT_EQ( people_table.fields.size(), 5 );
  assert_field( people_table, 0, "id",         ActiveRecord::integer );
  assert_field( people_table, 1, "name",       ActiveRecord::text );
  assert_field( people_table, 2, "surname",    ActiveRecord::text );
  assert_field( people_table, 3, "created_at", ActiveRecord::text );
  assert_field( people_table, 4, "updated_at", ActiveRecord::text );
}
