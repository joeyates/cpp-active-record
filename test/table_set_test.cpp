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

  RowSet rows = connection.select_values( "SELECT sql FROM sqlite_master WHERE type='table';" );
  ASSERT_EQ( 1, rows.size() );
  ASSERT_STREQ( "CREATE TABLE people (id INTEGER PRIMARY KEY, name TEXT, surname TEXT)", rows[ 0 ].get_text( "sql" ).c_str() );
}

class StructureUpdateTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    string create = "echo 'CREATE TABLE foo (bar INTEGER);' | sqlite3 " + database_file + ";";
    system( create.c_str() );
    connect_database();
  }
  virtual void TearDown() {
    delete_database();
  }
};

TEST_F( StructureUpdateTest, AddsFields ) {
  Table td( &connection, "foo" );
  td.fields().push_back( Field( "foo", ActiveRecord::integer ) );
  td.fields().push_back( Field( "baz", ActiveRecord::text ) );
  td.fields().push_back( Field( "qux", ActiveRecord::floating_point ) );

  TableSet tables;
  tables[ "Foo" ] = td;
  tables.update_database();

  TableSet schema = connection.schema();
  Table foo_table = schema[ "foo" ];
  ASSERT_EQ( 4, foo_table.fields().size() );
  assert_field( foo_table, 1, "foo", ActiveRecord::integer );
  assert_field( foo_table, 2, "baz", ActiveRecord::text );
  assert_field( foo_table, 3, "qux", ActiveRecord::floating_point );
}
