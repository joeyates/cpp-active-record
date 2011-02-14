#include "test_helper.h"
#include <active_record/table.h>

extern string database_file;

class TableSetTest : public ::testing::Test {
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

TEST_F( TableSetTest, TableCreation ) {
  Table td( &connection, "people" );

  TableSet::create_table( td );

  assert_file_non_zero_length( database_file );
}

TEST_F( TableSetTest, UpdateDatabase ) {
  Table td( &connection, "people" );
  td.fields().push_back( Field( "name",    ActiveRecord::text ) );
  td.fields().push_back( Field( "surname", ActiveRecord::text ) );

  TableSet tables;
  tables[ "Person" ] = td;

  tables.update_database();

  RowSet rows = connection.select_all( "SELECT sql FROM sqlite_master WHERE type='table';" );
  ASSERT_EQ( 1, rows.size() );
  string sql = rows[ 0 ].get_text( "sql" );
  assert_string( "CREATE TABLE people (id INTEGER PRIMARY KEY, name TEXT, surname TEXT)", sql );
}

class ReadSchemaTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    string create = "echo 'CREATE TABLE foo (bar INTEGER, baz TEXT, qux FLOAT);' | sqlite3 " + database_file + ";";
    system( create.c_str() );
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

TEST_F( ReadSchemaTest, LoadsSchema ) {
  TableSet schema = TableSet::schema( &connection );
  Table foo_table = schema[ "foo" ];

  ASSERT_EQ( foo_table.fields().size(), 3 );
  assert_field( foo_table, 0, "bar", ActiveRecord::integer );
  assert_field( foo_table, 1, "baz", ActiveRecord::text );
  assert_field( foo_table, 2, "qux", ActiveRecord::floating_point );
}

class TableSetUpdateDatabaseTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    pipe_to_sqlite( database_file, "CREATE TABLE foo (bar INTEGER);" );
    connect_database( connection, database_file );
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( TableSetUpdateDatabaseTest, AddsFields ) {
  Table td( &connection, "foo" );
  td.fields().push_back( Field( "bar", ActiveRecord::integer ) );
  td.fields().push_back( Field( "baz", ActiveRecord::text ) );
  td.fields().push_back( Field( "qux", ActiveRecord::floating_point ) );

  TableSet tables;
  tables[ "Foo" ] = td;
  tables.update_database();

  TableSet schema = TableSet::schema( &connection );
  Table foo_table = schema[ "foo" ];
  ASSERT_EQ( 3, foo_table.fields().size() );
  assert_field( foo_table, 0, "bar", ActiveRecord::integer );
  assert_field( foo_table, 1, "baz", ActiveRecord::text );
  assert_field( foo_table, 2, "qux", ActiveRecord::floating_point );
}

TEST_F( TableSetUpdateDatabaseTest, PrimaryKeyField ) {
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

TEST_F( TableSetUpdateDatabaseTest, Timestamps ) {
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
