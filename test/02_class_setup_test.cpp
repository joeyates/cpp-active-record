#include "test_helper.h"

extern ActiveRecord::Connection connection;

class CreateTableTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    connect_database();
  }
};

TEST_F(CreateTableTest, Static) {
  EXPECT_NO_THROW(Person::setup(&connection));
  // Test if table exists
}
