#include "test_helper.h"
#include <sys/stat.h>

extern ActiveRecord::Connection connection;
extern TableSet ActiveRecord::tables;
extern string database_file;

class TableDataTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    connect_database();
  }
};

TEST_F( TableDataTest, CreatesDatabase ) {
  TableData td;
  td.connection  = &connection;
  td.table_name  = "people";

  connection.add_class( "Person" );
  ActiveRecord::tables[ "Person" ] = td;

  connection.update_database();

  // Tests: check the db file has been created and in non-zero in size
  struct stat buffer;
  int status = lstat( database_file.c_str(), &buffer);
  ASSERT_EQ( status, 0 );
  ASSERT_GT( buffer.st_size, 0 );
}

TEST_F( TableDataTest, CreatesFields ) {
  TableData td;
  td.connection  = &connection;
  td.table_name  = "people";
  td.fields.push_back( Field( "name",    ActiveRecord::text ) );
  td.fields.push_back( Field( "surname", ActiveRecord::text ) );

  connection.add_class( "Person" );
  ActiveRecord::tables[ "Person" ] = td;

  connection.update_database();

  // Tests: Check for the fields in the schema
  Schema schema          = connection.schema();
  TableData people_table = schema[ "people" ];
  ASSERT_EQ( people_table.fields.size(), 3 );
  ASSERT_STREQ( people_table.fields[ 0 ].name().c_str(), "id" );
  ASSERT_STREQ( people_table.fields[ 1 ].name().c_str(), "name" );
  ASSERT_STREQ( people_table.fields[ 2 ].name().c_str(), "surname" );
}

TEST_F( TableDataTest, PrimaryKeyField ) {
  TableData td;
  td.connection  = &connection;
  td.primary_key = "hi";
  td.table_name  = "people";
  td.fields.push_back( Field( "name",    ActiveRecord::text ) );
  td.fields.push_back( Field( "surname", ActiveRecord::text ) );

  connection.add_class( "Person" );
  ActiveRecord::tables[ "Person" ] = td;

  connection.update_database();

  // Tests: Check that we can call the primary key whatever we want
  Schema schema          = connection.schema();
  TableData people_table = schema[ "people" ];
  ASSERT_EQ( people_table.fields.size(), 3 );
  ASSERT_STREQ( people_table.fields[ 0 ].name().c_str(), "hi" );
  ASSERT_STREQ( people_table.fields[ 1 ].name().c_str(), "name" );
  ASSERT_STREQ( people_table.fields[ 2 ].name().c_str(), "surname" );
}

TEST_F( TableDataTest, Timestamps ) {
  TableData td;
  td.connection  = &connection;
  td.timestamps  = true;
  td.table_name  = "people";
  td.fields.push_back( Field( "name",    ActiveRecord::text ) );
  td.fields.push_back( Field( "surname", ActiveRecord::text ) );

  connection.add_class( "Person" );
  ActiveRecord::tables[ "Person" ] = td;

  connection.update_database();

  // Tests: Check for the timestamp fields
  Schema schema          = connection.schema();
  TableData people_table = schema[ "people" ];
  ASSERT_EQ( people_table.fields.size(), 5 );
  ASSERT_STREQ( people_table.fields[ 0 ].name().c_str(), "id" );
  ASSERT_STREQ( people_table.fields[ 1 ].name().c_str(), "name" );
  ASSERT_STREQ( people_table.fields[ 2 ].name().c_str(), "surname" );
  ASSERT_STREQ( people_table.fields[ 3 ].name().c_str(), "created_at" );
  ASSERT_STREQ( people_table.fields[ 4 ].name().c_str(), "updated_at" );
}
