#ifndef _POSTGRESQL_CONNECTION_TEST_H_
#define _POSTGRESQL_CONNECTION_TEST_H_

#include <active_record/connection/postgresql.h>

namespace ActiveRecord {

class PostgresqlTest: public ::testing::Test {
  protected:

  virtual void SetUp();

  protected:

  std::string test_db_;
  PostgresqlConnection connection_;
  OptionsHash connection_options_;
};

} // namespace ActiveRecord

#endif // ndef _POSTGRESQL_CONNECTION_TEST_H_
