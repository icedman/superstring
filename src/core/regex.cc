#include "regex.h"

#include <stdlib.h>

#include <cstring>

extern "C" {
#include "onigmognu.h"
}

using std::u16string;
using MatchResult = Regex::MatchResult;

const char16_t EMPTY_PATTERN[] = u".{0}";

Regex::Regex() {}

static u16string preprocess_pattern(const char16_t* pattern, uint32_t length) {
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

Regex::Regex(const char16_t* _pattern, uint32_t pattern_length, u16string* error_message, bool ignore_case,
             bool unicode) {
  if (pattern_length == 0) {
    _pattern = EMPTY_PATTERN;
    pattern_length = 4;
  }

  // std::string _spattern = u16string_to_string(_pattern);
  // printf(">%s", _spattern.c_str());

  _regex = NULL;

  // u16string final_pattern = preprocess_pattern(_pattern, pattern_length);
  // const char16_t* pattern_data = final_pattern.c_str();

  regex_t* reg = NULL;
  OnigErrorInfo einfo;

  UChar* pattern = (UChar*)_pattern;

  int opts = ONIG_OPTION_DEFAULT;
  if (ignore_case) {
    opts = opts | ONIG_OPTION_IGNORECASE;
  }

  int r = onig_new(&reg, pattern, pattern + pattern_length * 2, opts, ONIG_ENCODING_UTF16_LE,
                   ONIG_SYNTAX_DEFAULT, &einfo);
  if (r != ONIG_NORMAL) {
    OnigUChar s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(s, r, &einfo);
    *error_message = u"error";
    // fprintf(stderr, "ERROR: %s\n", s);
    // return -1;
  }

  _regex = (void*)reg;
}

Regex::Regex(const u16string& pattern, u16string* error_message, bool ignore_case, bool unicode)
    : Regex(pattern.data(), pattern.size(), error_message, ignore_case, unicode) {}

Regex::Regex(Regex&& other) {
  _regex = other._regex;
  other._regex = NULL;
}

Regex::~Regex() {
  if (_regex) {
    onig_free((regex_t*)_regex);
  }
}

Regex::MatchData::MatchData(const Regex& regex) { _region = (void*)onig_region_new(); }

Regex::MatchData::~MatchData() { onig_region_free((OnigRegion*)_region, 1); }

MatchResult Regex::match(const char16_t* string, size_t length, MatchData& match_data, unsigned options) const {
  MatchResult result{MatchResult::None, 0, 0};

  regex_t* reg = (regex_t*)_regex;

  if (_regex == NULL) return result;

  OnigRegion* region = (OnigRegion*)match_data._region;
  int r;
  unsigned char *start, *range, *end;

  unsigned int onig_options = ONIG_OPTION_NONE;
  if (!(options & MatchOptions::IsEndSearch)) onig_options |= ONIG_OPTION_NOTEOS;
  if (!(options & MatchOptions::IsBeginningOfLine)) onig_options |= ONIG_OPTION_NOTBOL;
  if (!(options & MatchOptions::IsEndOfLine)) onig_options |= ONIG_OPTION_NOTEOL;

  // expensive? << conversions
  // std::string _sstring = u16string_to_string(string);

  UChar* str = (UChar*)string;
  end = str + length;
  start = str;
  range = end;
  r = onig_search(reg, str, end, start, range, region, onig_options);
  if (r >= 0) {
    int i;
    // fprintf(stderr, "match at %d\n", r);
    for (i = 0; i < region->num_regs; i++) {
      // fprintf(stderr, "%d: (%ld-%ld)\n", i, region->beg[i], region->end[i]);
      result.start_offset = region->beg[i]/2;
      result.end_offset = region->end[i]/2;
      result.type = MatchResult::Full;
      break;
    }
    r = 0;
  } else if (r == ONIG_MISMATCH) {
    // fprintf(stderr, "search fail\n");
    result.type = MatchResult::None;
  } else { /* error */
    OnigUChar s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(s, r);
    fprintf(stderr, "ERROR: %s\n", s);
    result.type = MatchResult::Error;
  }

  onig_end();
  return result;
}
