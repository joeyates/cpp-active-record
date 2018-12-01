#ifndef _ACTIVE_RECORD_FIELD_H_
#define _ACTIVE_RECORD_FIELD_H_

#include <active_record/type.h>
#include <boost/assign.hpp>
#include <boost/variant.hpp>
#include <boost/assign/list_of.hpp>
using namespace std;
using namespace boost::assign;
using namespace boost;

namespace ActiveRecord {
class Field;
}

ostream& operator<<(ostream& cout, const ActiveRecord::Field& field);

namespace ActiveRecord {

class Field {
  friend ostream& ::operator<<(ostream& cout, const Field& field);

  public:

  Field(const string& name, ActiveRecord::Type::Type type):
    name_(name),
    type_(type) {}

  inline const string& name() const { return name_; }
  inline ActiveRecord::Type::Type type() const { return type_; }

  private:

  string                   name_;
  ActiveRecord::Type::Type type_;
};

typedef assign_detail::generic_list<Field> GenericFieldList;

class Fields: public vector<Field> {
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

  bool has_field(const string& field_name) {
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
  return assign::list_of(Field(name, type));
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_FIELD_H_
