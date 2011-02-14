#include "test_helper.h"
#include <list>

class AttributeTest : public ::testing::Test {
};

TEST_F( AttributeTest, GetType ) {
  Attribute foo( 13 );
  Attribute bar( "hello" );
  Attribute baz( 1.8 );

  ASSERT_EQ( ActiveRecord::integer,        get_type( foo ) );
  ASSERT_EQ( ActiveRecord::text,           get_type( bar ) );
  ASSERT_EQ( ActiveRecord::floating_point, get_type( baz ) );
}

TEST_F( AttributeTest, AttributesFunction ) {
  const AttributePairList attribs = attributes
                            ( "foo", 13 )
                            ( "bar", "hello" )
                            ( "baz", 15.5 );

  AttributePairList expected;
  expected.push_back( AttributePair( "foo", 13 ) );
  expected.push_back( AttributePair( "bar", "hello" ) );
  expected.push_back( AttributePair( "baz", 15.5 ) );

  assert_attribute_pair_list( expected, attribs );
}
