#include "test_helper.h"
#include "postgresql_connection_test.h"
#include <active_record/attribute.h>

#include <c.h>
#include <catalog/pg_type.h>
#include <postgresql/libpq-fe.h>

namespace ActiveRecord {

class PostgresqlAttributesTest: public PostgresqlTest {
  protected:

  virtual void SetUp() {
    PostgresqlTest::SetUp();
    created_database_ = "active_record_test_database";
    postgresql_shell_create_database(created_database_, connection_options_);
    connection_options_["database"] = created_database_;

    std::string conninfo = postgresql_conninfo(connection_options_);
    pgconn_ = PQconnectdb(conninfo.c_str());
    if (PQstatus(pgconn_) != CONNECTION_OK) {
      throw "Connection failed";
    }

    std::string create_table =
      "CREATE TABLE foo ("
      "  id      SERIAL, "
      "  i       INTEGER, "
      "  bi      BIGINT, "
      "  cv      CHARACTER VARYING(100), "
      "  t       TEXT, "
      "  fp      NUMERIC, "
      "  d       DATE"
      ")";
    postgresql_shell_command(create_table, connection_options_);

    std::string insert_values =
      "INSERT INTO foo "
      "  (i, bi, cv, t, fp, d) "
      "VALUES "
      "  ( "
      "    42, "
      "    9999999999999999, "
      "    '\\''Hello'\\'', "
      "    '\\''World!'\\'', "
      "    1.99, "
      "    date '\\''2015-10-11'\\''"
      ")";
    postgresql_shell_command(insert_values, connection_options_);

    std::string insert_blanks = "INSERT INTO foo DEFAULT VALUES";
    postgresql_shell_command(insert_blanks, connection_options_);

    std::string query = "SELECT * FROM foo;";
    exec_result_ = PQexec(pgconn_, query.c_str());
  }

  virtual void TearDown() {
    PQclear(exec_result_);
    PQfinish(pgconn_);
    connection_options_["database"] = "template1";
    postgresql_shell_drop_database(created_database_, connection_options_);
  }

  protected:

  std::string created_database_;
  PGconn* pgconn_;
  PGresult* exec_result_;
};

TEST_F(PostgresqlAttributesTest, IntegerFromField) {
  ASSERT_EQ(INT4OID, PQftype(exec_result_, 1));
  Attribute i = Attribute::from_field(exec_result_, 0, 1);
  ASSERT_EQ(Type::integer, i.type());
  ASSERT_EQ(Attribute(42), i);
}

TEST_F(PostgresqlAttributesTest, EmptyFromIntegerField) {
  ASSERT_EQ(INT4OID, PQftype(exec_result_, 1));
  Attribute blank = Attribute::from_field(exec_result_, 1, 1);
  ASSERT_FALSE(blank.has_data());
}

TEST_F(PostgresqlAttributesTest, BigIntegerFromField) {
  ASSERT_EQ(INT8OID, PQftype(exec_result_, 2));
  Attribute bi = Attribute::from_field(exec_result_, 0, 2);
  ASSERT_EQ(Type::integer, bi.type());
  ASSERT_EQ(Attribute(9999999999999999), bi);
}

TEST_F(PostgresqlAttributesTest, TextFromCharacterVaryingField) {
  ASSERT_EQ(VARCHAROID, PQftype(exec_result_, 3));
  Attribute cv = Attribute::from_field(exec_result_, 0, 3);
  ASSERT_EQ(Type::text, cv.type());
  ASSERT_EQ(Attribute("Hello"), cv);
}

TEST_F(PostgresqlAttributesTest, TextFromTextField) {
  ASSERT_EQ(TEXTOID, PQftype(exec_result_, 4));
  Attribute t = Attribute::from_field(exec_result_, 0, 4);
  ASSERT_EQ(Type::text, t.type());
  ASSERT_EQ(Attribute("World!"), t);
}

TEST_F(PostgresqlAttributesTest, EmptyFromTextField) {
  ASSERT_EQ(TEXTOID, PQftype(exec_result_, 4));
  Attribute blank = Attribute::from_field(exec_result_, 1, 4);
  ASSERT_FALSE(blank.has_data());
}

TEST_F(PostgresqlAttributesTest, FloatFromField) {
  ASSERT_EQ(NUMERICOID, PQftype(exec_result_, 5));
  Attribute n = Attribute::from_field(exec_result_, 0, 5);
  ASSERT_EQ(Type::floating_point, n.type());
  ASSERT_EQ(Attribute(1.99), n);
}

TEST_F(PostgresqlAttributesTest, EmptyFromFloatField) {
  ASSERT_EQ(NUMERICOID, PQftype(exec_result_, 5));
  Attribute blank = Attribute::from_field(exec_result_, 1, 5);
  ASSERT_FALSE(blank.has_data());
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

TEST_F(PostgresqlAttributesTest, EmptyFromDateField) {
  ASSERT_EQ(DATEOID, PQftype(exec_result_, 6));
  Attribute blank = Attribute::from_field(exec_result_, 1, 6);
  ASSERT_FALSE(blank.has_data());
}

} // namespace ActiveRecord
