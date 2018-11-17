#ifndef _ACTIVE_RECORD_ATTRIBUTE_H_
#define _ACTIVE_RECORD_ATTRIBUTE_H_

#include <map>
#include <list>
#include <boost/assign.hpp>
#include <boost/variant.hpp>
#include <boost/assign/list_of.hpp>
#include <sqlite3.h>
#include <postgresql/libpq-fe.h>

#include <active_record/type.h>
#include <active_record/date.h>

using namespace std;
using namespace boost::assign;
using namespace boost;

namespace ActiveRecord {

typedef boost::variant<int, long long, string, double, Date> AttributeType;

// N.B. boost::variant.which() returns a 0-based index into the
// AttributeType list
class Attribute: public AttributeType {
  friend class Connection;
  friend class Row;

  // static
  public:

  static Attribute from_field(sqlite3_stmt *pStmt, int i);
  static Attribute from_field(PGresult * exec_result, int row, int column);
  static Type::Type pg_type_to_ar_type(Oid pg_type);

  // instance methods
  public:

  Attribute():                AttributeType(), initialised_(false) {}
  Attribute(int i):           AttributeType(i), initialised_(true) {}
  Attribute(long long i):     AttributeType(i), initialised_(true) {}
  Attribute(const string& s): AttributeType(s), initialised_(true) {}
  Attribute(const char* s):   AttributeType(string(s)), initialised_(true) {}
  Attribute(double d):        AttributeType(d), initialised_(true) {}
  Attribute(Date date):       AttributeType(date), initialised_(true) {}

  bool has_data() const { return initialised_; }
  Type::Type type() const;
  bool operator==(const Attribute& other) const;

  private:

  bool initialised_;
};

typedef pair<string, Attribute>             AttributePair;
typedef map<string, Attribute>              AttributeHash;
typedef list<AttributePair>                 AttributePairList;
typedef list<Attribute>                     AttributeList;
typedef pair<string, AttributeList>         QueryParametersPair;
typedef assign_detail::generic_list<AttributePair> GenericAttributePairList;

} // namespace ActiveRecord

// Instantiate boost::assign::list_of for our option type
// so we can define 'attribute_pairs' below
// Warning: this *may* be GCC STL-specific
namespace boost
{
namespace assign
{

template<>
inline ActiveRecord::GenericAttributePairList
list_of(const ActiveRecord::AttributePair& t) {
  return ActiveRecord::GenericAttributePairList()(t);
}

} // namespace assign
} // namespace boost

namespace ActiveRecord {

/*
 (attribute_pairs ("foo" 13) ("bar" "hello") ("baz" 15.5))
*/
inline GenericAttributePairList attribute_pairs(
  const char* name, const Attribute& value
) {
  return boost::assign::list_of(AttributePair(name, value));
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_ATTRIBUTE_H_
