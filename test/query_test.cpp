#include "test_helper.h"
#include <active_record/connection/sqlite3.h>
#include <active_record/query.h>

extern string database_name;

class QueryTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( connection, database_name );
    Person::setup( &connection );
    connection.update_database();
    connection.execute("INSERT INTO people (name, surname, age, height) VALUES (\"Joe\", \"Yates\", 45, 1.80);");
    connection.execute("INSERT INTO people (name, surname, age, height) VALUES (\"Joe\", \"Smith\", 45, 1.80);");
    connection.execute("INSERT INTO people (name, surname, age, height) VALUES (\"John\", \"Smith\", 67, 1.80);");
    connection.execute("INSERT INTO people (name, surname, age, height) VALUES (\"Frank\", \"Smith\", 45, 1.79);");
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Sqlite3Connection connection;
};

TEST_F( QueryTest, All ) {
  Query< Person > query( connection );
  vector< Person > people = query.all();

  ASSERT_EQ( 4, people.size() );
}

TEST_F( QueryTest, First ) {
  Query< Person > query( connection );
  Person person = query.first();

  assert_attribute( "Joe", person[ "name" ] );
}

TEST_F( QueryTest, FirstNoData ) {
  Query< Person > query( connection );

  ASSERT_THROW( query.where( "age = ?", 99 ).first(), ActiveRecordException );
}

TEST_F( QueryTest, Where ) {
  Query< Person > query( connection );
  query.where( "surname = ?", "Smith" );
  vector< Person > people = query.all();

  ASSERT_EQ( 3, people.size() );
}

TEST_F( QueryTest, Limit ) {
  Query< Person > query( connection );
  query.limit( 2 );
  vector< Person > people = query.all();

  ASSERT_EQ( 2, people.size() );
}
