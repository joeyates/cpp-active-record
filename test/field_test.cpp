#include "test_helper.h"
#include <active_record/field.h>

using namespace ActiveRecord;

class FieldTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }
};

TEST_F( FieldTest, ReturnsProperties ) {
  Field field( "foo", Type::integer );

  ASSERT_STREQ( "foo", field.name().c_str() );
  ASSERT_EQ( Type::integer, field.type() );
}

TEST_F( FieldTest, SubtractsAccordingToName ) {
  Field a( "a", Type::integer );
  Field b( "b", Type::integer );
  Field c( "c", Type::integer );
  Field d( "d", Type::integer );

  Fields abc;
  abc.push_back( a );
  abc.push_back( b );
  abc.push_back( c );

  Fields cd;
  cd.push_back( c );
  cd.push_back( d );

  Fields ab = abc - cd;

  ASSERT_EQ( 2, ab.size() );
  ASSERT_STREQ( "a", ab[ 0 ].name().c_str() );
  ASSERT_STREQ( "b", ab[ 1 ].name().c_str() );
}
/*
TEST_F( FieldTest, Ostream ) {
  stringstream field_out;
  Field a( "a", Type::integer );

  field_out << a;

  assert_string( "", field_out.str() );
}
*/
class FieldsTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }
};

TEST_F( FieldsTest, DoesNotSubtractAccordingToType ) {
  Field i( "a", Type::integer );
  Field t( "a", Type::text );
  Field f( "a", Type::floating_point );

  Fields it;
  it.push_back( i );
  it.push_back( t );

  Fields tf;
  tf.push_back( t );
  tf.push_back( f );

  Fields result = it - tf;

  ASSERT_EQ( 0, result.size() );
}

TEST_F( FieldsTest, FieldsMethodInCtor ) {
  Fields abc( fields
              ( "a", Type::integer )
              ( "b", Type::text )
              ( "c", Type::floating_point )
              ( "d", Type::date ) );

  ASSERT_EQ( 4, abc.size() );
}

TEST_F( FieldsTest, FieldsMethodInAssignment ) {
  Fields abc = fields
               ( "a", Type::integer )
               ( "b", Type::text )
               ( "c", Type::floating_point )
               ( "d", Type::date );

  ASSERT_EQ( 4, abc.size() );
}
