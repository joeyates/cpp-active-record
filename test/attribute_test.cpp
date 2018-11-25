#include "test_helper.h"

class AttributeTest: public ::testing::Test {};

using namespace ActiveRecord;

TEST_F(AttributeTest, GetType) {
  Attribute foo((int64) 13);
  Attribute bar("hello");
  Attribute baz(1.8);
  Attribute qux(Date(1999, 8, 29));

  ASSERT_EQ(Type::integer,        foo.type());
  ASSERT_EQ(Type::text,           bar.type());
  ASSERT_EQ(Type::floating_point, baz.type());
  ASSERT_EQ(Type::date,           qux.type());
}

TEST_F(AttributeTest, HasData) {
  Attribute foo((int64) 13);
  Attribute def;

  ASSERT_TRUE(foo.has_data());
  ASSERT_FALSE(def.has_data());
}

TEST_F(AttributeTest, Equality) {
  Attribute foo((int64) 13);
  Attribute bar("hello");
  Attribute baz(1.8);
  Attribute qux(Date(1999, 8, 29));

  ASSERT_TRUE(foo == foo);
  ASSERT_FALSE(foo == bar);
}

TEST_F(AttributeTest, Ostream) {
  Attribute foo((int64) 13);
  Attribute bar("hello");
  Attribute baz(1.8);
  Attribute qux(Date(1999, 8, 29));

  stringstream foo_out, bar_out, baz_out, qux_out;
  foo_out << foo;
  bar_out << bar;
  baz_out << baz;
  qux_out << qux;

  assert_string("13",         foo_out.str());
  assert_string("hello",      bar_out.str());
  assert_string("1.8",        baz_out.str());
  assert_string("1999-08-29", qux_out.str());
}

class AttributeHashTest: public ::testing::Test {};

TEST_F(AttributeHashTest, AttributesFunction) {
  const AttributePairList attribs =
    attribute_pairs
      ("foo", (int64) 13)
      ("bar", "hello")
      ("baz", 15.5)
      ("qux", Date(1999, 8, 29));

  ASSERT_EQ(4, attribs.size());

  AttributePairList expected;
  expected.push_back(AttributePair("foo", (int64) 13));
  expected.push_back(AttributePair("bar", "hello"));
  expected.push_back(AttributePair("baz", 15.5));
  expected.push_back(AttributePair("qux", Date(1999, 8, 29)));

  assert_attribute_pair_list(expected, attribs);
}

TEST_F(AttributeHashTest, Access) {
  AttributeHash attribs =
    attribute_pairs
      ("foo", (int64) 13)
      ("bar", "hello");

  assert_attribute((int64) 13, attribs["foo"]);
  assert_attribute("hello",    attribs["bar"]);
}

TEST_F(AttributeHashTest, MissingKeys) {
  AttributeHash attribs =
    attribute_pairs
      ("foo", (int64) 13)
      ("bar", "hello");

  Attribute a = attribs["xxx"];
  ASSERT_FALSE(a.has_data());
}
