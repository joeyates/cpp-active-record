#include "test_helper.h"
#include <active_record/connection/postgresql.h>

extern string database_name;
extern const char * pg_user;

class PostgresqlTableSetTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    postgresql_shell_create_database(
      "active_record_postgresql_test", "template1", pg_user
    );
    connection.connect(
      options
        ("database", "active_record_postgresql_test")
        ("username", pg_user)
      );
   }
  virtual void TearDown() {
    connection.disconnect();
    postgresql_shell_drop_database(
      "active_record_postgresql_test", "template1", pg_user
    );
  }
 protected:
  PostgresqlConnection connection;
};

TEST_F(PostgresqlTableSetTest, CreateTable) {
  Table td(&connection, "people");

  TableSet::create_table(td);

  // TODO
  //assert_postgresql_table_exists(database_name, "people", pg_user);
}
