#include "test_helper.h"
#include <sys/stat.h>
#include <active_record/field.h>

extern string database_file;

class DatabaseCreationTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( connection, database_file );
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( DatabaseCreationTest, PrimaryKeyField ) {
  Table td( &connection, "people" );
  td.primary_key( "hi" );
  td.fields().push_back( Field( "height",  ActiveRecord::floating_point ) );

  TableSet tables;
  tables[ "Person" ] = td;
  tables.update_database();

  // Tests: Check that we can call the primary key whatever we want
  TableSet schema    = TableSet::schema( &connection );
  Table people_table = schema[ "people" ];
  ASSERT_EQ( people_table.fields().size(), 2 );
  assert_field( people_table, 0, "hi",     ActiveRecord::integer );
  assert_field( people_table, 1, "height", ActiveRecord::floating_point );
}

TEST_F( DatabaseCreationTest, Timestamps ) {
  Table td( &connection, "people" );
  td.timestamps( true );
  td.fields().push_back( Field( "name",    ActiveRecord::text ) );
  td.fields().push_back( Field( "surname", ActiveRecord::text ) );

  TableSet tables;
  tables[ "Person" ] = td;
  tables.update_database();

  // Tests: Check for the timestamp fields
  TableSet schema    = TableSet::schema( &connection );
  Table people_table = schema[ "people" ];
  ASSERT_EQ( people_table.fields().size(), 5 );
  assert_field( people_table, 0, "id",         ActiveRecord::integer );
  assert_field( people_table, 1, "name",       ActiveRecord::text );
  assert_field( people_table, 2, "surname",    ActiveRecord::text );
  assert_field( people_table, 3, "created_at", ActiveRecord::text );
  assert_field( people_table, 4, "updated_at", ActiveRecord::text );
}

TEST_F( DatabaseCreationTest, FieldsAssignmentList ) {
  Table td( &connection, "people" );
  td.fields( fields
             ( "a", ActiveRecord::integer )
             ( "b", ActiveRecord::text )
             ( "c", ActiveRecord::floating_point ) );

  ASSERT_EQ( 3, td.fields().size() );
}
