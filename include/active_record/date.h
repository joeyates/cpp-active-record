#ifndef _ACTIVE_RECORD_DATE_
#define _ACTIVE_RECORD_DATE_

#include <iostream>
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace std;

namespace ActiveRecord {

  class Date : public boost::gregorian::date {
  friend ostream & operator<<( ostream &cout, const Date &date );
 public:
  static Date parse( const string &date ) {
    return Date( boost::gregorian::from_string( date ) );
  }

  Date() : boost::gregorian::date() {}
  Date( boost::gregorian::date date ) : boost::gregorian::date( date ) {}
  Date( const string &date )
    : boost::gregorian::date( boost::gregorian::from_string( date ) ) {}
  Date( int year, int month, int day )
    : boost::gregorian::date( year, month, day ) {}

  string to_string() const {
    return boost::gregorian::to_iso_extended_string( *this );
  }
};

#endif //ndef _ACTIVE_RECORD_DATE_
