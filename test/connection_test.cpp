#include "test_helper.h"
#include <active_record/exception.h>
#include <active_record/table.h>
#include <active_record/query.h>

extern string database_name;

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
                      ( "database", database_name ) );
  assert_file_exists( database_name );
}

TEST_F( ConnectionTest, ConnectExistingDatabase ) {
  pipe_to_sqlite(database_name, "CREATE TABLE foo (bar INTEGER);");
  Connection connection;
  ASSERT_NO_THROW( {
      connection.connect( options
                          ( "adapter", "sqlite" )
                          ( "database", database_name ) );
  } );
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
  connect_database( connection, database_name );

  connection.execute( "CREATE TABLE foo (bar INTEGER);" );

  assert_table_exists( database_name, "foo" );
}

TEST_F( ConnectionQueryTest, ExecuteBadSQL ) {
  connect_database( connection, database_name );

  ASSERT_THROW( connection.execute( "CREATE THING xxx" ), ActiveRecord::ActiveRecordException );
}

TEST_F( ConnectionQueryTest, SelectOne ) {
  connect_database( connection, database_name );

  connection.execute( "CREATE TABLE foo (bar INTEGER);" );
  connection.execute( "INSERT INTO foo (bar) VALUES (42);" );

  Row row = connection.select_one( "SELECT bar FROM foo;" );

  ASSERT_EQ( 42, row.get_integer( "bar" ) );
}

TEST_F( ConnectionQueryTest, SelectOneNoData ) {
  connect_database( connection, database_name );

  connection.execute( "CREATE TABLE foo (bar INTEGER);" );
  connection.execute( "INSERT INTO foo (bar) VALUES (42);" );

  Row row = connection.select_one( "SELECT * FROM foo WHERE bar = 13;" );

  ASSERT_FALSE( row.has_data() );
}

TEST_F( ConnectionQueryTest, SelectOneWithAttributes ) {
  connect_database( connection, database_name );

  connection.execute( "CREATE TABLE foo (bar INTEGER, baz TEXT);" );
  connection.execute( "INSERT INTO foo (bar, baz) VALUES (42, 'hello');" );
  connection.execute( "INSERT INTO foo (bar, baz) VALUES (13, 'ciao');" );

  AttributeList params;
  params.push_back( Attribute( 13 ) );
  Row row = connection.select_one( "SELECT * FROM foo WHERE bar = ?;", params );

  ASSERT_EQ( "ciao", row.get_text( "baz" ) );
}

TEST_F( ConnectionQueryTest, SelectOneWithParameters ) {
  connect_database( connection, database_name );

  connection.execute( "CREATE TABLE foo (bar INTEGER, baz TEXT);" );
  connection.execute( "INSERT INTO foo (bar, baz) VALUES (42, 'hello');" );
  connection.execute( "INSERT INTO foo (bar, baz) VALUES (13, 'ciao');" );

  Row row = connection.select_one( "SELECT * FROM foo WHERE bar = ?;", parameters ( 13 ) );

  ASSERT_EQ( "ciao", row.get_text( "baz" ) );
}

TEST_F( ConnectionQueryTest, SelectAll ) {
  connect_database( connection, database_name );

  connection.execute( "CREATE TABLE foo (bar INTEGER);" );
  connection.execute( "INSERT INTO foo (bar) VALUES (42);" );
  connection.execute( "INSERT INTO foo (bar) VALUES (99);" );

  RowSet rows = connection.select_all( "SELECT bar FROM foo ORDER BY bar;" );

  RowSet::iterator it = rows.begin();
  ASSERT_EQ( 42, it->get_integer( "bar" ) );
  ++it;
  ASSERT_EQ( 99, it->get_integer( "bar" ) );
}
