#include "test_helper.h"
#include "postgresql_connection_test.h"
#include <active_record/attribute.h>

#include <c.h>
#include <catalog/pg_type.h>
#include <postgresql/libpq-fe.h>

class PostgresqlAttributesTest: public PostgresqlTest {
  protected:

  virtual void SetUp() {
    PostgresqlTest::SetUp();
    created_database_ = "active_record_test_database";
    postgresql_shell_create_database(created_database_, connection_options_);
    connection_options_["database"] = created_database_;

    string conninfo = postgresql_conninfo(connection_options_);
    pgconn_ = PQconnectdb(conninfo.c_str());
    if (PQstatus(pgconn_) != CONNECTION_OK) {
      throw "Connection failed";
    }

    string create_table =
      "CREATE TABLE foo ("
      "  id      SERIAL, "
      "  i       INTEGER, "
      "  bi      BIGINT, "
      "  n       NUMERIC, "
      "  cv      CHARACTER VARYING(100), "
      "  t       TEXT, "
      "  d       DATE"
      ")";
    postgresql_shell_command(create_table, connection_options_);

    string insert_foo =
      "INSERT INTO foo "
      "  (i, bi, n, cv, t, d) "
      "VALUES "
      "  ( "
      "    42, "
      "    9999999999999999, "
      "    1.99, "
      "    '\\''Hello'\\'', "
      "    '\\''World!'\\'', "
      "    date '\\''2015-10-11'\\''"
      ")";
    postgresql_shell_command(insert_foo, connection_options_);

    string query = "SELECT id, i, bi, n, cv, t, d FROM foo;";
    exec_result_ = PQexec(pgconn_, query.c_str());
  }

  virtual void TearDown() {
    PQclear(exec_result_);
    PQfinish(pgconn_);
    connection_options_["database"] = "template1";
    postgresql_shell_drop_database(created_database_, connection_options_);
  }

  protected:

  string created_database_;
  PGconn* pgconn_;
  PGresult* exec_result_;
};

TEST_F(PostgresqlAttributesTest, IntegerFromField) {
  ASSERT_EQ(INT4OID, PQftype(exec_result_, 1));
  Attribute id = Attribute::from_field(exec_result_, 0, 1);
  ASSERT_EQ(Type::integer, id.type());
  ASSERT_EQ(Attribute(42), id);
}

TEST_F(PostgresqlAttributesTest, BigIntegerFromField) {
  ASSERT_EQ(INT8OID, PQftype(exec_result_, 2));
  Attribute bi = Attribute::from_field(exec_result_, 0, 2);
  ASSERT_EQ(Type::long_long, bi.type());
  ASSERT_EQ(Attribute((long long) 9999999999999999), bi);
}

TEST_F(PostgresqlAttributesTest, FloatFromField) {
  ASSERT_EQ(NUMERICOID, PQftype(exec_result_, 3));
  Attribute n = Attribute::from_field(exec_result_, 0, 3);
  ASSERT_EQ(Type::floating_point, n.type());
  ASSERT_EQ(Attribute(1.99), n);
}

TEST_F(PostgresqlAttributesTest, TextFromCharacterVaryingField) {
  ASSERT_EQ(VARCHAROID, PQftype(exec_result_, 4));
  Attribute cv = Attribute::from_field(exec_result_, 0, 4);
  ASSERT_EQ(Type::text, cv.type());
  ASSERT_EQ(Attribute("Hello"), cv);
}

TEST_F(PostgresqlAttributesTest, TextFromTextField) {
  ASSERT_EQ(TEXTOID, PQftype(exec_result_, 5));
  Attribute t = Attribute::from_field(exec_result_, 0, 5);
  ASSERT_EQ(Type::text, t.type());
  ASSERT_EQ(Attribute("World!"), t);
}

TEST_F(PostgresqlAttributesTest, DateFromField) {
  ASSERT_EQ(DATEOID, PQftype(exec_result_, 6));
  Attribute d = Attribute::from_field(exec_result_, 0, 6);
  ASSERT_EQ(Type::date, d.type());
  Date date = boost::get<Date>(d);
  ASSERT_EQ(11, date.day());
  ASSERT_EQ(10, date.month());
  ASSERT_EQ(2015, date.year());
}
