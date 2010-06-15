#ifndef ACTIVE_RECORD_BASE_H
#define ACTIVE_RECORD_BASE_H

#include <active_record/connection.h>
#include <active_record/query.h>

namespace ActiveRecord {

template <class T>
  class Base : public Query<T> {
 public:
  virtual ~Base() = 0; // stop direct instantiation of this base class
  static string table_name;
};

template <class T>
  Base<T>::~Base() {}

} // namespace ActiveRecord

// Allow abbreviations - you just need to say 'using ActiveRecordAbbreviations;' to be able to use AR:: instead of ActiveRecord::
namespace ActiveRecordAbbreviations {
  namespace AR = ActiveRecord;
}

#endif // ndef ACTIVE_RECORD_BASE_H
