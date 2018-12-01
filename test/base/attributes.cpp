#include "./test.h"

class BaseAttributesTest: public BaseTest {};

TEST_F(BaseAttributesTest, IdDefaultsToMinusOne) {
  Person joe;

  ASSERT_EQ(-1, joe.id());
}

TEST_F(BaseAttributesTest, CanBeInitializedViaAttributePairs) {
  Person joe(
    attribute_pairs
      ("age", 45)
      ("name", "Joe")
      ("surname", "Yates")
      ("height", 1.80)
      ("dob", Date(1965, 7, 31))
  );

  ASSERT_EQ(45,                       joe.integer("age"));
  assert_string("Joe",                joe.text("name"));
  ASSERT_DOUBLE_EQ(1.80,              joe.floating_point("height"));
  assert_attribute(Date(1965, 7, 31), joe.date("dob"));
}

TEST_F(BaseAttributesTest, CanBeSetSingly) {
  Person joe;

  joe["age"]     = 45;
  joe["name"]    = "Joe";
  joe["surname"] = "Yates";
  joe["height"]  = 1.80;
  joe["dob"]     = Date(1965, 7, 31);

  ASSERT_EQ(45,                       joe.integer("age"));
  assert_string("Joe",                joe.text("name"));
  ASSERT_DOUBLE_EQ(1.80,              joe.floating_point("height"));
  assert_attribute(Date(1965, 7, 31), joe.date("dob"));
}

TEST_F(BaseAttributesTest, CanBeSetViaInit) {
  Person joe;
  ASSERT_NO_THROW(
    joe.init(
      attribute_pairs
        ("age", 45)
        ("name", "Joe")
        ("surname", "Yates")
        ("height", 1.80)
        ("dob", Date(1965, 7, 31))
    )
  );

  ASSERT_EQ(45,                       joe.integer("age"));
  assert_string("Joe",                joe.text("name"));
  ASSERT_DOUBLE_EQ(1.80,              joe.floating_point("height"));
  assert_attribute(Date(1965, 7, 31), joe.date("dob"));
}
