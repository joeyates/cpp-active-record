#include "./test.h"

class BaseToString: public BaseTest {
  protected:

  virtual void SetUp() {
    BaseTest::SetUp();
    add_cher();
  }
};

TEST_F(BaseToString, OutputsAttributes) {
  Person cher(1);

  assert_string(
    "Person: "
    "age 64, "
    "dob 1946-05-20, "
    "height 1.68, "
    "id 1, "
    "name Cherilyn, "
    "surname Sarkisian",
    cher.to_string()
  );
}
