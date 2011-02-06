#ifndef _ACTIVE_RECORD_TABLE_SET_H_
#define _ACTIVE_RECORD_TABLE_SET_H_

#include <active_record/table.h>

namespace ActiveRecord {

class TableSet : public map< string, Table > {
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TABLE_SET_H_
