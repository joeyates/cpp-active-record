#include "test_helper.h"

extern ActiveRecord::Connection connection;
extern string database_file;

class TableDataTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }
};

TEST_F( TableDataTest, Defaults ) {
  Table table;
  ASSERT_EQ( "id",  table.primary_key() );
  ASSERT_EQ( false, table.timestamps() );
}

TEST_F( TableDataTest, Attributes ) {
  Table table( NULL, "foo" );
  ASSERT_EQ( "id",  table.primary_key() );
  ASSERT_EQ( false, table.timestamps() );
  ASSERT_EQ( "foo",  table.table_name() );
}

TEST_F( TableDataTest, Fields ) {
  Table table( NULL, "foo" );
  table.fields().push_back( Field( "name", ActiveRecord::text ) );

  ASSERT_EQ( 1, table.fields().size() );
}
