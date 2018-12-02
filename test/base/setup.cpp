#include "../test_helper.h"
#include <active_record/connection/sqlite3.h>

extern std::string database_name;

namespace ActiveRecord {

class NoTableNameModel: public ActiveRecord::Base<NoTableNameModel> {
  public:

  AR_CONSTRUCTORS(NoTableNameModel)
  static Table table(Connection* connection) {
    Table td;
    return td;
  }
};

AR_DECLARE(NoTableNameModel)

class BaseSetupTest: public ::testing::Test {};

TEST_F(BaseSetupTest, RequiresTableName) {
  ASSERT_THROW(
    NoTableNameModel::setup(nullptr), ActiveRecord::ActiveRecordException
  );
}

} // namespace ActiveRecord
