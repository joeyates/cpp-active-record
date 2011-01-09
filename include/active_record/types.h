#ifndef _ACTIVE_RECORD_TYPES_H_
#define _ACTIVE_RECORD_TYPES_H_

#include <iostream> // For debugging
#include <map>
#include <string>
#include <vector>
#include <boost/assign.hpp>
#include <boost/variant.hpp>
#include <boost/assign/list_of.hpp>
using namespace boost::assign;
using namespace std;

namespace ActiveRecord {

// Options are for string only data
typedef pair< string, string >                OptionPair;
typedef map< string, string >                 OptionsHash;

// Attributes are typed
enum Type {
  integer        = 0,
  text           = 1,
  floating_point = 2
};
//define VARIANT_TYPES_BASE 1
// N.B. boost::variant.which() returns a 0-based index into this type list
typedef boost::variant< int, string, double > Attribute;
typedef pair< string, Attribute >             AttributePair;
typedef map< string, Attribute >              AttributeHash;
typedef vector< Attribute >                   AttributeList;
typedef vector< AttributePair >               AttributePairList;
typedef pair< string, AttributeList >         QueryParametersPair;
} // namespace ActiveRecord

// Instantiate boost::assign::list_of for our option type
// so we can define 'options' below
// Warning: this *may* be GCC STL-specific
namespace boost
{
namespace assign
{

template<>
inline assign_detail::generic_list< ActiveRecord::OptionPair >
list_of( const ActiveRecord::OptionPair &t ) {
  return assign_detail::generic_list< ActiveRecord::OptionPair >()( t );
}

}
}

namespace ActiveRecord {

// Define a method that takes a list of OptionPairs to define options
inline boost::assign_detail::generic_list< ActiveRecord::OptionPair > options( const char * name, const char * value ) {
  return boost::assign::list_of( ActiveRecord::OptionPair( name, value ) );
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TYPES_H_
