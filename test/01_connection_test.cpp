#include "test_helper.h"

extern ActiveRecord::Connection connection;
extern TableSet ActiveRecord::tables;

class ConnectionTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    connect_database();
  }
};

TEST_F( ConnectionTest, Static ) {
  TableData td;
  td.connection  = &connection;
  td.primary_key = "id";
  td.table_name  = "people";
  td.fields.push_back( Field( "name",    ActiveRecord::text ) );
  td.fields.push_back( Field( "surname", ActiveRecord::text ) );

  connection.add_class( "Person" );
  ActiveRecord::tables[ "Person" ] = td;

  connection.update_database();
  // Test if table exists
}
