#ifndef _ACTIVE_RECORD_OPTIONS_H_
#define _ACTIVE_RECORD_OPTIONS_H_

#include <map>
#include <boost/assign.hpp>
#include <boost/variant.hpp>
#include <boost/assign/list_of.hpp>
using namespace std;
using namespace boost::assign;
using namespace boost;

namespace ActiveRecord {

// Options are for string only data
typedef pair<string, string> OptionPair;
typedef map<string, string>  OptionsHash;

} // namespace ActiveRecord

// Instantiate boost::assign::list_of for our option type
// so we can define 'options' below
// Warning: this *may* be GCC STL-specific
namespace boost
{
namespace assign
{

template<>
inline assign_detail::generic_list<ActiveRecord::OptionPair>
list_of(const ActiveRecord::OptionPair& t) {
  return assign_detail::generic_list<ActiveRecord::OptionPair>()(t);
}

} // namespace boost
} // namespace assign

namespace ActiveRecord {

// Define a method that takes a list of OptionPairs to define options
inline assign_detail::generic_list<ActiveRecord::OptionPair> options(
  const char* name, const char* value
) {
  return assign::list_of(ActiveRecord::OptionPair(name, value));
}
inline assign_detail::generic_list<ActiveRecord::OptionPair> options(
  const string& name, const string& value
) {
  return assign::list_of(ActiveRecord::OptionPair(name, value));
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_OPTIONS_H_
