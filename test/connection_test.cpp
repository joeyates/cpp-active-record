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

TEST_F( ConnectionQueryTest, SelectOne ) {
  connect_database( connection, database_file );

  connection.execute( "CREATE TABLE foo (bar INTEGER);" );
  connection.execute( "INSERT INTO foo (bar) VALUES (42);" );

  Row row = connection.select_one( "SELECT bar FROM foo;" );

  ASSERT_EQ( 42, row.get_integer( "bar" ) );
}

TEST_F( ConnectionQueryTest, SelectOneWithParameters ) {
  connect_database( connection, database_file );

  connection.execute( "CREATE TABLE foo (bar INTEGER, baz TEXT);" );
  connection.execute( "INSERT INTO foo (bar, baz) VALUES (42, 'hello');" );
  connection.execute( "INSERT INTO foo (bar, baz) VALUES (13, 'ciao');" );

  AttributeList params;
  params.push_back( Attribute( 13 ) );
  Row row = connection.select_one( "SELECT * FROM foo WHERE bar = ?;", params );

  ASSERT_EQ( "ciao", row.get_text( "baz" ) );
}

TEST_F( ConnectionQueryTest, SelectAll ) {
  connect_database( connection, database_file );

  connection.execute( "CREATE TABLE foo (bar INTEGER);" );
  connection.execute( "INSERT INTO foo (bar) VALUES (42);" );
  connection.execute( "INSERT INTO foo (bar) VALUES (99);" );

  RowSet rows = connection.select_all( "SELECT bar FROM foo ORDER BY bar;" );

  ASSERT_EQ( 42, rows[ 0 ].get_integer( "bar" ) );
  ASSERT_EQ( 99, rows[ 1 ].get_integer( "bar" ) );
}
