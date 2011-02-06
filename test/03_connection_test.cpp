#include "test_helper.h"
#include <active_record/table.h>

extern string database_file;

class ConnectionTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    connection.connect( options
                        ( "adapter", "sqlite" )
                        ( "database", database_file ) );
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( ConnectionTest, Connect ) {
  assert_file_exists( database_file );
}

TEST_F( ConnectionTest, TableCreation ) {
  Table td( &connection, "people" );

  connection.create_table( td );

  assert_file_non_zero_length( database_file );
}

// select_value
// select_values

TEST_F( ConnectionTest, UpdateDatabase ) {
  Table td( &connection, "people" );
  td.fields().push_back( Field( "name",    ActiveRecord::text ) );
  td.fields().push_back( Field( "surname", ActiveRecord::text ) );

  ActiveRecord::tables[ "Person" ] = td;

  connection.update_database();

  RowSet rows = connection.select_values( "SELECT sql FROM sqlite_master WHERE type='table';" );
  ASSERT_EQ( 1, rows.size() );
  ASSERT_STREQ( "CREATE TABLE people (id INTEGER PRIMARY KEY, name TEXT, surname TEXT)", rows[ 0 ].get_text( "sql" ).c_str() );
}
