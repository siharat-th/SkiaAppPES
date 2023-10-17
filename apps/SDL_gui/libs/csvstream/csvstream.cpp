/* -*- mode: c++ -*- */
#include "csvstream.h"

void GUI_Log(const char * format, ...);


///////////////////////////////////////////////////////////////////////////////
// Implementation

// Read and tokenize one line from a stream
static bool read_csv_line(std::istream &is,
                          std::vector<std::string> &data,
                          char delimiter
                          ) {

  // Add entry for first token, start with empty string
  data.clear();
  data.push_back(std::string());

  // Process one character at a time
  char c = '\0';
  enum State {BEGIN, QUOTED, QUOTED_ESCAPED, UNQUOTED, UNQUOTED_ESCAPED, END};
  State state = BEGIN;
  while(is.get(c)) {
    switch (state) {
    case BEGIN:
      // We need this state transition to properly handle cases where nothing
      // is extracted.  Note the intended switch fallthrough.
      state = UNQUOTED;

    case UNQUOTED:
      if (c == '"') {
        // Change states when we see a double quote
        state = QUOTED;
      } else if (c == '\\') { //note this checks for a single backslash char
        state = UNQUOTED_ESCAPED;
        data.back() += c;
      } else if (c == delimiter) {
        // If you see a delimiter, then start a new field with an empty string
        data.push_back("");
      } else if (c == '\n' || c == '\r') {
        // If you see a line ending *and it's not within a quoted token*, stop
        // parsing the line.  Works for UNIX (\n) and OSX (\r) line endings.
        // Consumes the line ending character.
        state = END;
      } else {
        // Append character to current token
        data.back() += c;
      }
      break;

    case UNQUOTED_ESCAPED:
      // If a character is escaped, add it no matter what.
      data.back() += c;
      state = UNQUOTED;
      break;

    case QUOTED:
      if (c == '"') {
        // Change states when we see a double quote
        state = UNQUOTED;
      } else if (c == '\\') {
        state = QUOTED_ESCAPED;
        data.back() += c;
      } else {
        // Append character to current token
        data.back() += c;
      }
      break;

    case QUOTED_ESCAPED:
      // If a character is escaped, add it no matter what.
      data.back() += c;
      state = QUOTED;
      break;

    case END:
      if (c == '\n') {
        // Handle second character of a Windows line ending (\r\n).  Do
        // nothing, only consume the character.
      } else {
        // If this wasn't a Windows line ending, then put character back for
        // the next call to read_csv_line()
        is.unget();
      }

      // We're done with this line, so break out of both the switch and loop.
      goto multilevel_break; //This is a rare example where goto is OK
      break;

    default:
      assert(0);
      return false; //throw state;

    }//switch
  }//while

 multilevel_break:
  // Clear the failbit if we extracted anything.  This is to mimic the behavior
  // of getline(), which will set the eofbit, but *not* the failbit if a partial
  // line is read.
  if (state != BEGIN) is.clear();
    


  // Return status is the underlying stream's status
  return static_cast<bool>(is);
}


csvstream::csvstream(const std::string &filename, char delimiter, bool strict)
  : filename(filename),
    is(fin),
    delimiter(delimiter),
    strict(strict),
    line_no(0) {

  // Open file
  fin.open(filename.c_str());
  if (!fin.is_open()) {
//    throw csvstream_exception("Error opening file: " + filename);
      return;
  }

  // Process header
  read_header();
}



csvstream::csvstream(std::istream &is, char delimiter, bool strict)
  : filename("[no filename]"),
    is(is),
    delimiter(delimiter),
    strict(strict),
    line_no(0) {
  read_header();
}


csvstream::~csvstream() {
  if (fin.is_open()) fin.close();
}


csvstream::operator bool() const {
  return static_cast<bool>(is);
}


std::vector<std::string> csvstream::getheader() const {
  return header;
}


csvstream & csvstream::operator>> (std::map<std::string, std::string>& row) {
  // Clear input row
  row.clear();

  // Read one line from stream, bail out if we're at the end
  std::vector<std::string> data;
  if (!read_csv_line(is, data, delimiter)) return *this;
  line_no += 1;

  // When strict mode is disabled, coerce the length of the data.  If data is
  // larger than header, discard extra values.  If data is smaller than header,
  // pad data with empty strings.
  if (!strict) {
    data.resize(header.size());
  }

  // Check length of data
  if (data.size() != header.size()) {
    auto msg = "Number of items in row does not match header. " +
      filename + ":L" + std::to_string(line_no) + " " +
      "header.size() = " + std::to_string(header.size()) + " " +
      "row.size() = " + std::to_string(data.size()) + " "
      ;
//    throw csvstream_exception(msg);
      return *this;
  }

  // combine data and header into a row object
  for (size_t i=0; i<data.size(); ++i) {
    row[header[i]] = data[i];
  }

  return *this;
}


csvstream & csvstream::operator>> (std::vector<std::pair<std::string, std::string> >& row) {
  // Clear input row
  row.clear();
  row.resize(header.size());

  // Read one line from stream, bail out if we're at the end
  std::vector<std::string> data;
  if (!read_csv_line(is, data, delimiter)) return *this;
  line_no += 1;

  // When strict mode is disabled, coerce the length of the data.  If data is
  // larger than header, discard extra values.  If data is smaller than header,
  // pad data with empty strings.
  if (!strict) {
    data.resize(header.size());
  }

  // Check length of data
  if (row.size() != header.size()) {
    auto msg = "Number of items in row does not match header. " +
      filename + ":L" + std::to_string(line_no) + " " +
      "header.size() = " + std::to_string(header.size()) + " " +
      "row.size() = " + std::to_string(row.size()) + " "
      ;
//    throw csvstream_exception(msg);
      return *this;
  }

  // combine data and header into a row object
  for (size_t i=0; i<data.size(); ++i) {
    row[i] = make_pair(header[i], data[i]);
  }

  return *this;
}


void csvstream::read_header() {
  // read first line, which is the header
  if (!read_csv_line(is, header, delimiter)) {
//    throw csvstream_exception("error reading header");
      return;
  }
    if( header.size() > 0 ) {
        std::string s = header[0];
        const char *sz = s.c_str();
        if( (const unsigned char)sz[0] == 0xEF &&
            (const unsigned char)sz[1] == 0xBB &&
            (const unsigned char)sz[2] == 0xBF ) {
            header[0] = std::string( &sz[3] );
        }
    }
}


