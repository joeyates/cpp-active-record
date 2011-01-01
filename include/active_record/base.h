#ifndef _ACTIVE_RECORD_BASE_H_
#define _ACTIVE_RECORD_BASE_H_

#include <active_record/types.h>
#include <active_record/connection.h>
#include <active_record/query.h>
#include <active_record/table_data.h>

#define ACTIVE_RECORD_UNSAVED -1

namespace ActiveRecord {

extern TableSet tables;
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
    attributes_[tables[T::class_name].primary_key] = ACTIVE_RECORD_UNSAVED;
  }
  Base(int id) : loaded_(false) {
    attributes_[tables[T::class_name].primary_key] = id;
  }
  Attribute& operator[](const string &name) {
    return attributes_[name];
  }
  string text(const string &name) {
    if (is_new())
      return boost::get<string>(attributes_[name]);
    // TODO: check type? ...or allow conversion
    if (!loaded_)
      load();
    return boost::get<string>(attributes_[name]);
  }
  int integer(const string &name) {
    if (is_new())
      return boost::get<int>(attributes_[name]);
    // TODO: check type?
    if (!loaded_)
      load();
    return boost::get<int>(attributes_[name]);
  }
  double floating_point(const string &name) {
    if (is_new())
      return boost::get<double>(attributes_[name]);
    // TODO: check type?
    if (!loaded_)
      load();
    return boost::get<double>(attributes_[name]);
  }
  bool save() {
    if (is_new())
      return create();
    else
      return update();
  }
 private:
  static void   update_database();
  bool          loaded_;
  AttributeHash attributes_;
  bool          load();
  bool          create();
  bool          update();
  inline int id() {
    return boost::get<int>(attributes_[tables[T::class_name].primary_key]);
  }
  inline bool   is_new() {
    return (id() == ACTIVE_RECORD_UNSAVED)? true : false;
  }
};

/////////////////////////////////////////
// Private

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

template <class T>
bool Base<T>::load() {
  stringstream ss;
  ss << "SELECT * ";
  ss << "FROM " << tables[T::class_name].table_name << " ";
  ss << "WHERE ";
  ss << tables[T::class_name].primary_key << " = ";
  ss << attributes_[tables[T::class_name].primary_key];
  Row row = tables[T::class_name].connection->select_one(ss.str());
  attributes_ = row.attributes();
  loaded_ = true;
  return true;
}

// TODO: Re-read the record afterwards to get default values?
template <class T>
bool Base<T>::create() {
  stringstream ss, columns, placeholders;
  ss << "INSERT INTO " << tables[T::class_name].table_name << " ";
  bool columns_added = false;
  AttributeList parameters;
  for (AttributeHash::iterator it = attributes_.begin(); it != attributes_.end(); ++it) {
    if (it->first == tables[T::class_name].primary_key)
      continue;
    if (columns_added) {
      columns << ", ";
      placeholders << ", ";
    }
    columns << it->first;
    placeholders << "?";
    parameters.push_back(it->second);
    columns_added = true;
  }
  ss << "(" << columns.str() << ") VALUES (" << placeholders.str() << ");";
  return tables[T::class_name].connection->execute(ss.str(), parameters);  
}

template <class T>
bool Base<T>::update() {
  if (!loaded_)
    load();
  stringstream ss, columns;
  ss << "UPDATE " << tables[T::class_name].table_name << " ";
  bool columns_added = false;
  AttributeList parameters;
  for (AttributeHash::iterator it = attributes_.begin(); it != attributes_.end(); ++it) {
    if (it->first == tables[T::class_name].primary_key)
      continue;
    if (columns_added)
      columns << ", ";
    columns << it->first << " = ?";
    parameters.push_back(it->second);
    columns_added = true;
  }
  ss << "SET " << columns.str() << " ";
  ss << "WHERE " << tables[T::class_name].primary_key << " = ?;";
  parameters.push_back(id());
  return tables[T::class_name].connection->execute(ss.str(), parameters);
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_BASE_H_
