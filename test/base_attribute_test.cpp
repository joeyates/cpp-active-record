#include "test_helper.h"

extern string database_file;

class BaseAttributeTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( connection, database_file );
    Person::setup( &connection );
    ActiveRecord::tables.update_database();
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( BaseAttributeTest, Defaults ) {
  Person joe;

  ASSERT_EQ( -1, joe.integer( "id" ) );
}

TEST_F( BaseAttributeTest, SetSingly ) {
  Person joe;
  joe[ "name" ]    = "Joe";
  joe[ "surname" ] = "Yates";
  joe[ "age" ]     = 45;
  joe[ "height" ]  = 1.80;

  ASSERT_TRUE( joe.save() );
  ASSERT_TRUE( joe.integer( "id" ) > 0 );
}

TEST_F( BaseAttributeTest, SetUsingAttributes ) {
  Person joe( attributes
              ( "name", "Joe" )
              ( "surname", "Yates" )
              ( "age", 45 )
              ( "height", 1.80 ) );

  ASSERT_TRUE( joe.save() );
  ASSERT_TRUE( joe.integer( "id" ) > 0 );
}
