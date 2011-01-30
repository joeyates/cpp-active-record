#ifndef _ACTIVE_RECORD_TEST_HELPER_H_
#define _ACTIVE_RECORD_TEST_HELPER_H_

#include <gtest/gtest.h>
#include "models.h"

void connect_database();
void delete_database();

void assert_field_name( const Table &td, int field_index, const string &name );
void assert_field_type( const Table &td, int field_index, ActiveRecord::Type type );
void assert_field( const Table &td, int field_index, const string &name, ActiveRecord::Type type );

#endif // ndef _ACTIVE_RECORD_TEST_HELPER_H_
