#include "test_helper.h"
#include <active_record/date.h>

class DateTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }
};

TEST_F( DateTest, ParsesOk ) {
  ASSERT_NO_THROW( { Date::parse( "1964-01-24" ); } );
}

/*
TEST_F( DateTest, CtorDefault ) {
  Date d;

  ASSERT_EQ( 2001, d.year() );
  ASSERT_EQ(    9, d.month() );
  ASSERT_EQ(   11, d.day() );
  ASSERT_THROW
}
*/

TEST_F( DateTest, CtorParts ) {
  Date d( 2001, 9, 11 );

  ASSERT_EQ( 2001, d.year() );
  ASSERT_EQ(    9, d.month() );
  ASSERT_EQ(   11, d.day() );
}

TEST_F( DateTest, CtorParse ) {
  Date d( "1964-01-24" );

  ASSERT_EQ( 1964, d.year() );
  ASSERT_EQ(    1, d.month() );
  ASSERT_EQ(   24, d.day() );
}

TEST_F( DateTest, ToString ) {
  Date d( 2001, 9, 11 );

  assert_string( "2001-09-11", d.to_string() );
}

TEST_F( DateTest, StreamOutput ) {
  Date d( 2001, 9, 11 );
  stringstream date_out;
  date_out << d;

  assert_string( "2001-09-11", date_out.str() );
}

TEST_F( DateTest, Compare ) {
  Date d1( 2001, 9, 11 );
  Date d2( 2001, 9, 11 );
  Date d3( 2099, 7,  3 );

  ASSERT_TRUE( d1 == d2 );
  ASSERT_FALSE( d1 == d3 );
}
