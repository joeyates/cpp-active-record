#include "test_helper.h"

extern string database_file;

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
    connect_database( ActiveRecord::connection, database_file );
    Person::setup( &ActiveRecord::connection );
    ActiveRecord::connection.update_database();
  }
  virtual void TearDown() {
    delete_database();
  }
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

  assert_string( "Joe", joe.text( "name" ) );
  ASSERT_EQ( 45, joe.integer( "age" ) );
  ASSERT_DOUBLE_EQ( 1.80, joe.floating_point( "height" ) );
}

TEST_F( BaseAttributeTest, SettingAttributesUsingAttributesMethod ) {
  Person joe( attributes
              ( "name", "Joe" )
              ( "surname", "Yates" )
              ( "age", 45 )
              ( "height", 1.80 ) );

  assert_string( "Joe", joe.text( "name" ) );
  ASSERT_EQ( 45, joe.integer( "age" ) );
  ASSERT_DOUBLE_EQ( 1.80, joe.floating_point( "height" ) );
}

TEST_F( BaseAttributeTest, SettingAttributesAfterConstruction ) {
  Person joe;
  ASSERT_NO_THROW( joe.init( attributes
                             ( "name", "Joe" )
                             ( "surname", "Yates" )
                             ( "age", 45 )
                             ( "height", 1.80 ) ) );

  assert_string( "Joe", joe.text( "name" ) );
  ASSERT_EQ( 45, joe.integer( "age" ) );
  ASSERT_DOUBLE_EQ( 1.80, joe.floating_point( "height" ) );
}

// TODO: Setting incorrect attributes raises error

class BaseLoadTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    connect_database( ActiveRecord::connection, database_file );
    Person::setup( &ActiveRecord::connection );
    ActiveRecord::connection.update_database();
    pipe_to_sqlite( database_file, "INSERT INTO people (name, surname, age, height) VALUES (\"Joe\", \"Yates\", 45, 1.80);" );
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Connection connection;
};

TEST_F( BaseLoadTest, Default ) {
  Person joe( 1 );

  assert_string( "Joe", joe.text( "name" ) );
  ASSERT_EQ( 45, joe.integer( "age" ) );
  ASSERT_DOUBLE_EQ( 1.80, joe.floating_point( "height" ) );
}

TEST_F( BaseLoadTest, LoadInvalidId ) {
  Person joe( 999 );

  ASSERT_THROW( joe.text( "name" ) , ActiveRecordException );
}

TEST_F( BaseLoadTest, SquareBracketsOperator ) {
  Person joe( 1 );

  assert_attribute( "Joe", joe[ "name" ] );
  assert_attribute( 45,    joe[ "age" ] );
  assert_attribute( 1.80,  joe[ "height" ] );
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
              ( "height", 1.80 ) );

  ASSERT_TRUE( joe.has_data() );
}

class BaseSaveTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( ActiveRecord::connection, database_file );
    Person::setup( &ActiveRecord::connection );
    ActiveRecord::connection.update_database();
  }
  virtual void TearDown() {
    delete_database();
  }
 protected:
  Connection connection;
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
              ( "height", 1.80 ) );
  joe.save();

  Person first( 1 );
  assert_string( "Joe", first.text( "name" ) );
  ASSERT_EQ( 45, joe.integer( "age" ) );
  ASSERT_DOUBLE_EQ( 1.80, joe.floating_point( "height" ) );
}
