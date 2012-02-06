#include <active_record/attribute.h>

// ensure CppConcat is defined
#include <climits>
#include <c.h>
#include <catalog/pg_type.h>

namespace ActiveRecord {

Attribute Attribute::from_field(PGresult * exec_result, int row, int column) {
  Oid pg_type   = PQftype(exec_result, column);
  Type::Type type = pg_type_to_ar_type(pg_type);
  char * raw = PQgetvalue(exec_result, row, column);
  switch(type) {
    case Type::text:
      return raw;
    case Type::integer:
      return atoi(raw);
    case Type::long_long: {
      // if it's small enough, stuff it in a normal int
      long long ll = atoll(raw);
      if(ll >= INT_MIN and ll <= INT_MAX)
        return (int) ll;
      else
        return ll;
      }
    case Type::floating_point:
      return atof(raw);
    case Type::date:
      return Date::parse(raw);
    default: {
      stringstream error;
      error << "Value '" << raw << "' has unhandled data type " << pg_type;
      throw ActiveRecordException(error.str(), __FILE__, __LINE__);
    }
  }
}

Type::Type Attribute::pg_type_to_ar_type(Oid pg_type) {
  switch(pg_type) {
    case TEXTOID:
    case NAMEOID:
    case VARCHAROID:
      return Type::text;
    case OIDOID:
    case INT2OID:
    case INT4OID:
      return Type::integer;
    case INT8OID:
      return Type::long_long;
    case NUMERICOID:
      return Type::floating_point;
    case DATEOID:
      return Type::date;
    default:
      return Type::unknown;
  }
}

}
