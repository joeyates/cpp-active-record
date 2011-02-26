#include "test_helper.h"

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
  const AttributePairList attribs = attributes ( "foo", 13 )
                                               ( "bar", "hello" )
                                               ( "baz", 15.5 );

  AttributePairList expected;
  expected.push_back( AttributePair( "foo", 13 ) );
  expected.push_back( AttributePair( "bar", "hello" ) );
  expected.push_back( AttributePair( "baz", 15.5 ) );

  assert_attribute_pair_list( expected, attribs );
}

TEST_F( AttributeTest, Ostream ) {
  Attribute foo( 13 );
  Attribute bar( "hello" );
  Attribute baz( 1.8 );

  stringstream foo_out, bar_out, baz_out;
  foo_out << foo;
  bar_out << bar;
  baz_out << baz;

  assert_string( "13",    foo_out.str() );
  assert_string( "hello", bar_out.str() );
  assert_string( "1.8",   baz_out.str() );
}
