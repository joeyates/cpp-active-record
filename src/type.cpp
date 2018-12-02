#include <active_record/type.h>

#include <sstream>

#include <active_record/exception.h>

namespace ActiveRecord {

// TYPE_LIST
Type::Type index_to_type(int index) {
  switch(index) {
    case 0:
      return Type::integer;
    case 1:
      return Type::text;
    case 2:
      return Type::floating_point;
    case 3:
      return Type::date;
    default: {
      std::stringstream message;
      message << "No type has index " << index;
      throw ActiveRecordException(message.str(), __FILE__, __LINE__);
    }
  }
}

// TYPE_LIST
Type::Type to_type(const std::string& type_name) {
  if(type_name == "INTEGER") {
    return Type::integer;
  }

  if(type_name == "TEXT") {
    return Type::text;
  }

  if(type_name == "FLOAT") {
    return Type::floating_point;
  }

  if(type_name == "DATE") {
    return Type::date;
  }

  return Type::unknown;
}

// TYPE_LIST
const char* type_string(const Type::Type& type) {
  if(type == Type::integer) {
    return "INTEGER";
  }

  if(type == Type::text) {
    return "TEXT";
  }

  if(type == Type::floating_point) {
    return "FLOAT";
  }

  if(type == Type::date) {
    return "DATE";
  }

  std::stringstream message;
  message << "Unknown type " << type;
  throw ActiveRecordException(message.str(), __FILE__, __LINE__);
}

} // namespace ActiveRecord
