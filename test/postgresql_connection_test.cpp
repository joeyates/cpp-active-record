#include "test_helper.h"
#include "postgresql_connection_test.h"

#include <cstdint>

extern const char* pg_host;
extern const char* pg_port;
extern const char* pg_user;

/* N.B. These tests assume that a 'template1' database exists
 *      and that the supplied user can access that database
 *      and can create new databases */

namespace ActiveRecord {

void PostgresqlTest::SetUp() {
  test_db_ = "active_record_postgresql_test";
  if(pg_user) {
    connection_options_["username"] = pg_user;
  } else {
    connection_options_["username"] = "postgres";
  }
  connection_options_["database"] = "template1";
  if(pg_host) {
    connection_options_["host"] = pg_host;
  }
  if(pg_port) {
    connection_options_["port"] = pg_port;
  }
}

TEST_F(PostgresqlTest, ConnectToExistentDatabase) {
  ASSERT_NO_THROW(connection_.connect(connection_options_));
}

TEST_F(PostgresqlTest, ConnectToNonExistentDatabase) {
  connection_options_["database"] = "database_that_does_not_exist";
  ASSERT_THROW(
    connection_.connect(connection_options_),
    ActiveRecord::ActiveRecordException
  );
}

TEST_F(PostgresqlTest, DropDatabase) {
  postgresql_shell_create_database(test_db_, connection_options_);
  ASSERT_TRUE(postgresql_shell_database_exists(test_db_, connection_options_));

  // Connect *after* we've used template1 via command line to avoid
  // "ERROR:  source database template1 is being accessed by other users"
  connection_.connect(connection_options_);

  ASSERT_NO_THROW(
    ActiveRecord::PostgresqlConnection::drop_database(connection_, test_db_)
  );
  ASSERT_FALSE(postgresql_shell_database_exists(test_db_, connection_options_));
}

class PostgresqlWithTemplate1ConnectionTest : public PostgresqlTest {
  protected:

  virtual void SetUp() {
    PostgresqlTest::SetUp();
    connection_.connect(connection_options_);
  }

  virtual void TearDown() {
    connection_.disconnect();
    postgresql_shell_drop_database(test_db_, connection_options_);
  }
};

TEST_F(PostgresqlWithTemplate1ConnectionTest, CreateDatabase) {
  bool created = ActiveRecord::PostgresqlConnection::create_database(
    connection_,
    options
      ("database", test_db_)
      ("owner",    connection_options_["username"])
  );
  ASSERT_TRUE(created);
  ASSERT_TRUE(
    postgresql_shell_database_exists(test_db_, connection_options_)
  );
}

TEST_F(PostgresqlWithTemplate1ConnectionTest, DatabaseExists) {
  ASSERT_TRUE(
      ActiveRecord::PostgresqlConnection::database_exists(
      connection_, connection_options_["database"]
    )
  );
  ASSERT_FALSE(
      ActiveRecord::PostgresqlConnection::database_exists(
      connection_, "database_that_does_not_exist"
    )
  );
}

class PostgresqlWithConnectionTest: public PostgresqlTest {
  protected:

  virtual void SetUp() {
    PostgresqlTest::SetUp();
    postgresql_shell_create_database(test_db_, connection_options_);
    connection_options_["database"] = test_db_;
    connection_.connect(connection_options_);
  }

  virtual void TearDown() {
    connection_.disconnect();
    connection_options_["database"] = "template1";
    postgresql_shell_drop_database(test_db_, connection_options_);
  }
};

TEST_F(PostgresqlWithConnectionTest, TableExists) {
  postgresql_shell_command("CREATE TABLE foo ()", connection_options_);

  ASSERT_TRUE(connection_.table_exists("foo"));
  ASSERT_FALSE(connection_.table_exists("bar"));
}

TEST_F(PostgresqlWithConnectionTest, Execute) {
  connection_.execute("CREATE TABLE bar ()");

  ASSERT_TRUE(connection_.table_exists("bar"));
}

class PostgresqlWithConnectionAndTableTest: public PostgresqlWithConnectionTest
{
  protected:

