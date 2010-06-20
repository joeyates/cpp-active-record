#ifndef _ACTIVE_RECORD_BASE_H_
#define _ACTIVE_RECORD_BASE_H_

#include <active_record/connection.h>
#include <active_record/query.h>
#include <active_record/types.h>

namespace ActiveRecord {

template <class T>
class Base {
 public:
  Base(int id);
  /* Attribute access and assignment */
  Attribute& operator[](const string &name);
  static string       table_name;
  static Connection * connection;
 private:
  // TODO stop direct instantiation of this base class
  bool          loaded_;
  AttributeHash attributes_;
  bool          load();
};

template <class T>
Base<T>::Base(int id) : loaded_(false) {
  attributes_["id"] = TypedAttribute(ActiveRecord::integer, id);
}

template <class T>
Attribute& Base<T>::operator[](const string &name) {
  if (!loaded_)
    load();
  return attributes_[name].second;
}

template <class T>
bool Base<T>::load() {
  stringstream ss;
  ss << "SELECT * ";
  ss << "FROM " << table_name << " ";
  ss << "WHERE id = " << attributes_["id"].second; // TODO: use correct pkey name
  Row row = connection->select_one(ss.str());
  attributes_ = row.attributes();
  loaded_ = true;
  return true;
}

} // namespace ActiveRecord

// Allow abbreviations - you just need to say 'using ActiveRecordAbbreviations;' to be able to use AR:: instead of ActiveRecord::
namespace ActiveRecordAbbreviations {
  namespace AR = ActiveRecord;
}

#endif // ndef _ACTIVE_RECORD_BASE_H_
