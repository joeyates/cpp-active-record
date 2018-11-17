#ifndef _POSTGRESQL_CONNECTION_TEST_H_
#define _POSTGRESQL_CONNECTION_TEST_H_

#include <active_record/connection/postgresql.h>

class PostgresqlTest: public ::testing::Test {
  protected:

  virtual void SetUp();

  protected:

  string test_db_;
  PostgresqlConnection connection_;
  OptionsHash connection_options_;
};

#endif // ndef _POSTGRESQL_CONNECTION_TEST_H_
