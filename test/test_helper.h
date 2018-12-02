#ifndef _ACTIVE_RECORD_TEST_HELPER_H_
#define _ACTIVE_RECORD_TEST_HELPER_H_

#include <gtest/gtest.h>
#include "models.h"
#include <active_record/exception.h>

typedef std::list<std::string> strings;

namespace ActiveRecord {

strings shell_command(const std::string& command);

void sqlite_connect_database(
  ActiveRecord::Connection& connection,
  const std::string& database_name
);
void sqlite_delete_database();
std::string sqlite_table_definition(
  ActiveRecord::Connection& connection,
  const std::string& table_name
);

void pipe_to_sqlite(
  const std::string& database_name,
  const std::string& command
);

void assert_string(const std::string& expected, const std::string& actual);
void assert_attribute(
  const ActiveRecord::Attribute& expected,
  const ActiveRecord::Attribute& actual
);
void assert_attribute_pair_list(
  const ActiveRecord::AttributePairList& expected,
  const ActiveRecord::AttributePairList& actual
);

void sqlite_assert_table_exists(
  const std::string& database_name,
  const std::string& table
);

void assert_field_name(
  ActiveRecord::Table& td,
  int field_index,
  const std::string& name
);
void assert_field_type(
  ActiveRecord::Table& td,
  int field_index,
  ActiveRecord::Type::Type type
);
void assert_field(
  ActiveRecord::Table& td,
  int field_index,
  const std::string& name,
  ActiveRecord::Type::Type type
);
void assert_file_exists(const std::string& file_name);
void assert_file_non_zero_length(const std::string& file_name);

// postgresql helpers
std::string postgresql_invocation(const ActiveRecord::OptionsHash& options);
std::string postgresql_conninfo(const ActiveRecord::OptionsHash& options);
strings postgresql_shell_command(
  const std::string& query,
  const ActiveRecord::OptionsHash& connection_options
);
void postgresql_shell_create_database(
  const std::string& create_database_name,
  const ActiveRecord::OptionsHash& connection_options
);
void postgresql_shell_drop_database(
  const std::string& drop_database_name,
  const ActiveRecord::OptionsHash& connection_options
);
bool postgresql_shell_database_exists(
  const std::string& database_name,
  const ActiveRecord::OptionsHash& connection_options
);
void assert_postgresql_table_exists(
  const std::string& table_name,
  const ActiveRecord::OptionsHash& connection_options
);

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TEST_HELPER_H_
