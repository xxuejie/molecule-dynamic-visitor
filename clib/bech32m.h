#ifndef BECH32M_H
#define BECH32M_H

#include <stdint.h>
#include <stdlib.h>

/*
 * This is a modified bech32m encoder from
 * https://github.com/sipa/bech32/tree/7a7d7ab158db7078a333384e0e918c90dbc42917/ref/c
 * with the following changes:
 *
 * * Length restriction is relaxed for CKB's needs
 * * input & output has been abstracted to support arbitrary length
 * * Only bech32m variant is supported
 * * A single header is used for the whole implementation
 *
 * Note that the original code returns 1 as success, and 0 as error. We have
 * flipped this behavior to suit better with other libraries included here.
 */

#define _BECH32M_BUF_LEN 32

/*
 * In normal cases, an inputter fills buf with data, and length with correct
 * length. Note that length both provides the source buffer length, and also
 * conveys the returned written length. 0 shall be returned in success.
 *
 * In case the inputter does not have any data, inputter shall set length as
 * 0, then return 0 as the return code.
 */
typedef int (*bech32m_inputter_t)(uint8_t *buf, size_t *length, void *context);
typedef int (*bech32m_outputter_t)(const uint8_t *data, size_t length,
                                   void *context);

static uint32_t _bech32m_polymod_step(uint32_t pre) {
  uint8_t b = pre >> 25;
  return ((pre & 0x1FFFFFF) << 5) ^ (-((b >> 0) & 1) & 0x3b6a57b2UL) ^
         (-((b >> 1) & 1) & 0x26508e6dUL) ^ (-((b >> 2) & 1) & 0x1ea119faUL) ^
         (-((b >> 3) & 1) & 0x3d4233ddUL) ^ (-((b >> 4) & 1) & 0x2a1462b3UL);
}

int bech32m_encode(const char *hrp, bech32m_inputter_t inputter,
                   void *inputter_context, bech32m_outputter_t outputter,
                   void *outputter_context) {
  const char *charset = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

  uint32_t chk = 1;
  size_t i = 0;
  while (hrp[i] != 0) {
    int ch = hrp[i];
    if (ch < 33 || ch > 126) {
      return 1;
    }

    if (ch >= 'A' && ch <= 'Z') return 0;
    chk = _bech32m_polymod_step(chk) ^ (ch >> 5);
    ++i;
  }
  chk = _bech32m_polymod_step(chk);
  while (*hrp != 0) {
    chk = _bech32m_polymod_step(chk) ^ (*hrp & 0x1f);
    if (outputter((const uint8_t *)hrp, 1, outputter_context)) {
      return 2;
    }
    hrp++;
  }
  if (outputter((const uint8_t *)"1", 1, outputter_context)) {
    return 3;
  }
  char data[_BECH32M_BUF_LEN];
  size_t data_len;
  do {
    data_len = _BECH32M_BUF_LEN;
    if (inputter((uint8_t *)data, &data_len, inputter_context)) {
      return 4;
    }
    for (i = 0; i < data_len; ++i) {
      if (data[i] >> 5) return 0;
      chk = _bech32m_polymod_step(chk) ^ (data[i]);
      char ch = charset[(size_t)data[i]];
      if (outputter((const uint8_t *)&ch, 1, outputter_context)) {
        return 5;
      }
    }
  } while (data_len > 0);
  for (i = 0; i < 6; ++i) {
    chk = _bech32m_polymod_step(chk);
  }
  chk ^= 0x2bc830a3;
  for (i = 0; i < 6; ++i) {
    char ch = charset[(chk >> ((5 - i) * 5)) & 0x1f];
    if (outputter((const uint8_t *)&ch, 1, outputter_context)) {
      return 6;
    }
  }
  return 0;
}

typedef struct {
  uint32_t buffer_bits;
  uint8_t buffer;
  int end;

  bech32m_inputter_t raw_inputter;
  void *raw_inputter_context;
} bech32m_raw_to_5bits_inputter_context;

void bech32m_initialize_raw_to_5bits_inputter(
    bech32m_raw_to_5bits_inputter_context *context,
    bech32m_inputter_t raw_inputter, void *raw_inputter_context) {
  context->buffer_bits = 0;
  context->buffer = 0;
  context->end = 0;

  context->raw_inputter = raw_inputter;
  context->raw_inputter_context = raw_inputter_context;
}

int bech32m_raw_to_5bits_inputter(uint8_t *buf, size_t *length, void *context) {
  bech32m_raw_to_5bits_inputter_context *c =
      (bech32m_raw_to_5bits_inputter_context *)context;

  if (*length < 4) {
    return 1;
  }
  size_t wrote = 0;
  if (!c->end) {
    size_t input_buf_length = (*length - 2) / 2;
    if (input_buf_length > _BECH32M_BUF_LEN) {
      input_buf_length = _BECH32M_BUF_LEN;
    }
    uint8_t input_buf[_BECH32M_BUF_LEN];

    if (c->raw_inputter(input_buf, &input_buf_length,
                        c->raw_inputter_context)) {
      return 2;
    }
    c->end = (input_buf_length == 0);

    for (size_t i = 0; i < input_buf_length; i++) {
      if (c->buffer_bits >= 5) {
        if (wrote >= *length) {
          return 3;
        }
        buf[wrote++] = (c->buffer & 0b11111000) >> 3;
        c->buffer <<= 5;
        c->buffer_bits -= 5;
      }

      uint8_t from_buffer = c->buffer >> 3;
      uint8_t from_byte = input_buf[i] >> (3 + c->buffer_bits);

      if (wrote >= *length) {
        return 4;
      }
      buf[wrote++] = from_buffer | from_byte;
      c->buffer = input_buf[i] << (5 - c->buffer_bits);
      c->buffer_bits += 3;
    }
  }
  if (c->end) {
    if ((c->buffer_bits >= 5) && (wrote < *length)) {
      buf[wrote++] = (c->buffer & 0b11111000) >> 3;
      c->buffer <<= 5;
      c->buffer_bits -= 5;
    }
    if ((c->buffer_bits != 0) && (wrote < *length)) {
      buf[wrote++] = c->buffer >> 3;
      c->buffer = 0;
      c->buffer_bits = 0;
    }
  }

  *length = wrote;
  return 0;
}

#endif /* BECH32M_H */
