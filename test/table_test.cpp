#include "test_helper.h"

class TableTest : public ::testing::Test {
};

TEST_F( TableTest, Defaults ) {
  Table table;
  ASSERT_EQ( "id",  table.primary_key() );
  ASSERT_EQ( false, table.timestamps() );
}

TEST_F( TableTest, Attributes ) {
  Table table( NULL, "foo" );
  ASSERT_EQ( "id",  table.primary_key() );
  ASSERT_EQ( false, table.timestamps() );
  ASSERT_EQ( "foo",  table.table_name() );
}

TEST_F( TableTest, Fields ) {
  Table table( NULL, "foo" );
  table.fields().push_back( Field( "name", ActiveRecord::text ) );

  ASSERT_EQ( 1, table.fields().size() );
}

TEST_F( TableTest, AddFieldWithoutConnection ) {
  Table table( NULL, "foo" );

  ASSERT_THROW( table.add_field( Field( "bar", ActiveRecord::text ) ), ActiveRecordException );
}
