#include "test_helper.h"
#include <active_record/connection/postgresql.h>
#include <active_record/connection/sqlite3.h>

extern std::string database_name;

namespace ActiveRecord {

class TableTest: public ::testing::Test {};

TEST_F(TableTest, Defaults) {
  ActiveRecord::Table table;
  ASSERT_EQ("id",  table.primary_key());
  ASSERT_EQ(false, table.timestamps());
}

TEST_F(TableTest, Attributes) {
  ActiveRecord::Table table(NULL, "foo");
  ASSERT_EQ("id",  table.primary_key());
  ASSERT_EQ(false, table.timestamps());
  ASSERT_EQ("foo",  table.table_name());
}

TEST_F(TableTest, Fields) {
  ActiveRecord::Table table(NULL, "foo");
  table.fields().push_back(Field("name", Type::text));

  ASSERT_EQ(1, table.fields().size());
}

TEST_F(TableTest, FieldsAssignmentList) {
  ActiveRecord::Table td( NULL, "people");
  td.fields(
    ActiveRecord::fields
      ("a", Type::integer)
      ("b", Type::text)
      ("c", Type::floating_point)
  );

  ASSERT_EQ( 3, td.fields().size());
}

TEST_F(TableTest, AddFieldWithoutConnection) {
  ActiveRecord::Table table(NULL, "foo");

  ASSERT_THROW(
    table.add_field( Field("bar", Type::text)), ActiveRecordException
  );
}

TEST_F(TableTest, Ostream) {
  ActiveRecord::Table td(NULL, "people");
  td.fields(
    ActiveRecord::fields
      ("a", Type::integer)
      ("b", Type::text)
      ("c", Type::floating_point)
   );

  std::stringstream table_out;
  table_out << td;

  assert_string("people: a INTEGER, b TEXT, c FLOAT", table_out.str());
}

} // namespace ActiveRecord
