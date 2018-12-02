#ifndef _ACTIVE_RECORD_FIELD_H_
#define _ACTIVE_RECORD_FIELD_H_

#include <active_record/type.h>
#include <boost/assign.hpp>
#include <boost/variant.hpp>
#include <boost/assign/list_of.hpp>
#include <sstream>

namespace ActiveRecord {

class Field {
  public:

  Field(const std::string& name, ActiveRecord::Type::Type type):
    name_(name),
    type_(type) {}

  inline const std::string& name() const { return name_; }
  inline ActiveRecord::Type::Type type() const { return type_; }

  private:

  std::string name_;
  ActiveRecord::Type::Type type_;
};

std::ostream& operator<<(std::ostream& cout, const ActiveRecord::Field& field);

typedef boost::assign_detail::generic_list<Field> GenericFieldList;

class Fields: public std::vector<Field> {
  public:

  Fields() {}

  // Allow Fields foo(fields (...) ... );
  Fields(const GenericFieldList& other) {
    for(auto& field: other) {
      push_back(field);
    }
  }

  Fields operator-(Fields& other) {
    Fields fields;
    for(auto& field: *this) {
      if(!other.has_field(field.name())) {
        fields.push_back(Field(field.name(), field.type()));
      }
    }
    return fields;
  }

  private:

  bool has_field(const std::string& field_name) {
    for(auto& field: *this) {
      if(field.name() == field_name) {
        return true;
      }
    }
    return false;
  }
};

} // namespace ActiveRecord

namespace boost
{
namespace assign
{

template<>
inline ActiveRecord::GenericFieldList
list_of(const ActiveRecord::Field& f) {
  return ActiveRecord::GenericFieldList()(f);
}

} // namespace boost
} // namespace assign

namespace ActiveRecord {

inline GenericFieldList fields(
  const char* name,
  ActiveRecord::Type::Type type
) {
  return boost::assign::list_of(Field(name, type));
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_FIELD_H_
