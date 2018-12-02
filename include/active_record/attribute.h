#ifndef _ACTIVE_RECORD_ATTRIBUTE_H_
#define _ACTIVE_RECORD_ATTRIBUTE_H_

#include <cstdint>
#include <map>
#include <list>
#include <boost/assign.hpp>
#include <boost/variant.hpp>
#include <boost/assign/list_of.hpp>
#ifdef AR_POSTGRES
#include <postgresql/libpq-fe.h>
#endif // def AR_POSTGRES
#ifdef AR_SQLITE
#include <sqlite3.h>
#endif // def AR_SQLITE

#include <active_record/type.h>
#include <active_record/date.h>

namespace ActiveRecord {

// TYPE_LIST
typedef boost::variant<int64_t, std::string, double, Date> AttributeType;

// N.B. boost::variant.which() returns a 0-based index into the
// AttributeType list
class Attribute: public AttributeType {
  friend class Connection;
  friend class Row;

  // static
  public:

#ifdef AR_POSTGRES
  static Attribute from_field(PGresult* exec_result, int row, int column);
  static Type::Type pg_type_to_ar_type(Oid pg_type);
#endif // def AR_POSTGRES
#ifdef AR_SQLITE
  static Attribute from_field(sqlite3_stmt* pStmt, int i);
#endif // def AR_SQLITE

  // instance methods
  public:

  Attribute():                AttributeType(), initialised_(false) {}
  Attribute(int i):           AttributeType((int64_t) i), initialised_(true) {}
  // TYPE_LIST
  Attribute(int64_t i):       AttributeType(i), initialised_(true) {}
  Attribute(const std::string& s): AttributeType(s), initialised_(true) {}
  Attribute(const char* s):   AttributeType(std::string(s)),
    initialised_(true) {}
  Attribute(double d):        AttributeType(d), initialised_(true) {}
  Attribute(Date date):       AttributeType(date), initialised_(true) {}

  bool has_data() const { return initialised_; }
  Type::Type type() const;
  bool operator==(const Attribute& other) const;
  bool operator!=(const Attribute& other) const;

  private:

  bool initialised_;
};

typedef std::pair<std::string, Attribute> AttributePair;
typedef std::map<std::string, Attribute> AttributeHash;
typedef std::list<AttributePair> AttributePairList;
typedef std::list<Attribute> AttributeList;
typedef std::pair<std::string, AttributeList> QueryParametersPair;
typedef boost::assign_detail::generic_list<AttributePair>
  GenericAttributePairList;

std::ostream& operator<<(
  std::ostream& cout, const ActiveRecord::AttributeHash& attributes
);

std::ostream& operator<<(
  std::ostream& cout, const ActiveRecord::AttributeList& attributes
);

} // namespace ActiveRecord

std::ostream& operator<<(
  std::ostream& cout, const ActiveRecord::GenericAttributePairList& attributes
);

// Instantiate boost::assign::list_of for our option type
// so we can define 'attribute_pairs' below
// Warning: this *may* be GCC STL-specific
namespace boost
{
namespace assign
{

// See `attribute_pairs` definition, below

template<>
inline ActiveRecord::GenericAttributePairList
list_of(const ActiveRecord::AttributePair& t) {
  return ActiveRecord::GenericAttributePairList()(t);
}

// See `parameters` definition, below

template<>
inline assign_detail::generic_list<ActiveRecord::Attribute>
list_of(const ActiveRecord::Attribute& t) {
  return assign_detail::generic_list<ActiveRecord::Attribute>()(t);
}

} // namespace assign
} // namespace boost

namespace ActiveRecord {

/*
ActiveRecord::GenericAttributePairList attrs =
  attribute_pairs ("foo" 13) ("bar" "hello") ("baz" 15.5)
*/

inline GenericAttributePairList attribute_pairs(
  const char* name, const Attribute& value
) {
  return boost::assign::list_of(AttributePair(name, value));
}

/*
parameters (13) ("hello") (15.5)
*/

inline boost::assign_detail::generic_list<Attribute> parameters(
  const Attribute& value
) {
  return boost::assign::list_of(Attribute(value));
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_ATTRIBUTE_H_
