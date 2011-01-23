#include "test_helper.h"

extern ActiveRecord::Connection connection;

class AttributeTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    connect_database();
    Person::setup( &connection );
  }
};

TEST_F( AttributeTest, Static ) {
  Person joe;
  joe["name"] = "Joe";
  joe["surname"] = "Yates";
  joe["age"] = 45;
  joe["height"] = 1.80;
  EXPECT_TRUE( joe.save() );
}
