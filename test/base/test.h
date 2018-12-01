#include "../test_helper.h"
#include <active_record/connection/sqlite3.h>

extern string database_name;

class BaseTest: public ::testing::Test {
  protected:

  virtual void SetUp() {
    sqlite_connect_database(connection_, database_name);
    Person::setup(&connection_);
    connection_.update_database();
  }

  virtual void TearDown() {
    sqlite_delete_database();
  }

  protected:

  Sqlite3Connection connection_;

  void add_cher() {
    connection_.execute(
      "INSERT INTO people"
      "  (name, surname, age, height, dob) "
      "VALUES"
      "  (\"Cherilyn\", \"Sarkisian\", 64, 1.68, \"1946-05-20\");"
    );
  }

  void add_christina() {
    connection_.execute(
      "INSERT INTO people"
      "  (name, surname, age, height, dob) "
      "VALUES"
      "  (\"Christina\", \"Aguilera\", 30, 1.56, \"1980-12-18\");"
    );
  }
};
