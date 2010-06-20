#ifndef _ACTIVE_RECORD_QUERY_H_
#define _ACTIVE_RECORD_QUERY_H_

#include <sstream>
#include <string>
#include <vector>
#include <active_record/row.h>
#include <active_record/types.h>
#include <active_record/connection.h>
using namespace std;

namespace ActiveRecord {

template <class T>
class Query {
 public:
  Query() {};
  Query(const Query<T> &other);
  Query<T> operator=(const Query<T> &other);

  Query<T>  where(TypedAttributePairList conditions);
  Query<T>  limit(int limit);
  Query<T>  order(string order);
  // Results
  T         first();
  vector<T> all();
 protected:
  TypedAttributePairList  conditions_;
  int            limit_;
  vector<string> orderings_;
 private:
  QueryParametersPair condition_clause();
  string              order_clause();
  QueryParametersPair query_and_parameters();
};

extern map<string, TableData> tables;

template <class T>
Query<T>::Query(const Query<T> &other) {
  conditions_ = other.conditions_;
  limit_      = other.limit_;
  orderings_  = other.orderings_;
}

template <class T>
Query<T> Query<T>::operator=(const Query<T> &other) {
  Query<T> result(other);
  return result;
}

// foo.where(options ("name = ?", "Joe"));
template <class T>
Query<T> Query<T>::where(TypedAttributePairList conditions) {
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

// foo.all();
template <class T>
vector<T> Query<T>::all() {
  TableData td = tables[T::table_name];
  QueryParametersPair query = query_and_parameters();
  RowSet rows = T::connection->select_values(query.first, query.second);
  vector<T> results;
  for (RowSet::iterator it = rows.begin(); it != rows.end(); ++it) {
    T t(it->get_integer(td.primary_key));
    results.push_back(t);
  }
  return results;
}

/////////////////////////////////////////////
// Private

template <class T>
QueryParametersPair Query<T>::condition_clause() {
  AttributeList parameters;
  if (conditions_.size() == 0)
    return QueryParametersPair("", parameters);
  stringstream ss;
  for (TypedAttributePairList::const_iterator it = conditions_.begin(); it != conditions_.end(); ++it) {
    if (it == conditions_.begin())
      ss << " WHERE ";
    else
      ss << " AND ";
    ss << it->first;
    parameters.push_back(it->second);
  }
  return QueryParametersPair(ss.str(), parameters);
}

// BUG HERE: there should be a comma between orderings!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
QueryParametersPair Query<T>::query_and_parameters() {
  TableData td = tables[T::table_name];
  stringstream ss;
  ss << "SELECT ";
  ss << td.primary_key << " ";
  ss << "FROM " << T::table_name;
  QueryParametersPair conditions = condition_clause();
  ss << conditions.first;
  ss << order_clause();
  ss << ";";
  return QueryParametersPair(ss.str(), conditions.second);
}


} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_QUERY_H_
