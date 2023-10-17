/* -*- mode: c++ -*- */
#ifndef CSVSTREAM_H
#define CSVSTREAM_H
/* csvstream.h
 *
 * Andrew DeOrio <awdeorio@umich.edu>
 *
 * An easy-to-use CSV file parser for C++
 * https://github.com/awdeorio/csvstream
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <exception>

void GUI_Log(const char * format, ...);

// A custom exception type
class csvstream_exception : public std::exception {
public:
  const char * what () const throw () {
    return msg.c_str();
  }
  const std::string msg;
  csvstream_exception(const std::string &msg) : msg(msg) {};
};

// csvstream interface
class csvstream {
public:
  // Constructor from filename
  csvstream(const std::string &filename, char delimiter=',', bool strict=true);

  // Constructor from stream
  csvstream(std::istream &is, char delimiter=',', bool strict=true);

  // Destructor
  ~csvstream();

  // Return true if an error flag on underlying stream is set
  explicit operator bool() const;

  // Return header processed by constructor
  std::vector<std::string> getheader() const;

  // Stream extraction operator reads one row
  csvstream & operator>> (std::map<std::string, std::string>& row);

  // Stream extraction operator reads one row, keeping column order
  csvstream & operator>> (std::vector<std::pair<std::string, std::string> >& row);

private:
  // Filename.  Used for error messages.
  std::string filename;

  // File stream in CSV format, used when library is called with filename ctor
  std::ifstream fin;

  // Stream in CSV format
  std::istream &is;

  // Delimiter between columns
  char delimiter;

  // Strictly enforce the number of values in each row.  Raise an exception if
  // a row contains too many values or too few compared to the header.  When
  // strict=false, ignore extra values and set missing values to empty string.
  bool strict;

  // Line no in file.  Used for error messages
  size_t line_no;

  // Store header column names
  std::vector<std::string> header;

  // Process header, the first line of the file
  void read_header();

  // Disable copying because copying streams is bad!
  csvstream(const csvstream &);
  csvstream & operator= (const csvstream &);
};




#endif
