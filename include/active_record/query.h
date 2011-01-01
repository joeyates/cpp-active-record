#ifndef _ACTIVE_RECORD_QUERY_H_
#define _ACTIVE_RECORD_QUERY_H_

#include <sstream>
#include <string>
#include <vector>
#include <active_record/types.h>
#include <active_record/table_data.h>
#include <active_record/row.h>
#include <active_record/connection.h>
using namespace std;

#define INVALID_LIMIT -1

namespace ActiveRecord {

extern TableSet tables;

template < class T >
class Query {
 public:
  Query() : limit_(INVALID_LIMIT) {};
  Query(const Query< T > &other);
  Query< T > operator=(const Query< T > &other);

  Query< T >  where(const string &condition, const Attribute &value);
  Query< T >  order(string order);
  Query< T >  limit(int limit);
  // Results
  T         first();
  vector< T > all();
 protected:
  AttributePairList  conditions_;
  int            limit_;
  vector< string > orderings_;
 private:
  QueryParametersPair condition_clause();
  string              order_clause();
  string              limit_clause();
  QueryParametersPair query_and_parameters();
};

template < class T >
Query< T >::Query(const Query< T > &other) {
  conditions_ = other.conditions_;
  limit_      = other.limit_;
  orderings_  = other.orderings_;
}

template < class T >
Query< T > Query< T >::operator=(const Query< T > &other) {
  Query< T > result(other);
  return result;
}

// foo.where(options ("name = ?", "Joe"));
template < class T >
Query< T > Query< T >::where(const string &condition, const Attribute &value) {
  conditions_.push_back(AttributePair(condition, value));
  return *this;
}

// foo.limit(50);
template < class T >
Query< T > Query< T >::limit(int limit) {
  limit_ = limit;
  return *this;
}

// foo.order("bar DESC");
template < class T >
Query< T > Query< T >::order(string order) {
  orderings_.push_back(order);
  return *this;
}

// foo.all();
template < class T >
vector< T > Query< T >::all() {
  QueryParametersPair query = query_and_parameters();
  RowSet rows = tables[T::class_name].connection->select_values(query.first, query.second);
  vector< T > results;
  for (RowSet::iterator it = rows.begin(); it != rows.end(); ++it) {
    T t(it->get_integer(tables[T::class_name].primary_key));
    results.push_back(t);
  }
  return results;
}

/////////////////////////////////////////////
// Private

template < class T >
QueryParametersPair Query< T >::condition_clause() {
  AttributeList parameters;
  if (conditions_.size() == 0)
    return QueryParametersPair("", parameters);
  stringstream ss;
  ss << " ";
  for (AttributePairList::const_iterator it = conditions_.begin(); it != conditions_.end(); ++it) {
    if (it == conditions_.begin())
      ss << "WHERE ";
    else
      ss << " AND ";
    ss << it->first;
    parameters.push_back(it->second);
  }
  return QueryParametersPair(ss.str(), parameters);
}

template < class T >
string Query< T >::order_clause() {
  if (orderings_.size() == 0)
    return "";
  stringstream ss;
  ss << " ";
  for (vector< string >::const_iterator it = orderings_.begin(); it != orderings_.end(); ++it) {
    if (it == orderings_.begin())
      ss << "ORDER BY ";
    else
      ss << ", ";
    ss << *it;
  }
  return ss.str();
}

template < class T >
string Query< T >::limit_clause() {
  if (limit_ == INVALID_LIMIT)
    return "";
  stringstream ss;
  ss << " LIMIT " << limit_;
  return ss.str();
}

template < class T >
QueryParametersPair Query< T >::query_and_parameters() {
  stringstream ss;
  ss << "SELECT ";
  ss << tables[T::class_name].primary_key << " ";
  ss << "FROM ";
  ss << tables[T::class_name].table_name;
  QueryParametersPair conditions = condition_clause();
  ss << conditions.first;
  ss << order_clause();
  ss << limit_clause();
  ss << ";";
  return QueryParametersPair(ss.str(), conditions.second);
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_QUERY_H_
