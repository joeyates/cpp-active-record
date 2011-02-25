#include "test_helper.h"
#include <active_record/table.h>

extern string database_file;
namespace ActiveRecord {
extern Connection connection;
}

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
  td.fields() = fields
                ( "name",    ActiveRecord::text )
                ( "surname", ActiveRecord::text );

  TableSet tables;
  tables[ "Person" ] = td;

  tables.update_database();

  string sql = table_definition( connection, "people" );
  assert_string( "CREATE TABLE people (id INTEGER PRIMARY KEY, name TEXT, surname TEXT)", sql );
}

class ReadSchemaTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    pipe_to_sqlite( database_file, "CREATE TABLE foo (bar INTEGER, baz TEXT, qux FLOAT);" );
    ActiveRecord::connection.connect( options
                                      ( "adapter", "sqlite" )
                                      ( "database", database_file ) );
  }
  virtual void TearDown() {
    delete_database();
  }
};

TEST_F( ReadSchemaTest, LoadsSchema ) {
  TableSet schema = TableSet::schema( &ActiveRecord::connection );
  Table foo_table = schema[ "foo" ];

  ASSERT_EQ( foo_table.fields().size(), 3 );
  assert_field( foo_table, 0, "bar", ActiveRecord::integer );
  assert_field( foo_table, 1, "baz", ActiveRecord::text );
  assert_field( foo_table, 2, "qux", ActiveRecord::floating_point );
}

class TableSetCreateTableTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( ActiveRecord::connection, database_file );
  }
  virtual void TearDown() {
    delete_database();
  }
};

TEST_F( TableSetCreateTableTest, PrimaryKeyField ) {
  Table td( &ActiveRecord::connection, "people" );
  td.primary_key( "hi" );
  td.fields().push_back( Field( "height",  ActiveRecord::floating_point ) );

  TableSet tables;
  tables[ "Person" ] = td;
  tables.update_database();

  string sql = table_definition( connection, "people" );
  assert_string( "CREATE TABLE people (hi INTEGER PRIMARY KEY, height FLOAT)", sql );
}

TEST_F( TableSetCreateTableTest, Timestamps ) {
  Table td( &ActiveRecord::connection, "people" );
  td.timestamps( true );
  td.fields() = fields
                ( "name",    ActiveRecord::text )
                ( "surname", ActiveRecord::text );

  TableSet tables;
  tables[ "Person" ] = td;
  tables.update_database();

  string sql = table_definition( connection, "people" );
  assert_string( "CREATE TABLE people (id INTEGER PRIMARY KEY, name TEXT, surname TEXT, created_at TEXT, updated_at TEXT)", sql );
}

class TableSetUpdateDatabaseTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    pipe_to_sqlite( database_file, "CREATE TABLE foo (bar INTEGER);" );
    connect_database( ActiveRecord::connection, database_file );
  }
  virtual void TearDown() {
    delete_database();
  }
};

TEST_F( TableSetUpdateDatabaseTest, AddsFields ) {
  Table td( &ActiveRecord::connection, "foo" );
  td.fields() = fields
                ( "bar", ActiveRecord::integer )
                ( "baz", ActiveRecord::text )
                ( "qux", ActiveRecord::floating_point );

  TableSet tables;
  tables[ "Foo" ] = td;
  tables.update_database();

  string sql = table_definition( connection, "foo" );
  assert_string( "CREATE TABLE foo (bar INTEGER, baz TEXT, qux FLOAT)", sql );
}
