#include "test_helper.h"
#include <active_record/table.h>

extern string database_file;

class ConnectionTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    connection.connect( options
                        ( "adapter", "sqlite" )
                        ( "database", database_file ) );
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( ConnectionTest, Connect ) {
  assert_file_exists( database_file );
}

// select_value
// select_values
