#include "test_helper.h"
#include <active_record/connection/sqlite3.h>
#include <active_record/table.h>

extern std::string database_name;

namespace ActiveRecord {

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
    sqlite_delete_database();
  }

  protected:

  ActiveRecord::Sqlite3Connection connection;
};

TEST_F(TableSetWithSqliteConnectionTest, TableCreation) {
  ActiveRecord::Table td(&connection, "people");

  ActiveRecord::TableSet::create_table(td);

  assert_file_non_zero_length(database_name);
}

TEST_F(TableSetWithSqliteConnectionTest, UpdateDatabase) {
  ActiveRecord::Table td(&connection, "people");
  td.fields() =
    fields
      ("name",    Type::text)
      ("surname", Type::text);

  ActiveRecord::TableSet tables;
  tables["Person"] = td;

  tables.update_database();

  std::string sql = sqlite_table_definition(connection, "people");
  assert_string(
    "CREATE TABLE people (id INTEGER PRIMARY KEY, name TEXT, surname TEXT)", sql
  );
}

TEST_F(TableSetWithSqliteConnectionTest, PrimaryKeyField) {
  ActiveRecord::Table td(&connection, "people");
  td.primary_key("hi");
  td.fields().push_back(ActiveRecord::Field("height",  Type::floating_point));

  ActiveRecord::TableSet tables;
  tables["Person"] = td;
  tables.update_database();

  std::string sql = sqlite_table_definition(connection, "people");
  assert_string(
    "CREATE TABLE people (hi INTEGER PRIMARY KEY, height FLOAT)", sql
  );
}

TEST_F(TableSetWithSqliteConnectionTest, Timestamps) {
  ActiveRecord::Table td(&connection, "people");
  td.timestamps(true);
  td.fields() =
    fields
      ("name",    Type::text)
      ("surname", Type::text);

  ActiveRecord::TableSet tables;
  tables["Person"] = td;
  tables.update_database();

  std::string sql = sqlite_table_definition(connection, "people");
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

  ActiveRecord::Table td(&connection, "foo");
  td.fields() =
    fields
      ("bar",  Type::integer)
      ("baz",  Type::text)
      ("qux",  Type::floating_point)
      ("derp", Type::date);

  ActiveRecord::TableSet tables;
  tables["Foo"] = td;
  tables.update_database();

  std::string sql = sqlite_table_definition(connection, "foo");
  assert_string(
    "CREATE TABLE foo "
    "(id INTEGER PRIMARY KEY, bar INTEGER, baz TEXT, qux FLOAT, derp DATE)",
    sql
  );
}

TEST_F(TableSetWithSqliteConnectionTest, UpdateTableWithPkeyAddsFields) {
  connection.execute("CREATE TABLE foo (id INTEGER PRIMARY KEY, bar INTEGER);");

  ActiveRecord::TableSet schema = connection.schema();

  ActiveRecord::Table fooUpdate(&connection, "foo");
  fooUpdate.fields().push_back(ActiveRecord::Field("bar", Type::integer));
  fooUpdate.fields().push_back(ActiveRecord::Field("baz", Type::text));

  schema.update_table(fooUpdate);

  std::string sql = sqlite_table_definition(connection, "foo");
  assert_string(
    "CREATE TABLE foo (id INTEGER PRIMARY KEY, bar INTEGER, baz TEXT)", sql
  );
}

TEST_F(TableSetWithSqliteConnectionTest, UpdateTableWithoutPkeyAddsFields) {
  connection.execute("CREATE TABLE foo (bar INTEGER);");

  ActiveRecord::TableSet schema = connection.schema();

  ActiveRecord::Table fooUpdate(&connection, "foo");
  fooUpdate.fields().push_back(ActiveRecord::Field("bar", Type::integer));
  fooUpdate.fields().push_back(ActiveRecord::Field("baz", Type::text));

  schema.update_table(fooUpdate);

  std::string sql = sqlite_table_definition(connection, "foo");
  assert_string("CREATE TABLE foo (bar INTEGER, baz TEXT)", sql);
}

TEST_F(TableSetWithSqliteConnectionTest, UpdateTableWithoutPkeyRemovesFields) {
  connection.execute("CREATE TABLE foo (bar INTEGER, baz TEXT);");

  ActiveRecord::TableSet schema = connection.schema();

  ActiveRecord::Table fooUpdate(&connection, "foo");
  fooUpdate.fields().push_back(ActiveRecord::Field("bar", Type::integer));

  schema.update_table(fooUpdate);

  std::string sql = sqlite_table_definition(connection, "foo");
  assert_string("CREATE TABLE \"foo\" (bar INTEGER)", sql);
}

TEST_F(
  TableSetWithSqliteConnectionTest, WithATableWithTheSameFieldsDoesNothing
) {
  connection.execute("CREATE TABLE foo (bar INTEGER);");

  ActiveRecord::TableSet schema = connection.schema();

  ActiveRecord::Table fooUpdate(&connection, "foo");
  fooUpdate.fields().push_back(ActiveRecord::Field("bar", Type::integer));

  schema.update_table(fooUpdate);

  std::string sql = sqlite_table_definition(connection, "foo");
  assert_string("CREATE TABLE foo (bar INTEGER)", sql);
}

TEST_F(TableSetWithSqliteConnectionTest, UsingTableFromSchemaDoesNothing) {
  connection.execute("CREATE TABLE foo (bar INTEGER);");

  ActiveRecord::TableSet schema = connection.schema();

  ActiveRecord::Table foo = connection.table_data("foo");

  schema.update_table(foo);

  std::string sql = sqlite_table_definition(connection, "foo");
  assert_string("CREATE TABLE foo (bar INTEGER)", sql);
}

} // namespace ActiveRecord
