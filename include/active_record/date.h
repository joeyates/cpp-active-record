#ifndef _ACTIVE_RECORD_DATE_
#define _ACTIVE_RECORD_DATE_

#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <active_record/exception.h>

using namespace std;

namespace ActiveRecord {

class Date {
  friend ostream & operator<<( ostream &cout, const Date &date );
 public:
  static Date parse( const string &date ) {
    return Date( date );
  }

  Date() {
    init( 0 );
  }
  Date( time_t t ) {
    init( t );
  }
  Date( const string &date ) {
    init( 0 );
    bool parsed = parse( date, tm_ );
    if( !parsed )
      throw ActiveRecordException( "Date format incorrect", __FILE__, __LINE__ );
  }
  Date( int year, int month, int day ) {
    init( 0 );
    tm_.tm_year = year - 1900;
    tm_.tm_mon  = month - 1;
    tm_.tm_mday = day;
  }

  bool operator==( const Date& other ) const {
    return ( memcmp( &tm_, &other.tm_, sizeof( struct tm ) ) == 0 ) ? true : false;
  }

  int year() const {
    return tm_.tm_year + 1900;
  }
  int month() const {
    return tm_.tm_mon + 1;
  }
  int day() const {
    return tm_.tm_mday;
  }
  string to_string() const {
    stringstream ss;
    ss.fill( '0' );
    ss.width( 0 );
    ss << year() << "-";
    ss.fill( '0' );
    ss.width( 2 );
    ss << month() << "-";
    ss.fill( '0' );
    ss.width( 2 );
    ss << day();
    return ss.str();
  }

 private:
  struct tm tm_;

  void init( time_t t ) {
    memset( &tm_, 0, sizeof( struct tm ) );
    gmtime_r( &t, &tm_ );
  }

  static bool parse( const string &date, struct tm &tm ) {
    int year, month, day;
    int found = sscanf( date.c_str(), "%u-%u-%u", &year, &month, &day );
    if( found != 3 )
      return false;
    if( month < 1 || month > 12 )
      return false;
    if( day < 1 || day > 31 )
      return false;

    tm.tm_year = year - 1900;
    tm.tm_mon  = month - 1;
    tm.tm_mday = day;

    return true;
  }
};

} // namespace ActiveRecord

#endif //ndef _ACTIVE_RECORD_DATE_
