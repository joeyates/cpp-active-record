#ifndef _ACTIVE_RECORD_BASE_H_
#define _ACTIVE_RECORD_BASE_H_

#include <active_record/types.h>
#include <active_record/connection.h>
#include <active_record/query.h>
#include <active_record/table_data.h>

namespace ActiveRecord {

// Mm, nice, globals!
// TODO: thread safety
TableSet tables;
extern TypeNameMap type_name;

/*
GCC wants template instantiation in the same file as the declaration,
so all of the code is here.
 */

template <class T>
class Base {
 public:
  static string class_name;
  static void setup(Connection * connection) {
    TableData td;
    td.connection = connection;
    // Set defaults
    td.primary_key = "id";
    td.update_database = false;
    T::set_table_data(td);
    if (td.table_name.empty())
      throw "set_table_data() must set the table name";
    tables[T::class_name] = td;
    if (td.update_database)
      update_database();
  }
  Base() : loaded_(false) {
    attributes_[tables[T::class_name].primary_key] = TypedAttribute(ActiveRecord::integer, -1); // TODO invalid id
  }
  Base(int id) : loaded_(false) {
    attributes_[tables[T::class_name].primary_key] =
      TypedAttribute(ActiveRecord::integer, id);
  }
  Attribute& operator[](const string &name) {
    return attributes_[name].second;
  }
  string text(const string &name) {
    // TODO: check type? ...or allow conversion
    if (!loaded_)
      load();
    return boost::get<string>(attributes_[name].second);
  }
  int integer(const string &name) {
    // TODO: check type?
    if (!loaded_)
      load();
    return boost::get<int>(attributes_[name].second);
  }
  double floating_point(const string &name) {
    // TODO: check type?
    if (!loaded_)
      load();
    return boost::get<double>(attributes_[name].second);
  }
 private:
  static void update_database();
  bool          loaded_;
  AttributeHash attributes_;
  bool          load();
  bool          is_new() { return (attributes_[tables[T::class_name].primary_key] == -1)? true : false; }
};

template <class T>
bool Base<T>::load() {
  stringstream ss;
  ss << "SELECT * ";
  ss << "FROM " << tables[T::class_name].table_name << " ";
  ss << "WHERE ";
  ss << tables[T::class_name].primary_key << " = ";
  ss << attributes_[tables[T::class_name].primary_key].second;
  Row row = tables[T::class_name].connection->select_one(ss.str());
  attributes_ = row.attributes();
  loaded_ = true;
  return true;
}

// TODO: Handle alter table
template <class T>
void Base<T>::update_database() {
  TableData td = tables[T::class_name];
  stringstream ss;
  ss << "CREATE TABLE IF NOT EXISTS " << td.table_name << " (";
  ss << td.primary_key << " INTEGER PRIMARY KEY";
  for (vector<Field>::iterator it = td.fields.begin(); it != td.fields.end(); ++it) {
    ss << ", " << it->name() << " " << type_name[it->type()];
  }
  ss << ");";
  tables[T::class_name].connection->execute(ss.str());
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_BASE_H_
