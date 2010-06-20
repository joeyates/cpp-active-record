#ifndef _ACTIVE_RECORD_TYPES_H_
#define _ACTIVE_RECORD_TYPES_H_

#include <iostream>
#include <map>
#include <boost/assign.hpp>
#include <boost/variant.hpp>
#include <boost/assign/list_of.hpp>
using namespace boost::assign;
using namespace std;

namespace ActiveRecord {

enum Type {
  unknown = 0,
  integer = 1,
  text = 2,
  floating_point = 3
};

class Connection;

struct TableData {
  Connection *        connection;
  string              primary_key;
  string              table_name;
  //map<string, string> attributes;
};

typedef map< string, TableData >              TableSet;
// Options are for string only data
typedef pair< string, string >                OptionPair;
typedef map< string, string >                 OptionsHash;
// Attributes are typed
typedef boost::variant< int, string, double > Attribute;
typedef pair< Type, Attribute >               TypedAttribute;
typedef pair< string, TypedAttribute >        TypedAttributePair;
typedef map< string, TypedAttribute >         AttributeHash;
typedef vector< TypedAttribute >              AttributeList;
typedef vector< TypedAttributePair >          TypedAttributePairList;
typedef pair< string, AttributeList >         QueryParametersPair;
} // namespace ActiveRecord


// Instantiate boost::assign::list_of for our option and parameter types
// so we can define 'options' and 'conditions' below
// Warning: this *may* be GCC STL-specific
namespace boost
{
namespace assign
{
template<>
inline assign_detail::generic_list< ActiveRecord::OptionPair > 
list_of(const ActiveRecord::OptionPair &t) {
  return assign_detail::generic_list< ActiveRecord::OptionPair >()(t);
}

template<>
inline assign_detail::generic_list< ActiveRecord::TypedAttributePair > 
list_of(const ActiveRecord::TypedAttributePair &t) {
  return assign_detail::generic_list< ActiveRecord::TypedAttributePair >()(t);
}
}
}

namespace ActiveRecord {

// Define a method that takes a list of OptionPairs to define options
inline boost::assign_detail::generic_list< ActiveRecord::OptionPair > options(const char * name, const char * value) {
  return boost::assign::list_of(ActiveRecord::OptionPair(name, value));
}

inline boost::assign_detail::generic_list< ActiveRecord::TypedAttributePair >
conditions(const char * name, int value) {
  return boost::assign::list_of(ActiveRecord::TypedAttributePair(name, ActiveRecord::TypedAttribute(integer, value)));
}

inline boost::assign_detail::generic_list< ActiveRecord::TypedAttributePair >
conditions(const string &name, const string &value) {
  return boost::assign::list_of(ActiveRecord::TypedAttributePair(name, ActiveRecord::TypedAttribute(text, value)));
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TYPES_H_
