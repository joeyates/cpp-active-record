#include "./test.h"

class BaseLoadTest: public BaseTest {
  protected:

  virtual void SetUp() {
    BaseTest::SetUp();
    add_cher();
  }
};

TEST_F(BaseLoadTest, Default) {
  Person cher(1);

  ASSERT_EQ(cher.integer("age"), 64);
  assert_string(cher.text("name"), "Cherilyn");
  ASSERT_DOUBLE_EQ(cher.floating_point("height"), 1.68);
  assert_attribute(cher.date("dob"), Date(1946, 5, 20));
}

TEST_F(BaseLoadTest, LoadInvalidId) {
  Person cher(999);

  ASSERT_THROW(cher.text("name") , ActiveRecordException);
}

TEST_F(BaseLoadTest, SquareBracketsOperator) {
  Person cher(1);

  assert_attribute(cher["age"], 64);
  assert_attribute(cher["name"], "Cherilyn");
  assert_attribute(cher["height"], 1.68);
  assert_attribute(cher["dob"], Date(1946, 5, 20));
}

TEST_F(BaseLoadTest, HasDataUninitialized) {
  Person cher;

  ASSERT_FALSE(cher.has_data());
}

TEST_F(BaseLoadTest, HasDataOnlyId) {
  Person cher(1);

  ASSERT_FALSE(cher.has_data());
}

TEST_F(BaseLoadTest, HasDataWithAttributes) {
  Person cher(
    attribute_pairs
      ("age", 64)
      ("name", "Cherilyn")
      ("surname", "Sarkisian")
      ("height", 1.68)
    ("dob", Date(1946, 5, 20))
  );

  ASSERT_TRUE(cher.has_data());
}
