#include <active_record/exception.h>

#include <c.h>
#include <sstream>

namespace ActiveRecord {

int64 string_to_int64(const char* raw) {
  errno = 0;
  int64 result = strtol(raw, nullptr, 10);

  switch(errno) {
    case 0:
      return result;

    case EINVAL: {
      std::stringstream error;
      error << "Expected an int, but got '" << raw << "'";
      throw ActiveRecord::ActiveRecordException(
        error.str(), __FILE__, __LINE__
      );
    }

    case ERANGE: {
      std::stringstream error;
      error << "The value '" << raw << "' is out of range for an integer";
      throw ActiveRecord::ActiveRecordException(
        error.str(), __FILE__, __LINE__
      );
    }

    default: {
      std::stringstream error;
      error << "Unexpected error " << errno;
      error << " when parsing '" << raw << "' as an integer";
      throw ActiveRecord::ActiveRecordException(
        error.str(), __FILE__, __LINE__
      );
    }
  }
}

double string_to_double(const char* raw) {
  char* end;
  double d = std::strtod(raw, &end);

  if(end == raw) {
    std::stringstream error;
    error << "Expected a floating point number, but got '" << raw << "'";
    throw ActiveRecord::ActiveRecordException(error.str(), __FILE__, __LINE__);
  }

  return d;
}

} // namespace ActiveRecord
