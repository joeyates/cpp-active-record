#ifndef _ACTIVE_RECORD_OPTIONS_H_
#define _ACTIVE_RECORD_OPTIONS_H_

#include <map>
#include <boost/assign.hpp>
#include <boost/variant.hpp>
#include <boost/assign/list_of.hpp>

namespace ActiveRecord {

// Options are for string only data
typedef std::pair<std::string, std::string> OptionPair;
typedef std::map<std::string, std::string> OptionsHash;

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
inline boost::assign_detail::generic_list<ActiveRecord::OptionPair> options(
  const char* name, const char* value
) {
  return boost::assign::list_of(ActiveRecord::OptionPair(name, value));
}

inline boost::assign_detail::generic_list<ActiveRecord::OptionPair> options(
  const std::string& name, const std::string& value
) {
  return boost::assign::list_of(ActiveRecord::OptionPair(name, value));
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_OPTIONS_H_
