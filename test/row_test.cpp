#include "test_helper.h"

extern std::string database_name;

namespace ActiveRecord {

class RowTest: public ::testing::Test {
  protected:

  virtual void SetUp() {
    sqlite_delete_database();

    pipe_to_sqlite(
      database_name,
      "CREATE TABLE foo (bar INTEGER, baz TEXT, qux FLOAT, derp DATE);"
    );

    pipe_to_sqlite(
      database_name,
      "INSERT INTO foo "
      "(bar, baz, qux, derp) "
      "VALUES "
      "(123, \"hello\", 1.5, \"1971-07-02\");"
    );

    sqlite3_open(database_name.c_str(), &db);
  }

  virtual void TearDown() {
    sqlite_delete_database();
  }

  protected:

  sqlite3* db;
  sqlite3_stmt* ppStmt;
};

TEST_F(RowTest, Empty) {
  ActiveRecord::Row row;

  ASSERT_FALSE(row.has_data());
}

TEST_F(RowTest, GetType) {
  char query[] = "SELECT * FROM foo";
  int prepare_result = sqlite3_prepare_v2(db, query, strlen(query), &ppStmt, 0);
  sqlite3_step(ppStmt);

  ActiveRecord::Row row(ppStmt);

  ASSERT_EQ(Type::integer,        row.get_type("bar"));
  ASSERT_EQ(Type::text,           row.get_type("baz"));
  ASSERT_EQ(Type::floating_point, row.get_type("qux"));
  ASSERT_EQ(Type::date,           row.get_type("derp"));
}

TEST_F(RowTest, IsType) {
  char query[] = "SELECT * FROM foo";
  int prepare_result = sqlite3_prepare_v2(db, query, strlen(query), &ppStmt, 0);
  sqlite3_step(ppStmt);

  ActiveRecord::Row row(ppStmt);

  ASSERT_TRUE(row.is_type("bar", Type::integer));
  ASSERT_FALSE(row.is_type("bar", Type::text));
}

TEST_F(RowTest, GetInteger) {
  char query[] = "SELECT * FROM foo";
  int prepare_result = sqlite3_prepare_v2(db, query, strlen(query), &ppStmt, 0);
  sqlite3_step(ppStmt);

  ActiveRecord::Row row(ppStmt);

  ASSERT_EQ(123, row.get_integer("bar"));
}

TEST_F(RowTest, GetText) {
  char query[] = "SELECT * FROM foo";
  int prepare_result = sqlite3_prepare_v2(db, query, strlen(query), &ppStmt, 0);
  sqlite3_step(ppStmt);

  ActiveRecord::Row row(ppStmt);

  ASSERT_EQ("hello", row.get_text("baz"));
}

TEST_F(RowTest, GetFloatingPoint) {
  char query[] = "SELECT * FROM foo";
  sqlite3_prepare_v2(db, query, strlen(query), &ppStmt, 0);
  sqlite3_step(ppStmt);

  ActiveRecord::Row row(ppStmt);

  ASSERT_EQ( 1.5, row.get_floating_point("qux"));
}

TEST_F(RowTest, GetDate) {
  char query[] = "SELECT * FROM foo";
  sqlite3_prepare_v2(db, query, strlen(query), &ppStmt, 0);
  sqlite3_step(ppStmt);

  ActiveRecord::Row row(ppStmt);

  assert_attribute(Date(1971, 07, 02), row.get_date("derp"));
}

} // namespace ActiveRecord
