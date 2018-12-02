#include "./test.h"

namespace ActiveRecord {

class BaseOperators: public BaseTest {
  protected:

  virtual void SetUp() {
    BaseTest::SetUp();
    add_cher();
    add_christina();
  }
};

TEST_F(BaseOperators, Equality) {
  Person cher(1);
  Person christina(2);

  ASSERT_TRUE(cher == cher);
  ASSERT_FALSE(cher == christina);
}

TEST_F(BaseOperators, EqualityWithChangedAttribute) {
  Person cher1(1);
  Person cher2(1);
  cher2["name"] = "cher";

  ASSERT_FALSE(cher1 == cher2);
}

TEST_F(BaseOperators, EqualityUpdatedRecords) {
  Person cher1(1);
  Person cher2(1);
  cher1["name"] = "Cherilyn";
  cher1.save();

  cher2["name"] = "cher";
  cher2.save();

  ASSERT_FALSE(cher1 == cher2);
}

TEST_F(BaseOperators, StreamOutput) {
  Person cher(1);
  std::stringstream cher_out;

  cher_out << cher;

  assert_string(
    "Person: "
    "age 64, "
    "dob 1946-05-20, "
    "height 1.68, "
    "id 1, "
    "name Cherilyn, "
    "surname Sarkisian",
    cher_out.str()
  );
}

} // namespace ActiveRecord
