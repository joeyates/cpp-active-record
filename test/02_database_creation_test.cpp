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

TEST_F( TableTest, PrimaryKeyField ) {
  Table td( &connection, "people" );
  td.primary_key( "hi" );
  td.fields().push_back( Field( "height",  ActiveRecord::floating_point ) );

  ActiveRecord::tables[ "Person" ] = td;

  ActiveRecord::tables.update_database();

  // Tests: Check that we can call the primary key whatever we want
  TableSet schema    = TableSet::schema( &connection );
  Table people_table = schema[ "people" ];
  ASSERT_EQ( people_table.fields().size(), 2 );
  assert_field( people_table, 0, "hi",     ActiveRecord::integer );
  assert_field( people_table, 1, "height", ActiveRecord::floating_point );
}

TEST_F( TableTest, Timestamps ) {
  Table td( &connection, "people" );
  td.timestamps( true );
  td.fields().push_back( Field( "name",    ActiveRecord::text ) );
  td.fields().push_back( Field( "surname", ActiveRecord::text ) );

  ActiveRecord::tables[ "Person" ] = td;

  ActiveRecord::tables.update_database();

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
