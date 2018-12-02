#ifndef _ACTIVE_RECORD_EXCEPTION_H_
#define _ACTIVE_RECORD_EXCEPTION_H_

#include <stdexcept>
#include <string>
#include <sstream>

namespace ActiveRecord {
class ActiveRecordException;
}

std::ostream& operator<<(
  std::ostream& cout, const ActiveRecord::ActiveRecordException& e
);

namespace ActiveRecord {

class ActiveRecordException: public std::runtime_error {
  friend std::ostream& operator<<(
    std::ostream& cout, const ActiveRecordException& e
  );

  public:

  ActiveRecordException(
    const std::string& message,
    const std::string& file = "",
    long line = -1
  ):
    std::runtime_error(message),
    message_(message),
    file_(file),
    line_(line) {}

  virtual ~ActiveRecordException() throw() {}

  std::string message() const { return message_; }
  std::string file() const { return file_; }
  long line() const { return line_; }

  std::string to_string() const {
    std::stringstream ss;
    if(file_.size() > 0) {
      ss << file_;
      if(line_ > 0) {
        ss << ":" << line_;
      }
      ss << ": ";
    }
    ss << message_;
    return ss.str();
  }

  virtual const char* what() const throw() {
    return message_.c_str();
  }

  private:

  std::string message_;
  std::string file_;
  long line_;
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_EXCEPTION_H_
