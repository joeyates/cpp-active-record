#include "test_helper.h"

extern ActiveRecord::Connection connection;
extern string database_file;

class StructureUpdateTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    string create = "echo 'CREATE TABLE foo (bar INTEGER);' | sqlite3 " + database_file + ";";
    system( create.c_str() );
    connect_database();
  }
  virtual void TearDown() {
    delete_database();
  }
};

TEST_F( StructureUpdateTest, LoadsSchema ) {
  Table td;
  td.connection = &connection;
  td.table_name = "foo";
  td.fields.push_back( Field( "foo", ActiveRecord::integer ) );
  td.fields.push_back( Field( "baz", ActiveRecord::text ) );
  td.fields.push_back( Field( "qux", ActiveRecord::floating_point ) );

  ActiveRecord::tables[ "Foo" ] = td;

  connection.update_database();

  Schema schema   = connection.schema();
  Table foo_table = schema[ "foo" ];
  ASSERT_EQ( 4, foo_table.fields.size() );
  assert_field( foo_table, 1, "foo", ActiveRecord::integer );
  assert_field( foo_table, 2, "baz", ActiveRecord::text );
  assert_field( foo_table, 3, "qux", ActiveRecord::floating_point );
}
