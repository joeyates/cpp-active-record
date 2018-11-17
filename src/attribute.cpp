#include <active_record/attribute.h>

#include <active_record/exception.h>

namespace ActiveRecord {

Type::Type Attribute::type() const {
  int type_index = which();
  return index_to_type(type_index);
}

bool Attribute::operator==(const Attribute& other) const {
  if(which() != other.which())
    return false;

  switch(which()) {
  case 0:
    return boost::get<int>(*this) == boost::get<int>(other);
  case 1:
    return boost::get<long long>(*this) == boost::get<long long>(other);
  case 2:
    return boost::get<string>(*this) == boost::get<string>(other);
  case 3:
    return boost::get<double>(*this) == boost::get<double>(other);
  case 4:
    return boost::get<Date>(*this) == boost::get<Date>(other);
  default:
    throw ActiveRecordException(
      "Unexpected Attribute type", __FILE__, __LINE__
    );
  }
}

ostream& operator<<(ostream& cout, ActiveRecord::Date const& date) {
  cout << date.to_string();
  return cout;
}

} // namespace ActiveRecord
