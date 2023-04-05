#include "format.h"

#include <iomanip>
#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  // Get the integer part of the division
  int h = seconds / 3600;
  // Get the remainder of the hour computation -> left over seconds and
  // transform into minutes
  int m = (seconds % 3600) / 60;
  // Witht the hours and minutes -> we can compute the remaining seconds
  int s = seconds - (h * 3600) - (m * 60);

  std::ostringstream formatStream;
  // Through the setw we specify minimum space needed for the next number and
  // fill up exceedings with '0'
  formatStream << std::setw(2) << std::setfill('0') << h << ":" << std::setw(2)
               << std::setfill('0') << m << ":" << std::setw(2)
               << std::setfill('0') << s;

  return formatStream.str();
}