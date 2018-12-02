#include "test_helper.h"
#include <active_record/field.h>

namespace ActiveRecord {

class FieldTest: public ::testing::Test {
  protected:

  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(FieldTest, ReturnsProperties) {
  ActiveRecord::Field field("foo", Type::integer);

  ASSERT_STREQ("foo", field.name().c_str());
  ASSERT_EQ(Type::integer, field.type());
}

TEST_F(FieldTest, SubtractsAccordingToName) {
  ActiveRecord::Field a("a", Type::integer);
  ActiveRecord::Field b("b", Type::integer);
  ActiveRecord::Field c("c", Type::integer);
  ActiveRecord::Field d("d", Type::integer);

  ActiveRecord::Fields abc;
  abc.push_back(a);
  abc.push_back(b);
  abc.push_back(c);

  ActiveRecord::Fields cd;
  cd.push_back(c);
  cd.push_back(d);

  ActiveRecord::Fields ab = abc - cd;

  ASSERT_EQ(2, ab.size());
  ASSERT_STREQ("a", ab[0].name().c_str());
  ASSERT_STREQ("b", ab[1].name().c_str());
}

TEST_F(FieldTest, Ostream) {
  std::stringstream field_out;
  ActiveRecord::Field a("a", Type::integer);

  field_out << a;

  assert_string(field_out.str(), "a INTEGER");
}

class FieldsTest: public ::testing::Test {
  protected:

  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(FieldsTest, DoesNotSubtractAccordingToType) {
  ActiveRecord::Field i("a", Type::integer);
  ActiveRecord::Field t("a", Type::text);
  ActiveRecord::Field f("a", Type::floating_point);

  ActiveRecord::Fields it;
  it.push_back(i);
  it.push_back(t);

  ActiveRecord::Fields tf;
  tf.push_back(t);
  tf.push_back(f);

  ActiveRecord::Fields result = it - tf;

  ASSERT_EQ(0, result.size());
}

TEST_F(FieldsTest, FieldsMethodInCtor) {
  ActiveRecord::Fields abc(
    ActiveRecord::fields
      ("a", Type::integer)
      ("b", Type::text)
      ("c", Type::floating_point)
      ("d", Type::date)
   );

  ASSERT_EQ(4, abc.size());
}

TEST_F(FieldsTest, FieldsMethodInAssignment) {
  ActiveRecord::Fields abc =
    ActiveRecord::fields
      ("a", Type::integer)
      ("b", Type::text)
      ("c", Type::floating_point)
      ("d", Type::date);

  ASSERT_EQ(4, abc.size());
}

} // namespace ActiveRecord
