#ifndef _ACTIVE_RECORD_TEST_HELPER_H_
#define _ACTIVE_RECORD_TEST_HELPER_H_

#include <gtest/gtest.h>
#include "models.h"

void connect_database();

void assert_field_name( const TableData &td, int field_index, const string &name );

#endif // ndef _ACTIVE_RECORD_TEST_HELPER_H_
