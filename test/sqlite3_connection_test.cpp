#include "test_helper.h"
#include <active_record/connection/sqlite3.h>
#include <active_record/exception.h>
#include <active_record/table.h>
#include <active_record/query.h>

extern std::string database_name;

namespace ActiveRecord {

class ConnectionTest: public ::testing::Test {
  protected:

  virtual void TearDown() {
    sqlite_delete_database();
  }
};

TEST_F(ConnectionTest, ConnectNewDatabase) {
  Sqlite3Connection connection;
  connection.connect(
    options
      ("adapter", "sqlite")
      ("database", database_name)
  );

  assert_file_exists(database_name);
}

TEST_F(ConnectionTest, ConnectExistingDatabase) {
  pipe_to_sqlite(database_name, "CREATE TABLE foo (bar INTEGER);");
  Sqlite3Connection connection;
  ASSERT_NO_THROW({
    connection.connect(
      options
        ("adapter", "sqlite")
        ("database", database_name)
    );
  });
}

class ConnectionQueryTest: public ::testing::Test {
  protected:

  virtual void TearDown() {
    sqlite_delete_database();
  }

  protected:

  Sqlite3Connection connection;
};

TEST_F(ConnectionQueryTest, Execute) {
  sqlite_connect_database(connection, database_name);

  connection.execute("CREATE TABLE foo (bar INTEGER);");

  sqlite_assert_table_exists(database_name, "foo");
}

TEST_F(ConnectionQueryTest, ExecuteBadSQL) {
  sqlite_connect_database(connection, database_name);

  ASSERT_THROW(
    connection.execute("CREATE THING xxx"), ActiveRecord::ActiveRecordException
  );
}

TEST_F(ConnectionQueryTest, SelectOne) {
  sqlite_connect_database(connection, database_name);

  connection.execute("CREATE TABLE foo (bar INTEGER);");
  connection.execute("INSERT INTO foo (bar) VALUES (42);");

  Row row = connection.select_one("SELECT bar FROM foo;");

  ASSERT_EQ( 42, row.get_integer("bar"));
}

TEST_F(ConnectionQueryTest, SelectOneNoData) {
  sqlite_connect_database(connection, database_name);

  connection.execute("CREATE TABLE foo (bar INTEGER);");
  connection.execute("INSERT INTO foo (bar) VALUES (42);");

  Row row = connection.select_one("SELECT * FROM foo WHERE bar = 13;");

  ASSERT_FALSE(row.has_data());
}

TEST_F(ConnectionQueryTest, SelectOneWithAttributes) {
  sqlite_connect_database(connection, database_name);

  connection.execute("CREATE TABLE foo (bar INTEGER, baz TEXT);");
  connection.execute("INSERT INTO foo (bar, baz) VALUES (42, 'hello');");
  connection.execute("INSERT INTO foo (bar, baz) VALUES (13, 'ciao');");

  AttributeList params = parameters (13);
  Row row = connection.select_one("SELECT * FROM foo WHERE bar = ?;", params);

  ASSERT_EQ("ciao", row.get_text("baz"));
}

TEST_F(ConnectionQueryTest, SelectOneWithParameters) {
  sqlite_connect_database(connection, database_name);

  connection.execute("CREATE TABLE foo (bar INTEGER, baz TEXT);");
  connection.execute("INSERT INTO foo (bar, baz) VALUES (42, 'hello');");
  connection.execute("INSERT INTO foo (bar, baz) VALUES (13, 'ciao');");

  Row row = connection.select_one(
    "SELECT * FROM foo WHERE bar = ?;", parameters (13)
  );

  ASSERT_EQ("ciao", row.get_text("baz"));
}

TEST_F(ConnectionQueryTest, SelectAll) {
  sqlite_connect_database(connection, database_name);

  connection.execute("CREATE TABLE foo (bar INTEGER);");
  connection.execute("INSERT INTO foo (bar) VALUES (42);");
  connection.execute("INSERT INTO foo (bar) VALUES (99);");

  RowSet rows = connection.select_all("SELECT bar FROM foo ORDER BY bar;");

  RowSet::iterator it = rows.begin();
  ASSERT_EQ(42, it->get_integer("bar"));
  ++it;
  ASSERT_EQ(99, it->get_integer("bar"));
}

class Sqlite3WithConnectionTest : public ::testing::Test {
  protected:

  virtual void SetUp() {
    sqlite_delete_database();
    sqlite_connect_database(connection, database_name);
    std::string create =
      "CREATE TABLE foo "
      "("
      "pk INTEGER PRIMARY KEY, "
      "bar INTEGER, "
      "baz TEXT, "
      "qux FLOAT, "
      "derp DATE"
      ");";
    connection.execute(create);
  }

  virtual void TearDown() {
    connection.disconnect();
    sqlite_delete_database();
  }

  protected:

  Sqlite3Connection connection;
};

TEST_F(Sqlite3WithConnectionTest, TableDataLoadsFieldsAndSkipsPkey) {
  Table foo_table = connection.table_data("foo");

  ASSERT_EQ(foo_table.fields().size(), 4);
  assert_field(foo_table, 0, "bar",  Type::integer);
  assert_field(foo_table, 1, "baz",  Type::text);
  assert_field(foo_table, 2, "qux",  Type::floating_point);
  assert_field(foo_table, 3, "derp", Type::date);
}

TEST_F(Sqlite3WithConnectionTest, RemoveField) {
  connection.remove_field("foo", "bar");

  Table foo_table = connection.table_data("foo");

  ASSERT_EQ("pk", foo_table.primary_key());
  ASSERT_EQ(foo_table.fields().size(), 3);
  assert_field(foo_table, 0, "baz",  Type::text);
  assert_field(foo_table, 1, "qux",  Type::floating_point);
  assert_field(foo_table, 2, "derp", Type::date);
}

TEST_F(Sqlite3WithConnectionTest, FindsPrimaryKey) {
  ASSERT_EQ(connection.primary_key("foo"), "pk");
}

} // namespace ActiveRecord
