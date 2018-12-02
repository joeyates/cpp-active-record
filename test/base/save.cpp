#include "./test.h"

namespace ActiveRecord {

class BaseSaveTest: public BaseTest {};

TEST_F(BaseSaveTest, SetsId) {
  Person joe;
  joe["name"] = "Joe";

  ASSERT_TRUE(joe.save());
  ASSERT_TRUE(joe.id() > 0);
}

TEST_F(BaseSaveTest, WorksWithNoData) {
  Person joe;

  ASSERT_TRUE(joe.save());
  ASSERT_TRUE(joe.id() > 0);
}

TEST_F(BaseSaveTest, SavesAttributes) {
  Person joe(
    attribute_pairs
      ("age", 45)
      ("name", "Joe")
      ("surname", "Yates")
      ("height", 1.80)
      ("dob", Date(1965, 7, 31))
  );
  ASSERT_TRUE(joe.save());

  Person first(1);
  ASSERT_EQ(45,                       first.integer("age"));
  assert_string("Joe",                first.text("name"));
  ASSERT_DOUBLE_EQ(1.80,              first.floating_point("height"));
  assert_attribute(Date(1965, 7, 31), first.date("dob"));
}

} // namespace ActiveRecord
