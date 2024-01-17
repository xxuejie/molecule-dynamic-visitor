#ifndef MDP_UTF8_H_
#define MDP_UTF8_H_

/*
 * This is a modified utf8 checker from
 * https://www.cl.cam.ac.uk/~mgk25/ucs/utf8_check.c
 * with the following changes:
 *
 * * Input has been abstracted to support arbitrary length, as a result,
 * string is not '\0' terminated anymore.
 * * A single header is used for the whole implementation
 * * The API is adjusted so that 0 is returned if the string contains full
 * valid UTF-8 code sequences, and non-zero value if error is encountered.
 *
 * The original copyright notice is included as follows
 */

/*
 * The utf8_check() function scans the '\0'-terminated string starting
 * at s. It returns a pointer to the first byte of the first malformed
 * or overlong UTF-8 sequence found, or NULL if the string contains
 * only correct UTF-8. It also spots UTF-8 sequences that could cause
 * trouble if converted to UTF-16, namely surrogate characters
 * (U+D800..U+DFFF) and non-Unicode positions (U+FFFE..U+FFFF). This
 * routine is very likely to find a malformed sequence if the input
 * uses any other encoding than UTF-8. It therefore can be used as a
 * very effective heuristic for distinguishing between UTF-8 and other
 * encodings.
 *
 * I wrote this code mainly as a specification of functionality; there
 * are no doubt performance optimizations possible for certain CPUs.
 *
 * Markus Kuhn <http://www.cl.cam.ac.uk/~mgk25/> -- 2005-03-30
 * License: http://www.cl.cam.ac.uk/~mgk25/short-license.html
 */

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define _UTF8_BUF_LEN 32

typedef int (*utf8_inputter_t)(uint8_t *buf, size_t *length, void *context);
typedef int (*utf8_outputter_t)(const uint8_t *data, size_t length,
                                void *context);

int utf8_check(utf8_inputter_t inputter, void *inputter_context,
               utf8_outputter_t outputter, void *outputter_context) {
  uint8_t buf[_UTF8_BUF_LEN];
  int end = 0;
  size_t filled = 0;

  while ((!end) || filled > 0) {
    if ((!end) && (filled < _UTF8_BUF_LEN)) {
      // Load input data
      size_t available = _UTF8_BUF_LEN - filled;
      int ret = inputter(&buf[filled], &available, inputter_context);
      if (ret != 0) {
        return ret;
      }
      end = (available == 0);
      filled += available;
    }

    size_t processed = 0;
    int waiting_for_data = 0;
    while (processed < filled) {
      if (buf[processed] < 0x80) {
        /* 0xxxxxxx */
        processed++;
      } else if ((buf[processed] & 0xe0) == 0xc0) {
        /* 110XXXXx 10xxxxxx */
        if (processed + 1 < filled) {
          if ((buf[processed + 1] & 0xc0) != 0x80 ||
              (buf[processed] & 0xfe) == 0xc0) /* overlong? */ {
            return 1;
          } else {
            processed += 2;
          }
        } else {
          waiting_for_data = 1;
          break;
        }
      } else if ((buf[processed] & 0xf0) == 0xe0) {
        /* 1110XXXX 10Xxxxxx 10xxxxxx */
        if (processed + 2 < filled) {
          if ((buf[processed + 1] & 0xc0) != 0x80 ||
              (buf[processed + 2] & 0xc0) != 0x80 ||
              (buf[processed] == 0xe0 &&
               (buf[processed + 1] & 0xe0) == 0x80) || /* overlong? */
              (buf[processed] == 0xed &&
               (buf[processed + 1] & 0xe0) == 0xa0) || /* surrogate? */
              (buf[processed] == 0xef && buf[processed + 1] == 0xbf &&
               (buf[processed + 2] & 0xfe) == 0xbe)) /* U+FFFE or U+FFFF? */ {
            return 2;
          } else {
            processed += 3;
          }
        } else {
          waiting_for_data = 1;
          break;
        }
      } else if ((buf[processed] & 0xf8) == 0xf0) {
        /* 11110XXX 10XXxxxx 10xxxxxx 10xxxxxx */
        if (processed + 3 < filled) {
          if ((buf[processed + 1] & 0xc0) != 0x80 ||
              (buf[processed + 2] & 0xc0) != 0x80 ||
              (buf[processed + 3] & 0xc0) != 0x80 ||
              (buf[processed] == 0xf0 &&
               (buf[processed + 1] & 0xf0) == 0x80) || /* overlong? */
              (buf[processed] == 0xf4 && buf[processed + 1] > 0x8f) ||
              buf[processed] > 0xf4) /* > U+10FFFF? */ {
            return 3;
          } else {
            processed += 4;
          }
        } else {
          waiting_for_data = 1;
          break;
        }
      } else {
        return 4;
      }
    }

    if (waiting_for_data && end) {
      return 5;
    }

    int ret = outputter(buf, filled, outputter_context);
    if (ret != 0) {
      return ret;
    }
    memmove(buf, &buf[processed], filled - processed);
    filled -= processed;
  }

  return 0;
}

#endif /* MDP_UTF8_H_ */
