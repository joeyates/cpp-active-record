#include <active_record/attribute.h>

#include <active_record/exception.h>

namespace ActiveRecord {

Type::Type Attribute::type() const {
  int type_index = which();
  return index_to_type(type_index);
}

bool Attribute::operator==(const Attribute& other) const {
  if(which() != other.which()) {
    return false;
  }

  // TYPE_LIST
  switch(which()) {
    case 0:
      return boost::get<int64>(*this) == boost::get<int64>(other);
    case 1:
      return boost::get<std::string>(*this) == boost::get<std::string>(other);
    case 2:
      return boost::get<double>(*this) == boost::get<double>(other);
    case 3:
      return boost::get<Date>(*this) == boost::get<Date>(other);
    default:
      throw ActiveRecordException(
        "Unexpected Attribute type", __FILE__, __LINE__
      );
  }
}

bool Attribute::operator!=(const Attribute& other) const {
  return !(*this == other);
}

std::ostream& operator<<(
  std::ostream& cout, ActiveRecord::Date const& date
) {
  cout << date.to_string();
  return cout;
}

std::ostream& operator<<(
  std::ostream& cout, const ActiveRecord::AttributeList& attributes
) {
  for(auto it = attributes.begin(); it != attributes.end(); ++it) {
    if(it != attributes.begin()) {
      cout << ", ";
    }
    cout << *it;
  }

  cout << std::endl;

  return cout;
}

std::ostream& operator<<(
  std::ostream& cout, const ActiveRecord::AttributeHash& attributes
) {
  for(auto& attribute: attributes) {
    if(attribute != *attributes.begin()) {
      cout << ", ";
    }
    cout << attribute.first << ": " << attribute.second;
  }

  cout << std::endl;

  return cout;
}

} // namespace ActiveRecord

std::ostream& operator<<(
  std::ostream& cout, const ActiveRecord::GenericAttributePairList& attributes
) {
  for(auto& attribute: attributes) {
    if(attribute != *attributes.begin()) {
      cout << ", ";
    }
    cout << attribute.first << ": " << attribute.second;
  }

  cout << std::endl;

  return cout;
}
