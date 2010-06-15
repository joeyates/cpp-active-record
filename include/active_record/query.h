#ifndef ACTIVE_RECORD_QUERY_H
#define ACTIVE_RECORD_QUERY_H

#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <active_record/table_data.h>
#include <boost/assign.hpp>
using namespace boost::assign;
using namespace std;

namespace ActiveRecord {

typedef pair<string, string> OptionPair;
typedef map<string, string>  Options;
// TODO: should not be a define - causes dependencies on boost::assign
#define to_options list_of< ActiveRecord::OptionPair >

template <class T>
class Query {
 public:
  Query() {};
  Query(const Query<T> &other);
  Query<T> operator=(const Query<T> &other);

  Query<T> where(Options conditions);
  Query<T> limit(int limit);
  Query<T> order(string order);

  string       sql();
 protected:
  Options        conditions_;
  int            limit_;
  vector<string> orderings_;
 private:
  string       condition_clause();
  string       order_clause();
};

extern map<string, TableData> tables;

template <class T>
Query<T>::Query(const Query<T> &other) {
  conditions_ = other.conditions_;
  limit_ = other.limit_;
  orderings_ = other.orderings_;
}

template <class T>
Query<T> Query<T>::operator=(const Query<T> &other) {
  Query<T> result(other);
  return result;
}

// foo.where(to_options ("name = ?", "Joe"));
template <class T>
Query<T> Query<T>::where(Options conditions) {
  conditions_ = conditions;
  return *this;
}

// foo.limit(50);
template <class T>
Query<T> Query<T>::limit(int limit) {
  limit_ = limit;
  return *this;
}

// foo.order("bar DESC");
template <class T>
Query<T> Query<T>::order(string order) {
  orderings_.push_back(order);
  return *this;
}

template <class T>
string Query<T>::condition_clause() {
  if (conditions_.size() == 0)
    return "";
  stringstream ss;
  for (Options::const_iterator it = conditions_.begin(); it != conditions_.end(); ++it) {
    if (it == conditions_.begin())
      ss << " WHERE ";
    else
      ss << " AND ";
    ss << it->first;
  }
  return ss.str();
}

template <class T>
string Query<T>::order_clause() {
  if (orderings_.size() == 0)
    return "";
  stringstream ss;
  ss << " ORDER BY";
  for (vector<string>::const_iterator it = orderings_.begin(); it != orderings_.end(); ++it)
    ss << " " << *it;
  return ss.str();
}

template <class T>
string Query<T>::sql() {
  TableData td = tables[T::table_name];
  stringstream ss;
  ss << "SELECT ";
  ss << td.primary_key << " ";
  ss << "FROM " << T::table_name;
  ss << condition_clause();
  ss << order_clause();
  ss << ";";
  return ss.str();
}


} // namespace ActiveRecord

#endif // ndef ACTIVE_RECORD_QUERY_H
