#include "test_helper.h"

extern string database_name;

class NoTableNameModel: public ActiveRecord::Base< NoTableNameModel > {
 public:
  AR_CONSTRUCTORS( NoTableNameModel )
  static Table table( Connection * connection ) {
    Table td;
    return td;
  }
};

AR_DECLARE( NoTableNameModel )

class BaseSetupTest : public ::testing::Test {
};

TEST_F( BaseSetupTest, NoTableName ) {
  ASSERT_THROW( NoTableNameModel::setup( NULL ), ActiveRecord::ActiveRecordException );
}

class BaseAttributeTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( connection, database_name );
    Person::setup( &connection );
    connection.update_database();
  }
  virtual void TearDown() {
    delete_database();
  }
 private:
  Sqlite3Connection connection;
};

TEST_F( BaseAttributeTest, Defaults ) {
  Person joe;

  ASSERT_EQ( -1, joe.id() );
}

TEST_F( BaseAttributeTest, SettingAttributesSingly ) {
  Person joe;

  joe[ "name" ]    = "Joe";
  joe[ "surname" ] = "Yates";
  joe[ "age" ]     = 45;
  joe[ "height" ]  = 1.80;
  joe[ "dob" ]     = Date( 1965, 7, 31 );

  assert_string( "Joe",                  joe.text( "name" ) );
  ASSERT_EQ( 45,                         joe.integer( "age" ) );
  ASSERT_DOUBLE_EQ( 1.80,                joe.floating_point( "height" ) );
  assert_attribute( Date( 1965, 7, 31 ), joe.date( "dob" ) );
}

TEST_F( BaseAttributeTest, SettingAttributesUsingAttributesMethod ) {
  Person joe( attributes
              ( "name", "Joe" )
              ( "surname", "Yates" )
              ( "age", 45 )
              ( "height", 1.80 )
              ( "dob", Date( 1965, 7, 31 ) ) );

  assert_string( "Joe",                  joe.text( "name" ) );
  ASSERT_EQ( 45,                         joe.integer( "age" ) );
  ASSERT_DOUBLE_EQ( 1.80,                joe.floating_point( "height" ) );
  assert_attribute( Date( 1965, 7, 31 ), joe.date( "dob" ) );
}

TEST_F( BaseAttributeTest, SettingAttributesAfterConstruction ) {
  Person joe;
  ASSERT_NO_THROW( joe.init( attributes
                             ( "name", "Joe" )
                             ( "surname", "Yates" )
                             ( "age", 45 )
                             ( "height", 1.80 )
                             ( "dob", Date( 1965, 7, 31 ) ) ) );

  assert_string( "Joe",                  joe.text( "name" ) );
  ASSERT_EQ( 45,                         joe.integer( "age" ) );
  ASSERT_DOUBLE_EQ( 1.80,                joe.floating_point( "height" ) );
  assert_attribute( Date( 1965, 7, 31 ), joe.date( "dob" ) );
}

// TODO: Setting incorrect attributes raises error

class BaseLoadTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    connect_database( connection, database_name );
    Person::setup( &connection );
    connection.update_database();
    connection.execute("INSERT INTO people (name, surname, age, height, dob) VALUES (\"Joe\", \"Yates\", 45, 1.80, \"1965-07-31\");" );
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Sqlite3Connection connection;
};

TEST_F( BaseLoadTest, Default ) {
  Person joe( 1 );

  assert_string( "Joe",                  joe.text( "name" ) );
  ASSERT_EQ( 45,                         joe.integer( "age" ) );
  ASSERT_DOUBLE_EQ( 1.80,                joe.floating_point( "height" ) );
  assert_attribute( Date( 1965, 7, 31 ), joe.date( "dob" ) );
}

TEST_F( BaseLoadTest, LoadInvalidId ) {
  Person joe( 999 );

  ASSERT_THROW( joe.text( "name" ) , ActiveRecordException );
}

TEST_F( BaseLoadTest, SquareBracketsOperator ) {
  Person joe( 1 );

  assert_attribute( "Joe",               joe[ "name" ] );
  assert_attribute( 45,                  joe[ "age" ] );
  assert_attribute( 1.80,                joe[ "height" ] );
  assert_attribute( Date( 1965, 7, 31 ), joe[ "dob" ] );
}

TEST_F( BaseLoadTest, HasDataUninitialized ) {
  Person joe;

  ASSERT_FALSE( joe.has_data() );
}

TEST_F( BaseLoadTest, HasDataOnlyId ) {
  Person joe( 1 );

  ASSERT_FALSE( joe.has_data() );
}

TEST_F( BaseLoadTest, HasDataWithAttributes ) {
  Person joe( attributes
              ( "name", "Joe" )
              ( "surname", "Yates" )
              ( "age", 45 )
              ( "height", 1.80 )
              ( "dob", Date( 1965, 7, 31 ) ) );

  ASSERT_TRUE( joe.has_data() );
}

class BaseOperators : public ::testing::Test {
 protected:
  virtual void SetUp() {
    connect_database( connection, database_name );
    Person::setup( &connection );
    connection.update_database();
    connection.execute("INSERT INTO people (name, surname, age, height, dob) VALUES (\"Cherilyn\", \"Sarkisian\", 64, 1.68, \"1946-05-20\");" );
    connection.execute("INSERT INTO people (name, surname, age, height, dob) VALUES (\"Christina\", \"Aguilera\", 30, 1.56, \"1980-12-18\");" );
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Sqlite3Connection connection;
};

TEST_F( BaseOperators, Equality ) {
  Person cher( 1 );
  Person christina( 2 );

  ASSERT_TRUE( cher == cher );
  ASSERT_FALSE( cher == christina );
}

TEST_F( BaseOperators, UpdatedEquality ) {
  Person cher1( 1 );
  Person cher2( 1 );
  cher2[ "name" ] = "cher";

  ASSERT_FALSE( cher1 == cher2 );
}

TEST_F( BaseOperators, ToS ) {
  Person cher( 1 );

  assert_string( "Person: age 64, dob 1946-05-20, height 1.68, id 1, name Cherilyn, surname Sarkisian", cher.to_string() );
}

TEST_F( BaseOperators, StreamOutput ) {
  Person cher( 1 );
  stringstream cher_out;

  cher_out << cher;

  assert_string( "Person: age 64, dob 1946-05-20, height 1.68, id 1, name Cherilyn, surname Sarkisian", cher_out.str() );
}

class BaseSaveTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( connection, database_name );
    Person::setup( &connection );
    connection.update_database();
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Sqlite3Connection connection;
};

TEST_F( BaseSaveTest, Save ) {
  Person joe;
  joe[ "name" ] = "Joe";

  ASSERT_TRUE( joe.save() );
  ASSERT_TRUE( joe.id() > 0 );
}

TEST_F( BaseSaveTest, SaveWithNoData ) {
  Person joe;

  ASSERT_TRUE( joe.save() );
  ASSERT_TRUE( joe.id() > 0 );
}

TEST_F( BaseSaveTest, SavedAttributes ) {
  Person joe( attributes
              ( "name", "Joe" )
              ( "surname", "Yates" )
              ( "age", 45 )
              ( "height", 1.80 )
              ( "dob", Date( 1965, 7, 31 ) ) );
  joe.save();

  Person first( 1 );
  assert_string( "Joe",                  first.text( "name" ) );
  ASSERT_EQ( 45,                         first.integer( "age" ) );
  ASSERT_DOUBLE_EQ( 1.80,                first.floating_point( "height" ) );
  assert_attribute( Date( 1965, 7, 31 ), first.date( "dob" ) );
}
