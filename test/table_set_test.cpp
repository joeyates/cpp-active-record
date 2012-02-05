#include "test_helper.h"
#include <active_record/table.h>

extern string database_name;

class TableSetTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    connection.connect( options
                        ( "adapter", "sqlite" )
                        ( "database", database_name ) );
  }
  virtual void TearDown() {
    connection.disconnect();
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( TableSetTest, TableCreation ) {
  Table td( &connection, "people" );

  TableSet::create_table( td );

  assert_file_non_zero_length( database_name );
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
    connect_database(connection, database_file);
    connection.execute("CREATE TABLE foo (bar INTEGER, baz TEXT, qux FLOAT, derp DATE);");
  }
  virtual void TearDown() {
    connection.disconnect();
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( ReadSchemaTest, LoadsSchema ) {
  TableSet schema = TableSet::schema(&connection);
  Table foo_table = schema[ "foo" ];

  ASSERT_EQ( foo_table.fields().size(), 4 );
  assert_field( foo_table, 0, "bar",  ActiveRecord::integer );
  assert_field( foo_table, 1, "baz",  ActiveRecord::text );
  assert_field( foo_table, 2, "qux",  ActiveRecord::floating_point );
  assert_field( foo_table, 3, "derp", ActiveRecord::date );
}

class TableSetCreateTableTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( connection, database_name );
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( TableSetCreateTableTest, PrimaryKeyField ) {
  Table td( &connection, "people" );
  td.primary_key( "hi" );
  td.fields().push_back( Field( "height",  ActiveRecord::floating_point ) );

  TableSet tables;
  tables[ "Person" ] = td;
  tables.update_database();

  string sql = table_definition( connection, "people" );
  assert_string( "CREATE TABLE people (hi INTEGER PRIMARY KEY, height FLOAT)", sql );
}

TEST_F( TableSetCreateTableTest, Timestamps ) {
  Table td( &connection, "people" );
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
    connect_database( connection, database_file );
    connection.execute("CREATE TABLE foo (bar INTEGER);");
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( TableSetUpdateDatabaseTest, AddsFields ) {
  Table td( &connection, "foo" );
  td.fields() = fields
                ( "bar",  ActiveRecord::integer )
                ( "baz",  ActiveRecord::text )
                ( "qux",  ActiveRecord::floating_point )
                ( "derp", ActiveRecord::date );

  TableSet tables;
  tables[ "Foo" ] = td;
  tables.update_database();

  string sql = table_definition( connection, "foo" );
  assert_string( "CREATE TABLE foo (bar INTEGER, baz TEXT, qux FLOAT, derp DATE)", sql );
}

class TableSetUpdateTableTestWithPrivateKey : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( connection, database_file );
    connection.execute("CREATE TABLE foo (id INTEGER PRIMARY KEY, bar INTEGER);");
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( TableSetUpdateTableTestWithPrivateKey, AddsFields ) {
  TableSet schema = TableSet::schema(&connection);

  Table fooUpdate(&connection, "foo");
  fooUpdate.fields().push_back( Field( "bar", ActiveRecord::integer ) );
  fooUpdate.fields().push_back( Field( "baz", ActiveRecord::text ) );

  schema.update_table(fooUpdate);

  string sql = table_definition( connection, "foo" );
  assert_string( "CREATE TABLE foo (id INTEGER PRIMARY KEY, bar INTEGER, baz TEXT)", sql );
}

class TableSetUpdateTableTestWithoutPrivateKey : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( connection, database_file );
    connection.execute("CREATE TABLE foo (bar INTEGER);");
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( TableSetUpdateTableTestWithoutPrivateKey, AddsFields ) {
  TableSet schema = TableSet::schema(&connection);

  Table fooUpdate(&connection, "foo");
  fooUpdate.fields().push_back( Field( "bar", ActiveRecord::integer ) );
  fooUpdate.fields().push_back( Field( "baz", ActiveRecord::text ) );

  schema.update_table(fooUpdate);

  string sql = table_definition( connection, "foo" );
  assert_string( "CREATE TABLE foo (bar INTEGER, baz TEXT)", sql );
}

TEST_F( TableSetUpdateTableTestWithoutPrivateKey, RemovingFieldsInSqliteFails ) {
  TableSet schema = TableSet::schema(&connection);

  Table fooUpdate(&connection, "foo");

  ASSERT_THROW( schema.update_table(fooUpdate), ActiveRecordException );
}

TEST_F( TableSetUpdateTableTestWithoutPrivateKey, WithATableWithTheSameFieldsDoesNothing ) {
  TableSet schema = TableSet::schema(&connection);

  Table fooUpdate(&connection, "foo");
  fooUpdate.fields().push_back( Field( "bar", ActiveRecord::integer ) );

  schema.update_table(fooUpdate);

  string sql = table_definition( connection, "foo" );
  assert_string( "CREATE TABLE foo (bar INTEGER)", sql );
}

TEST_F( TableSetUpdateTableTestWithoutPrivateKey, UsingTableFromSchemaDoesNothing ) {
  TableSet schema = TableSet::schema(&connection);
  Table foo = TableSet::table_data(&connection, "foo");

  schema.update_table(foo);

  string sql = table_definition( connection, "foo" );
  assert_string( "CREATE TABLE foo (bar INTEGER)", sql );
}