  virtual void SetUp() {
    PostgresqlWithConnectionTest::SetUp();
    postgresql_shell_command(
      "CREATE TABLE foo ("
      "  pk SERIAL PRIMARY KEY,"
      "  num INTEGER,"
      "  word TEXT,"
      "  fp NUMERIC,"
      "  dt DATE"
      ")",
      connection_options_
    );
  }
};

TEST_F(PostgresqlWithConnectionAndTableTest, Insert) {
  long id = connection_.insert(
    "INSERT INTO foo (num) VALUES (1) RETURNING pk;"
  );

  ASSERT_EQ(id, 1);

  std::list<std::string> output = postgresql_shell_command(
    "SELECT num FROM foo", connection_options_
  );

  ASSERT_EQ(" num \n", output.front());
  output.pop_front();
  output.pop_front();
  ASSERT_EQ("   1\n", output.front());
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectOne) {
  connection_.execute("INSERT INTO foo (num) VALUES (42);");

  Row row = connection_.select_one("SELECT num FROM foo");

  ASSERT_EQ(row.get_integer("num"), 42);
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectOneNoResult) {
  connection_.execute("INSERT INTO foo (num) VALUES (42);");

  Row row = connection_.select_one("SELECT * FROM foo WHERE num < 3");
  ASSERT_FALSE(row.has_data());
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectOneWithIntegerParam) {
  connection_.execute("INSERT INTO foo (num, word) VALUES (42, '42');");
  connection_.execute("INSERT INTO foo (num, word) VALUES (99, '99');");

  Row row = connection_.select_one(
    "SELECT * FROM foo WHERE num = $1", parameters (99)
  );
  assert_string(row.get_text("word"), "99");
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectOneWithTextParam) {
  connection_.execute("INSERT INTO foo (num, word) VALUES (1, 'Hello');");
  connection_.execute("INSERT INTO foo (num, word) VALUES (2, 'World');");

  Row row = connection_.select_one(
    "SELECT * FROM foo WHERE word = $1", parameters ("World")
  );

  ASSERT_TRUE(row.has_data());
  ASSERT_EQ(row.get_integer("num"), 2);
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectOneWithFloatingPointParam) {
  connection_.execute("INSERT INTO foo (fp, word) VALUES (1.5, '1.5');");
  connection_.execute("INSERT INTO foo (fp, word) VALUES (2.5, '2.5');");

  Row row = connection_.select_one(
    "SELECT * FROM foo WHERE fp > $1 AND fp < $2", parameters (2.0) (3.0)
  );

  ASSERT_TRUE(row.has_data());
  assert_string(row.get_text("word"), "2.5");
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectOneWithDateParam) {
  connection_.execute(
    "INSERT INTO foo (dt, word) VALUES ('1999-09-09', '1999');"
  );
  connection_.execute(
   "INSERT INTO foo (dt, word) VALUES ('2001-09-11', '9-11');"
  );

  Row row = connection_.select_one(
    "SELECT * FROM foo WHERE dt = $1", parameters (Date("2001-09-11"))
  );

  ASSERT_TRUE(row.has_data());
  assert_string(row.get_text("word"), "9-11");
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectAll) {
  connection_.execute("INSERT INTO foo (num) VALUES (42);");
  connection_.execute("INSERT INTO foo (num) VALUES (99);");

  ActiveRecord::RowSet rows = connection_.select_all("SELECT num FROM foo");

  ASSERT_EQ(2, rows.size());
  ActiveRecord::RowSet::iterator it = rows.begin();
  ASSERT_EQ(it->get_integer("num"), 42);
  ++it;
  ASSERT_EQ(it->get_integer("num"), 99);
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectAllWithParameter) {
  connection_.execute("INSERT INTO foo (num) VALUES (42);");
  connection_.execute("INSERT INTO foo (num) VALUES (99);");

  ActiveRecord::RowSet rows = connection_.select_all(
    "SELECT num FROM foo WHERE pk = $1", parameters (2)
  );

  ASSERT_EQ(1, rows.size());
  ActiveRecord::RowSet::iterator it = rows.begin();
  ASSERT_EQ(it->get_integer("num"), 99);
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectValue) {
  connection_.execute("INSERT INTO foo (num) VALUES (42);");

  ActiveRecord::Attribute result = connection_.select_value(
    "SELECT num FROM foo"
  );
  ASSERT_TRUE(result.has_data());
  ASSERT_EQ(result.type(), ActiveRecord::Type::integer);
  ASSERT_EQ(boost::get<int64_t>(result), 42);
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectValueWithNoResult) {
  connection_.execute("INSERT INTO foo (num) VALUES (42);");

  ActiveRecord::Attribute result = connection_.select_value(
    "SELECT pk FROM foo WHERE num = 99"
  );
  ASSERT_FALSE(result.has_data());
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectValueWithParam) {
  connection_.execute("INSERT INTO foo (num, fp) VALUES (42, 1.1);");
  connection_.execute("INSERT INTO foo (num, fp) VALUES (113, 2.2);");

  ActiveRecord::Attribute result = connection_.select_value(
    "SELECT num FROM foo WHERE fp > $1", parameters (2.0)
  );

  ASSERT_TRUE(result.has_data());
  ASSERT_EQ(result.type(), ActiveRecord::Type::integer);
  ASSERT_EQ(boost::get<int64_t>(result), 113);
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectValues) {
  connection_.execute("INSERT INTO foo (num) VALUES (42);");
  connection_.execute("INSERT INTO foo (num) VALUES (99);");

  AttributeList attributes = connection_.select_values("SELECT num FROM foo");

  ASSERT_EQ(2, attributes.size());
  AttributeList::iterator it = attributes.begin();
  assert_attribute(*it, 42);
  ++it;
  assert_attribute(*it, 99);
}

TEST_F(PostgresqlWithConnectionAndTableTest, SelectValuesWithParameter) {
  connection_.execute("INSERT INTO foo (num) VALUES (42);");
  connection_.execute("INSERT INTO foo (num) VALUES (99);");

  AttributeList attributes = connection_.select_values(
    "SELECT num FROM foo WHERE pk>= $1", parameters (2)
  );

  ASSERT_EQ(1, attributes.size());
  AttributeList::iterator it = attributes.begin();
  assert_attribute(*it, 99);
}

TEST_F(PostgresqlWithConnectionAndTableTest, TableData) {
  Table t = connection_.table_data("foo");

  ASSERT_EQ(t.primary_key(), "pk");
  ASSERT_EQ(t.fields().size(), 4);
  assert_field(t, 0, "num", Type::integer);
  assert_field(t, 1, "word", Type::text);
  assert_field(t, 2, "fp", Type::floating_point);
  assert_field(t, 3, "dt", Type::date);
}

TEST_F(PostgresqlWithConnectionAndTableTest, Schema) {
  TableSet t = connection_.schema();

  ASSERT_EQ(1, t.size());
  TableSet::const_iterator it = t.begin();
  ASSERT_EQ(it->first, "foo");
}

TEST_F(PostgresqlWithConnectionAndTableTest, FindsPrimaryKey) {
  ASSERT_EQ(connection_.primary_key("foo"), "pk");
}

TEST_F(PostgresqlWithConnectionAndTableTest, RemovesField) {
  connection_.remove_field("foo", "num");

  Table t = connection_.table_data("foo");

  ASSERT_EQ(t.primary_key(), "pk");
  ASSERT_EQ(t.fields().size(), 3);
  assert_field(t, 0, "word", Type::text);
  assert_field(t, 1, "fp", Type::floating_point);
  assert_field(t, 2, "dt", Type::date);
}

} // namespace ActiveRecord
