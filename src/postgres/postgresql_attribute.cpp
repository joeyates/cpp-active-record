#ifdef AR_POSTGRES
#include <active_record/attribute.h>
#include <active_record/conversions.h>

#include <c.h>
#include <catalog/pg_type.h>

namespace ActiveRecord {

Attribute Attribute::from_field(PGresult* exec_result, int row, int column) {
  Oid pg_type = PQftype(exec_result, column);
  Type::Type type = pg_type_to_ar_type(pg_type);
  char* raw = PQgetvalue(exec_result, row, column);

  // TYPE_LIST
  switch(type) {
    case Type::integer: {
      bool found = false;
      int64_t i = string_to_int64(raw, found);
      if(!found) {
        return Attribute();
      }
      return i;
    }

    case Type::text: {
      int is_null = PQgetisnull(exec_result, row, column);
      if(is_null == 1) {
        return Attribute();
      }
      return raw;
    }

    case Type::floating_point: {
      bool found = false;
      double d = string_to_double(raw, found);
      if(!found) {
        return Attribute();
      }
      return d;
    }

    case Type::date: {
      bool valid = Date::is_valid(raw);
      if(!valid) {
        return Attribute();
      }
      return Date::parse(raw);
    }

    default: {
      std::stringstream error;
      error << "Value '" << raw << "' has unhandled data type " << pg_type;
      throw ActiveRecordException(error.str(), __FILE__, __LINE__);
    }
  }
}

// TYPE_LIST
Type::Type Attribute::pg_type_to_ar_type(Oid pg_type) {
  switch(pg_type) {
    case OIDOID:
    case INT2OID:
    case INT4OID:
    case INT8OID:
      return Type::integer;
    case TEXTOID:
    case NAMEOID:
    case VARCHAROID:
      return Type::text;
    case NUMERICOID:
      return Type::floating_point;
    case DATEOID:
      return Type::date;
    default:
      return Type::unknown;
  }
}

} // namespace ActiveRecord

#endif // def AR_POSTGRES
