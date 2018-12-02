#ifndef _ACTIVE_RECORD_QUERY_H_
#define _ACTIVE_RECORD_QUERY_H_

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include <active_record/type.h>
#include <active_record/table.h>
#include <active_record/table_set.h>
#include <active_record/row.h>
#include <active_record/connection.h>

#define INVALID_LIMIT -1

namespace ActiveRecord {

template <class T>
class Query {
  public:

  Query(Connection& connection):
    limit_(INVALID_LIMIT),
    offset_(0),
    connection_(&connection) {};
  Query(const Query<T>& other);

  Query<T> operator=(const Query<T>& other);

  Query<T> where(const std::string& condition, const Attribute& value);
  Query<T> order(std::string order);
  Query<T> limit(int limit);
  Query<T> offset(int offset);
  // Results
  T first();
  std::vector<T> all();

  protected:

  AttributePairList   conditions_;
  std::vector<std::string> orderings_;
  int                 limit_;
  int                 offset_;
  Connection*         connection_;

  private:

  QueryParametersPair    condition_clause();
  std::string            order_clause();
  std::string            limit_clause();
  std::string            offset_clause();
  QueryParametersPair    query_and_parameters();
};

template <class T>
Query<T>::Query(const Query<T>& other) {
  conditions_ = other.conditions_;
  orderings_  = other.orderings_;
  limit_      = other.limit_;
  offset_     = other.offset_;
  connection_ = other.connection_;
}

template <class T>
Query<T> Query<T>::operator=(const Query<T>& other) {
  Query<T> result(other);
  return result;
}

// foo.where("name = ?", "Joe");
template <class T>
Query<T> Query<T>::where(const std::string& condition, const Attribute& value) {
  conditions_.push_back(AttributePair(condition, value));
  return *this;
}

// foo.order("bar DESC");
template <class T>
Query<T> Query<T>::order(std::string order) {
  orderings_.push_back(order);
  return *this;
}

// foo.limit(50);
template <class T>
Query<T> Query<T>::limit(int limit) {
  limit_ = limit;
  return *this;
}

// foo.offset(50);
template <class T>
Query<T> Query<T>::offset(int offset) {
  offset_ = offset;
  return *this;
}

template <class T>
std::vector<T> Query<T>::all() {
  QueryParametersPair query = query_and_parameters();
  AttributeList ids = connection_->select_values(query.first, query.second);
  std::vector<T> results;

  for(auto& id: ids) {
    T record(boost::get<int64_t>(id));
    results.push_back(record);
  }

  return results;
}

template <class T>
T Query<T>::first() {
  QueryParametersPair query = query_and_parameters();

  Row row = connection_->select_one(query.first, query.second);
  if(!row.has_data()) {
    throw ActiveRecordException("No data", __FILE__, __LINE__);
  }

  Table t = connection_->get_table(T::class_name);
  std::string primary_key = t.primary_key();
  T record(row.get_integer(primary_key));

  return record;
}

//////////////////////////////////////////////////
// private

template <class T>
QueryParametersPair Query<T>::condition_clause() {
  AttributeList parameters;

  if(conditions_.size() == 0) {
    return QueryParametersPair("", parameters);
  }

  std::stringstream ss;
  ss << " ";

  for(auto& condition: conditions_) {
    if(condition == *conditions_.begin()) {
      ss << "WHERE ";
    } else {
      ss << " AND ";
    }
    ss << condition.first;
    parameters.push_back(condition.second);
  }

  return QueryParametersPair(ss.str(), parameters);
}

template <class T>
std::string Query<T>::order_clause() {
  if(orderings_.size() == 0) {
    return "";
  }

  std::stringstream ss;
  ss << " ";

  for(auto& ordering: orderings_) {
    if(ordering == *orderings_.begin()) {
      ss << "ORDER BY ";
    } else {
      ss << ", ";
    }

    ss << ordering;
  }

  return ss.str();
}

template <class T>
std::string Query<T>::limit_clause() {
  if(limit_ == INVALID_LIMIT) {
    return "";
  }

  std::stringstream ss;
  ss << " LIMIT " << limit_;

  return ss.str();
}

template <class T>
std::string Query<T>::offset_clause() {
  if(offset_ == 0) {
    return "";
  }

  std::stringstream ss;

#ifdef AR_SQLITE
  if(dynamic_cast<Sqlite3Connection*>(connection_) != nullptr) {
    // Sqlite requires a LIMIT clause before OFFSET
    if(limit_ == INVALID_LIMIT) {
      ss << " LIMIT -1";
    }
  }
#endif // def AR_SQLITE

  ss << " OFFSET " << offset_;

  return ss.str();
}

template <class T>
QueryParametersPair Query<T>::query_and_parameters() {
  Table t = connection_->get_table(T::class_name);

  std::stringstream ss;
  ss << "SELECT ";
  ss << t.primary_key() << " ";
  ss << "FROM ";
  ss << t.table_name();

  QueryParametersPair conditions = condition_clause();
  ss << conditions.first;
  ss << order_clause();
  ss << limit_clause();
  ss << offset_clause();
  ss << ";";

  return QueryParametersPair(ss.str(), conditions.second);
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_QUERY_H_
