#include "test_helper.h"
#include <active_record/query.h>

extern string database_file;
namespace ActiveRecord {
extern Connection connection;
}

class QueryTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( ActiveRecord::connection, database_file );
    Person::setup( &ActiveRecord::connection );
    ActiveRecord::connection.update_database();
    pipe_to_sqlite( database_file, "INSERT INTO people (name, surname, age, height) VALUES (\"Joe\", \"Yates\", 45, 1.80);" );
    pipe_to_sqlite( database_file, "INSERT INTO people (name, surname, age, height) VALUES (\"Joe\", \"Smith\", 45, 1.80);" );
    pipe_to_sqlite( database_file, "INSERT INTO people (name, surname, age, height) VALUES (\"John\", \"Smith\", 67, 1.80);" );
    pipe_to_sqlite( database_file, "INSERT INTO people (name, surname, age, height) VALUES (\"Frank\", \"Smith\", 45, 1.79);" );
  }
  virtual void TearDown() {
    delete_database();
  }
};

TEST_F( QueryTest, All ) {
  Query< Person > query;
  vector< Person > people = query.all();

  ASSERT_EQ( 4, people.size() );
}

TEST_F( QueryTest, First ) {
  Query< Person > query;
  Person person = query.first();

  assert_attribute( "Joe", person[ "name" ] );
}

TEST_F( QueryTest, FirstNoData ) {
  Query< Person > query;

  ASSERT_THROW( query.where( "age = ?", 99 ).first(), ActiveRecordException );
}

TEST_F( QueryTest, Where ) {
  Query< Person > query;
  query.where( "surname = ?", "Smith" );
  vector< Person > people = query.all();

  ASSERT_EQ( 3, people.size() );
}

TEST_F( QueryTest, Limit ) {
  Query< Person > query;
  query.limit( 2 );
  vector< Person > people = query.all();

  ASSERT_EQ( 2, people.size() );
}
