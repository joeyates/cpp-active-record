#include "test_helper.h"
#include <active_record/connection/sqlite3.h>
#include <active_record/table.h>

extern string database_name;

using namespace ActiveRecord;

class TableSetWithSqliteConnectionTest: public ::testing::Test {
  protected:

  virtual void SetUp() {
    connection.connect(
      options
        ("adapter", "sqlite")
        ("database", database_name)
    );
  }

  virtual void TearDown() {
    connection.disconnect();
    delete_database();
  }

  protected:

  Sqlite3Connection connection;
};

TEST_F(TableSetWithSqliteConnectionTest, TableCreation) {
  Table td(&connection, "people");

  TableSet::create_table(td);

  assert_file_non_zero_length(database_name);
}

TEST_F(TableSetWithSqliteConnectionTest, UpdateDatabase) {
  Table td(&connection, "people");
  td.fields() =
    fields
      ("name",    Type::text)
      ("surname", Type::text);

  TableSet tables;
  tables["Person"] = td;

  tables.update_database();

  string sql = table_definition(connection, "people");
  assert_string(
    "CREATE TABLE people (id INTEGER PRIMARY KEY, name TEXT, surname TEXT)", sql
  );
}

TEST_F(TableSetWithSqliteConnectionTest, PrimaryKeyField) {
  Table td(&connection, "people");
  td.primary_key("hi");
  td.fields().push_back(Field("height",  Type::floating_point));

  TableSet tables;
  tables["Person"] = td;
  tables.update_database();

  string sql = table_definition(connection, "people");
  assert_string(
    "CREATE TABLE people (hi INTEGER PRIMARY KEY, height FLOAT)", sql
  );
}

TEST_F(TableSetWithSqliteConnectionTest, Timestamps) {
  Table td(&connection, "people");
  td.timestamps(true);
  td.fields() =
    fields
      ("name",    Type::text)
      ("surname", Type::text);

  TableSet tables;
  tables["Person"] = td;
  tables.update_database();

  string sql = table_definition(connection, "people");
  assert_string(
    "CREATE TABLE people "
    "("
    "id INTEGER PRIMARY KEY, "
    "name TEXT, "
    "surname TEXT, "
    "created_at TEXT, "
    "updated_at TEXT"
    ")",
    sql
  );
}

TEST_F(TableSetWithSqliteConnectionTest, UpdateDatabaseAddsFields) {
  connection.execute("CREATE TABLE foo (id INTEGER PRIMARY KEY, bar INTEGER);");

  Table td(&connection, "foo");
  td.fields() =
    fields
      ("bar",  Type::integer)
      ("baz",  Type::text)
      ("qux",  Type::floating_point)
      ("derp", Type::date);

  TableSet tables;
  tables["Foo"] = td;
  tables.update_database();

  string sql = table_definition(connection, "foo");
  assert_string(
    "CREATE TABLE foo "
    "(id INTEGER PRIMARY KEY, bar INTEGER, baz TEXT, qux FLOAT, derp DATE)",
    sql
  );
}

TEST_F(TableSetWithSqliteConnectionTest, UpdateTableWithPkeyAddsFields) {
  connection.execute("CREATE TABLE foo (id INTEGER PRIMARY KEY, bar INTEGER);");

  TableSet schema = connection.schema();

  Table fooUpdate(&connection, "foo");
  fooUpdate.fields().push_back(Field("bar", Type::integer));
  fooUpdate.fields().push_back(Field("baz", Type::text));

  schema.update_table(fooUpdate);

  string sql = table_definition(connection, "foo");
  assert_string(
    "CREATE TABLE foo (id INTEGER PRIMARY KEY, bar INTEGER, baz TEXT)", sql
  );
}

TEST_F(TableSetWithSqliteConnectionTest, UpdateTableWithoutPkeyAddsFields) {
  connection.execute("CREATE TABLE foo (bar INTEGER);");

  TableSet schema = connection.schema();

  Table fooUpdate(&connection, "foo");
  fooUpdate.fields().push_back(Field("bar", Type::integer));
  fooUpdate.fields().push_back(Field("baz", Type::text));

  schema.update_table(fooUpdate);

  string sql = table_definition(connection, "foo");
  assert_string("CREATE TABLE foo (bar INTEGER, baz TEXT)", sql);
}

TEST_F(TableSetWithSqliteConnectionTest, UpdateTableWithoutPkeyRemovesFields) {
  connection.execute("CREATE TABLE foo (bar INTEGER, baz TEXT);");

  TableSet schema = connection.schema();

  Table fooUpdate(&connection, "foo");
  fooUpdate.fields().push_back(Field("bar", Type::integer));

  schema.update_table(fooUpdate);

  string sql = table_definition(connection, "foo");
  assert_string("CREATE TABLE \"foo\" (bar INTEGER)", sql);
}

TEST_F(
  TableSetWithSqliteConnectionTest, WithATableWithTheSameFieldsDoesNothing
) {
  connection.execute("CREATE TABLE foo (bar INTEGER);");

  TableSet schema = connection.schema();

  Table fooUpdate(&connection, "foo");
  fooUpdate.fields().push_back(Field("bar", Type::integer));

  schema.update_table(fooUpdate);

  string sql = table_definition(connection, "foo");
  assert_string("CREATE TABLE foo (bar INTEGER)", sql);
}

TEST_F(TableSetWithSqliteConnectionTest, UsingTableFromSchemaDoesNothing) {
  connection.execute("CREATE TABLE foo (bar INTEGER);");

  TableSet schema = connection.schema();

  Table foo = connection.table_data("foo");

  schema.update_table(foo);

  string sql = table_definition(connection, "foo");
  assert_string("CREATE TABLE foo (bar INTEGER)", sql);
}
