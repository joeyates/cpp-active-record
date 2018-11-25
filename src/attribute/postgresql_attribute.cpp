#include <active_record/attribute.h>

#include <c.h>
#include <catalog/pg_type.h>
// ensure CppConcat is defined
#include <climits>

namespace ActiveRecord {

int64 string_to_int64(const char* raw) {
  errno = 0;
  int64 result = strtol(raw, nullptr, 10);

  switch(errno) {
    case 0:
      return result;

    case EINVAL: {
      stringstream error;
      error << "Expected an int, but got '" << raw << "'";
      throw ActiveRecordException(error.str(), __FILE__, __LINE__);
    }

    case ERANGE: {
      stringstream error;
      error << "The value '" << raw << "' is out of range for an integer";
      throw ActiveRecordException(error.str(), __FILE__, __LINE__);
    }

    default: {
      stringstream error;
      error << "Unexpected error " << errno;
      error << " when parsing '" << raw << "' as an integer";
      throw ActiveRecordException(error.str(), __FILE__, __LINE__);
    }
  }
}

double string_to_double(const char* raw) {
  char* end;
  double d = std::strtod(raw, &end);

  if(end == raw) {
    stringstream error;
    error << "Expected a floating point number, but got '" << raw << "'";
    throw ActiveRecordException(error.str(), __FILE__, __LINE__);
  }

  return d;
}

Attribute Attribute::from_field(PGresult* exec_result, int row, int column) {
  Oid pg_type = PQftype(exec_result, column);
  Type::Type type = pg_type_to_ar_type(pg_type);
  char* raw = PQgetvalue(exec_result, row, column);

  // TYPE_LIST
  switch(type) {
    case Type::text:
      return raw;

    case Type::integer:
      return string_to_int64(raw);

    case Type::floating_point:
      return string_to_double(raw);

    case Type::date:
      return Date::parse(raw);

    default: {
      stringstream error;
      error << "Value '" << raw << "' has unhandled data type " << pg_type;
      throw ActiveRecordException(error.str(), __FILE__, __LINE__);
    }
  }
}

// TYPE_LIST
Type::Type Attribute::pg_type_to_ar_type(Oid pg_type) {
  switch(pg_type) {
    case TEXTOID:
    case NAMEOID:
    case VARCHAROID:
      return Type::text;
    case OIDOID:
    case INT2OID:
    case INT4OID:
    case INT8OID:
      return Type::integer;
    case NUMERICOID:
      return Type::floating_point;
    case DATEOID:
      return Type::date;
    default:
      return Type::unknown;
  }
}

} // namespace ActiveRecord
