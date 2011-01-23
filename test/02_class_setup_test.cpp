#include "test_helper.h"

extern ActiveRecord::Connection connection;

class ClassSetupTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    connect_database();
  }
};

TEST_F( ClassSetupTest, Static ) {
  EXPECT_NO_THROW( Person::setup( &connection ) );
  // Test if table exists
}
