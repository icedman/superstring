#include "regex.h"
#include <stdlib.h>

using std::u16string;
using MatchResult = Regex::MatchResult;

const char16_t EMPTY_PATTERN[] = u".{0}";

Regex::Regex() {}

static u16string preprocess_pattern(const char16_t *pattern, uint32_t length) {
  u16string result;
  for (unsigned i = 0; i < length;) {
    char16_t c = pattern[i];

    // Replace escape sequences like '\u00cf' with their literal UTF16 value
    if (c == '\\' && i + 1 < length) {
      if (pattern[i + 1] == 'u') {
        if (i + 6 <= length) {
          std::string char_code_string(&pattern[i + 2], &pattern[i + 6]);
          char16_t char_code_value = strtol(char_code_string.data(), nullptr, 16);
          if (char_code_value != 0) {
            result += char_code_value;
            i += 6;
            continue;
          }
        }

        // Replace invalid '\u' escape sequences with the literal characters '\' and 'u'
        result += u"\\\\u";
        i += 2;
        continue;
      } else if (pattern[i + 1] == '\\') {
        // Prevent '\\u' from UTF16 replacement
        result += u"\\\\";
        i += 2;
        continue;
      }
    }

    result += c;
    i++;
  }

  return result;
}


Regex::Regex(const char16_t *pattern, uint32_t pattern_length, u16string *error_message, bool ignore_case, bool unicode) {
  if (pattern_length == 0) {
    pattern = EMPTY_PATTERN;
    pattern_length = 4;
  }

  // u16string final_pattern = preprocess_pattern(pattern, pattern_length);
}

Regex::Regex(const u16string &pattern, u16string *error_message, bool ignore_case, bool unicode)
  : Regex(pattern.data(), pattern.size(), error_message, ignore_case, unicode) {}

Regex::Regex(Regex &&other) {
}

Regex::~Regex() {
}

Regex::MatchData::MatchData(const Regex &regex)
{}

Regex::MatchData::~MatchData() {
}

MatchResult Regex::match(const char16_t *string, size_t length,
                         MatchData &match_data, unsigned options) const {
  MatchResult result{MatchResult::None, 0, 0};

  return result;
}
