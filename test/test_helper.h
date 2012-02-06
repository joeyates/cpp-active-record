#ifndef _ACTIVE_RECORD_TEST_HELPER_H_
#define _ACTIVE_RECORD_TEST_HELPER_H_

#include <gtest/gtest.h>
#include "models.h"
#include <active_record/exception.h>

#define QUOTE(str) #str
#define Q(str) QUOTE(str)

using namespace std;

typedef list<string> strings;

strings shell_command( const string &command );

void connect_database( Connection &connection, const string &database_name );
void delete_database();
string table_definition( Connection &connection, const string &table_name );

void pipe_to_sqlite( const string &database_name, const string &command );

void assert_string( const string &expected, const string &actual );
void assert_attribute( const Attribute &expected, const Attribute &actual );
void assert_attribute_pair_list( const AttributePairList &expected,
                                 const AttributePairList &actual );

void assert_table_exists( const string &database_name, const string &table );

void assert_field_name( Table &td, int field_index, const string &name );
void assert_field_type( Table &td, int field_index, ActiveRecord::Type::Type type );
void assert_field( Table &td, int field_index, const string &name, ActiveRecord::Type::Type type );
void assert_file_exists( const string &file_name );
void assert_file_non_zero_length( const string &file_name );

// postgresql helpers
strings postgresql_shell_command( const string& database_name, const string &database_user, const string &query );
void postgresql_shell_create_database( const string &create_database_name,
                                       const string &access_database_name,
                                       const string &database_user );
void postgresql_shell_drop_database( const string &drop_database_name,
                                     const string &access_database_name,
                                     const string &database_user );
bool postgresql_shell_database_exists( const string &database_name, const string &database_user );

#endif // ndef _ACTIVE_RECORD_TEST_HELPER_H_
