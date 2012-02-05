#ifndef _ACTIVE_RECORD_TEST_HELPER_H_
#define _ACTIVE_RECORD_TEST_HELPER_H_

#include <gtest/gtest.h>
#include "models.h"
#include <active_record/exception.h>

using namespace std;

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
void assert_field_type( Table &td, int field_index, ActiveRecord::Type type );
void assert_field( Table &td, int field_index, const string &name, ActiveRecord::Type type );
void assert_file_exists( const string &file_name );
void assert_file_non_zero_length( const string &file_name );

#endif // ndef _ACTIVE_RECORD_TEST_HELPER_H_
