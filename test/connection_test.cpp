#include "test_helper.h"
#include <active_record/table.h>

extern string database_file;

class ConnectionTest : public ::testing::Test {
 protected:
  virtual void TearDown() {
    delete_database();
  }
};

TEST_F( ConnectionTest, ConnectNewDatabase ) {
  Connection connection;
  connection.connect( options
                      ( "adapter", "sqlite" )
                      ( "database", database_file ) );
  assert_file_exists( database_file );
}

TEST_F( ConnectionTest, ConnectExistingDatabase ) {
  pipe_to_sqlite( database_file, "CREATE TABLE foo (bar INTEGER);" );
  Connection connection;
  connection.connect( options
                      ( "adapter", "sqlite" )
                      ( "database", database_file ) );

  // TODO: assert no error?
}

class ConnectionQueryTest : public ::testing::Test {
 protected:
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( ConnectionQueryTest, Execute ) {
  connect_database( connection, database_file );

  connection.execute( "CREATE TABLE foo (bar INTEGER);" );

  assert_table_exists( database_file, "foo" );
}

TEST_F( ConnectionQueryTest, SelectValue ) {
  connect_database( connection, database_file );

  connection.execute( "CREATE TABLE foo (bar INTEGER);" );
  connection.execute( "INSERT INTO foo (bar) VALUES (42);" );

  Row row = connection.select_one( "SELECT bar FROM foo;" );
  int bar = row.get_integer( "bar" );
  ASSERT_EQ( 42, bar );
}

// select_one
// select_values
