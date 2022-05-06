#ifndef REGEX_H_
#define REGEX_H_

#include <string>

#include "optional.h"

struct BuildRegexResult;

class Regex {
  void *_regex;

 public:
  Regex();
  Regex(const char16_t *, uint32_t, std::u16string *error_message, bool ignore_case = false, bool unicode = false);
  Regex(const std::u16string &, std::u16string *error_message, bool ignore_case = false, bool unicode = false);
  Regex(Regex &&);
  ~Regex();

  class MatchData {
    void *_region;
    friend class Regex;

   public:
    MatchData(const Regex &);
    ~MatchData();
  };

  struct MatchResult {
    enum {
      None,
      Error,
      Partial,
      Full,
    } type;

    size_t start_offset;
    size_t end_offset;
  };

  enum MatchOptions {
    None = 0,
    IsBeginningOfLine = 1,
    IsEndOfLine = 2,
    IsEndSearch = 4,
  };

  MatchResult match(const char16_t *data, size_t length, MatchData &, unsigned options = 0) const;
};

struct BuildRegexResult {
  optional<Regex> regex;
  std::u16string error_message;
};

#endif  // REGX_H_
