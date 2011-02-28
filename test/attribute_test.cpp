#include "test_helper.h"

class AttributeTest : public ::testing::Test {
};

TEST_F( AttributeTest, GetType ) {
  Attribute foo( 13 );
  Attribute bar( "hello" );
  Attribute baz( 1.8 );
  Attribute qux( Date( 1999, 8, 29 ) );

  ASSERT_EQ( ActiveRecord::integer,        foo.type() );
  ASSERT_EQ( ActiveRecord::text,           bar.type() );
  ASSERT_EQ( ActiveRecord::floating_point, baz.type() );
  ASSERT_EQ( ActiveRecord::date,           qux.type() );
}

TEST_F( AttributeTest, Equality ) {
  Attribute foo( 13 );

  ASSERT_TRUE( foo == foo );
}

TEST_F( AttributeTest, AttributesFunction ) {
  const AttributePairList attribs = attributes ( "foo", 13 )
                                               ( "bar", "hello" )
                                               ( "baz", 15.5 )
                                               ( "qux", Date( 1999, 8, 29 ) );

  AttributePairList expected;
  expected.push_back( AttributePair( "foo", 13 ) );
  expected.push_back( AttributePair( "bar", "hello" ) );
  expected.push_back( AttributePair( "baz", 15.5 ) );
  expected.push_back( AttributePair( "qux", Date( 1999, 8, 29 ) ) );

  assert_attribute_pair_list( expected, attribs );
}

TEST_F( AttributeTest, Ostream ) {
  Attribute foo( 13 );
  Attribute bar( "hello" );
  Attribute baz( 1.8 );
  Attribute qux( Date( 1999, 8, 29 ) );

  stringstream foo_out, bar_out, baz_out, qux_out;
  foo_out << foo;
  bar_out << bar;
  baz_out << baz;
  qux_out << qux;

  assert_string( "13",         foo_out.str() );
  assert_string( "hello",      bar_out.str() );
  assert_string( "1.8",        baz_out.str() );
  assert_string( "1999-08-29", qux_out.str() );
}
