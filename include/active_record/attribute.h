#ifndef _ACTIVE_RECORD_ATTRIBUTE_H_
#define _ACTIVE_RECORD_ATTRIBUTE_H_

#include <map>
#include <active_record/type.h>
#include <boost/assign.hpp>
#include <boost/variant.hpp>
#include <boost/assign/list_of.hpp>
using namespace std;
using namespace boost::assign;
using namespace boost;

namespace ActiveRecord {

// N.B. boost::variant.which() returns a 0-based index into this type list
typedef boost::variant< int, string, double > Attribute;
typedef pair< string, Attribute >             AttributePair;
typedef map< string, Attribute >              AttributeHash;
typedef vector< Attribute >                   AttributeList;
typedef vector< AttributePair >               AttributePairList;
typedef pair< string, AttributeList >         QueryParametersPair;
typedef assign_detail::generic_list< ActiveRecord::Attribute >     GenericAttributeList;
typedef assign_detail::generic_list< ActiveRecord::AttributePair > GenericAttributePairList;

Type get_type( const Attribute & attribute );

} // namespace ActiveRecord

// Instantiate boost::assign::list_of for our option type
// so we can define 'options' below
// Warning: this *may* be GCC STL-specific
namespace boost
{
namespace assign
{

template<>
inline ActiveRecord::GenericAttributePairList
list_of( const ActiveRecord::AttributePair &t ) {
  return ActiveRecord::GenericAttributePairList()( t );
}

} // namespace assign
} // namespace boost

namespace ActiveRecord {

/*
 ( attributes ( "foo" 13 ) ( "bar" "hello" ) ( "baz" 15.5 ) )
*/
inline ActiveRecord::GenericAttributePairList attributes( const char * name, const Attribute &value ) {
  return boost::assign::list_of( ActiveRecord::AttributePair( name, value ) );
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_ATTRIBUTE_H_
