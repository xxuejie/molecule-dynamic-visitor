#ifndef MOLECULE_DYNAMIC_PARSER_H_
#define MOLECULE_DYNAMIC_PARSER_H_

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bech32m.h"
#include "mol2_definitions.h"

/*
 * ----------------------------------------------------------------------
 * APIs
 * ----------------------------------------------------------------------
 */
typedef int (*mdp_text_feeder_t)(const uint8_t *data, size_t length,
                                 void *feeder_context);

typedef struct {
  const char *hrp;
  mol2_cursor_t schema;
  mol2_cursor_t data;
  mdp_text_feeder_t feeder;
  void *feeder_context;
} mdp_context;

/*
 * Given a molecule schema packed as a molecule-formatted Definitions
 * data structure(see schemas/definitions.mol file), this function visits
 * a piece of data generated from the given molecule schema, then generates
 * human readable text.
 *
 * This function fixates the exact format of generated text.
 *
 * To cope with environments with tight memory requirements(such as CKB),
 * the generated text is feeded into a designated mdp_text_feeder_t function.
 */
int mdp_visit(mdp_context context);

/*
 * ----------------------------------------------------------------------
 * Common (Tweakable) Definitions
 * ----------------------------------------------------------------------
 */
#ifndef MDP_ERROR_BASE_CODE
#define MDP_ERROR_BASE_CODE 0xf0
#endif

#ifndef MDP_DEBUG
#define MDP_DEBUG(...) fprintf(stderr, __VA_ARGS__)
#endif

#ifndef MDP_INDENT_VALUE
#define MDP_INDENT_VALUE "  "
#endif

#ifndef MDP_BUFFER_LEN
#define MDP_BUFFER_LEN 1024
#endif

#ifndef MDP_VSNPRINTF
#define MDP_VSNPRINTF vsnprintf
#endif

/*
 * One can tweak this macro for more behaviors, such as saving current
 * __LINE__ macro to a certain place for more debugging hints.
 */
#ifndef MDP_SET_ERROR
#define MDP_SET_ERROR(e)   \
  do {                     \
    inner->last_error = e; \
  } while (0)
#endif

#ifndef MDP_RETURN_ERROR
#define MDP_RETURN_ERROR(e)   \
  do {                        \
    MDP_SET_ERROR(e);         \
    return inner->last_error; \
  } while (0)
#endif

#define MDP_OK 0
#define MDP_ERROR_MOLECULE_ENCODING MDP_ERROR_BASE_CODE
#define MDP_ERROR_FEEDER (MDP_ERROR_BASE_CODE + 1)
#define MDP_ERROR_MOL2_IO (MDP_ERROR_BASE_CODE + 2)
#define MDP_ERROR_SCHEMA_ENCODING (MDP_ERROR_BASE_CODE + 3)
#define MDP_ERROR_SNPRINTF (MDP_ERROR_BASE_CODE + 4)
#define MDP_ERROR_BECH32M (MDP_ERROR_BASE_CODE + 5)

/*
 * ----------------------------------------------------------------------
 * Implementations
 * ----------------------------------------------------------------------
 */
void _mdp_debug_print_cursor(mol2_cursor_t v, const char *prefix) {
  char buf[MDP_BUFFER_LEN];
  uint32_t read = mol2_read_at(&v, (uint8_t *)buf, MDP_BUFFER_LEN - 1);
  buf[read] = '\0';
  MDP_DEBUG("%s%s\n", prefix, buf);
}

int _mdp_cursor_cmp(mol2_cursor_t a, mol2_cursor_t b, int *result) {
  if (a.size != b.size) {
    *result = a.size - b.size;
    return MDP_OK;
  }

  uint8_t buf_a[MDP_BUFFER_LEN];
  uint8_t buf_b[MDP_BUFFER_LEN];
  while (a.size > 0) {
    uint32_t read_a = mol2_read_at(&a, buf_a, MDP_BUFFER_LEN);
    if (read_a == 0) {
      return MDP_ERROR_MOL2_IO;
    }
    uint32_t read_b = mol2_read_at(&b, buf_b, MDP_BUFFER_LEN);
    if (read_b == 0) {
      return MDP_ERROR_MOL2_IO;
    }
    if (read_a != read_b) {
      MDP_DEBUG("Cursor cmp reads different length of data from 2 cursors!\n");
      return MDP_ERROR_MOL2_IO;
    }

    int ret = memcmp(buf_a, buf_b, read_a);
    if (ret != 0) {
      *result = ret;
      return MDP_OK;
    }

    mol2_add_offset(&a, read_a);
    mol2_sub_size(&a, read_a);
    mol2_add_offset(&b, read_b);
    mol2_sub_size(&b, read_b);
  }

  *result = 0;
  return MDP_OK;
}

int _mdp_cursor_s_cmp(mol2_cursor_t a, const char *s, int *result) {
  mol2_cursor_t b = mol2_make_cursor_from_memory(s, strlen(s));
  return _mdp_cursor_cmp(a, b, result);
}

int _mdp_find_union(struct UnionDefinitionType *t, mol2_num_t union_id,
                    struct UnionPairType *out) {
  struct UnionPairVecType vec = t->t->items(t);
  for (uint32_t i = 0; i < vec.t->len(&vec); i++) {
    bool found = false;
    struct UnionPairType pair = vec.t->get(&vec, i, &found);
    if (!found) {
      MDP_DEBUG(
          "Union definition has %u entries but accessing entry %u results in "
          "failure\n",
          vec.t->len(&vec), i);
      return MDP_ERROR_SCHEMA_ENCODING;
    }
    uint64_t id = pair.t->id(&pair);
    if (id == (uint64_t)union_id) {
      *out = pair;
      return MDP_OK;
    }
  }
  MDP_DEBUG("Cannot find union variant with ID %u\n", union_id);
  return MDP_ERROR_MOLECULE_ENCODING;
}

typedef struct {
  mdp_context *context;
  struct DefinitionsType defs;
  size_t indent_levels;
  int last_error;
} _mdp_inner;

int _mdp_send_cursor_to_feeder(_mdp_inner *inner, mol2_cursor_t c) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  uint8_t buf[MDP_BUFFER_LEN];
  while (c.size > 0) {
    uint32_t read = mol2_read_at(&c, buf, MDP_BUFFER_LEN);
    if (read == 0) {
      MDP_RETURN_ERROR(MDP_ERROR_MOL2_IO);
    }
    int ret = inner->context->feeder(buf, (size_t)read,
                                     inner->context->feeder_context);
    if (ret != 0) {
      MDP_DEBUG("Feeder error when sending a full cursor: %d\n", ret);
      MDP_RETURN_ERROR(MDP_ERROR_FEEDER);
      return inner->last_error;
    }
    mol2_add_offset(&c, read);
    mol2_sub_size(&c, read);
  }
  return inner->last_error;
}

int _mdp_send_literal(_mdp_inner *inner, const char *literal) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  int ret = inner->context->feeder((const uint8_t *)literal, strlen(literal),
                                   inner->context->feeder_context);
  if (ret != 0) {
    MDP_DEBUG("Feeder error when sending literal %s: %d\n", literal, ret);
    MDP_SET_ERROR(MDP_ERROR_FEEDER);
  }
  return inner->last_error;
}

int _mdp_send_newline(_mdp_inner *inner) {
  return _mdp_send_literal(inner, "\n");
}

int _mdp_send_indents(_mdp_inner *inner) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  for (size_t i = 0; i < inner->indent_levels; i++) {
    _mdp_send_literal(inner, MDP_INDENT_VALUE);
  }
  return inner->last_error;
}

int _mdp_send_printf(_mdp_inner *inner, const char *format, ...) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  char buf[MDP_BUFFER_LEN];
  va_list va;
  va_start(va, format);
  int ret = MDP_VSNPRINTF(buf, MDP_BUFFER_LEN, format, va);
  va_end(va);
  if (ret <= 0 || ret > MDP_BUFFER_LEN) {
    MDP_DEBUG("sprintf error: %d\n", ret);
    MDP_RETURN_ERROR(MDP_ERROR_SNPRINTF);
  }
  ret = inner->context->feeder((const uint8_t *)buf, (size_t)ret,
                               inner->context->feeder_context);
  if (ret != 0) {
    MDP_DEBUG("Feeder error when sending printf text %s: %d\n", buf, ret);
    MDP_RETURN_ERROR(MDP_ERROR_FEEDER);
  }
  return inner->last_error;
}

int _mdp_send_raw_bytes(_mdp_inner *inner, mol2_cursor_t value) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  mol2_num_t item_count = value.size;
  for (mol2_num_t i = 0; i < item_count; i++) {
    if (i % 8 == 0) {
      _mdp_send_indents(inner);
    }

    uint8_t c;
    if (mol2_read_at(&value, &c, 1) != 1) {
      MDP_DEBUG("Reading a single byte from cursor results in error!\n");
      MDP_RETURN_ERROR(MDP_ERROR_MOL2_IO);
    }
    mol2_add_offset(&value, 1);
    mol2_sub_size(&value, 1);

    _mdp_send_printf(inner, "0x%02x", c);
    if (i != item_count - 1) {
      _mdp_send_literal(inner, ", ");
    }
    if ((i + 1) % 8 == 0) {
      _mdp_send_literal(inner, "\n");
    }
  }
  if (item_count % 8 != 0) {
    _mdp_send_literal(inner, "\n");
  }

  return inner->last_error;
}

typedef struct {
  const mol2_cursor_t *cursors;
  size_t cursor_count;

  size_t current_cursor;
  uint32_t current_offset;
} cursors_bech32m_inputter_context;

void cursors_bech32m_inputter_context_initialize(
    cursors_bech32m_inputter_context *context, const mol2_cursor_t *cursors,
    size_t cursor_count) {
  context->cursors = cursors;
  context->cursor_count = cursor_count;
  context->current_cursor = 0;
  context->current_offset = 0;
}

int cursors_bech32m_inputter(uint8_t *buf, size_t *length, void *context) {
  cursors_bech32m_inputter_context *c =
      (cursors_bech32m_inputter_context *)context;

  size_t wrote = 0;
  while (wrote < *length) {
    if (c->current_cursor < c->cursor_count) {
      if (c->current_offset < c->cursors[c->current_cursor].size) {
        uint32_t available_data =
            c->cursors[c->current_cursor].size - c->current_offset;
        uint32_t available_space = (uint32_t)(*length - wrote);
        if (available_space > available_data) {
          available_space = available_data;
        }
        mol2_cursor_t target_cursor = c->cursors[c->current_cursor];
        mol2_add_offset(&target_cursor, c->current_offset);
        target_cursor.size = available_space;
        uint32_t read =
            mol2_read_at(&target_cursor, &buf[wrote], available_space);
        if (read != available_space) {
          MDP_DEBUG(
              "Cursor inputter got less data than requested, actual: %u, "
              "expected: %u!",
              read, available_space);
          return MDP_ERROR_MOL2_IO;
        }
        wrote += read;
        c->current_offset += read;
      } else {
        // Switch to next cursor
        c->current_cursor++;
        c->current_offset = 0;
      }
    } else {
      // Running out of data to use
      break;
    }
  }
  *length = wrote;
  return 0;
}

mol2_data_source_t _mdp_make_memory_source(const void *memory, uint32_t size) {
  mol2_data_source_t s_data_source = {0};

  s_data_source.read = mol2_source_memory;
  s_data_source.total_size = size;
  s_data_source.args[0] = (uintptr_t)memory;
  s_data_source.args[1] = (uintptr_t)size;

  s_data_source.cache_size = 0;
  s_data_source.start_point = 0;
  s_data_source.total_size = size;
  s_data_source.max_cache_size = MIN_CACHE_SIZE;
  return s_data_source;
}

mol2_cursor_t _mdp_cursor_from_source(mol2_data_source_t *source) {
  mol2_cursor_t cur;
  cur.offset = 0;
  cur.size = source->total_size;
  cur.data_source = source;
  return cur;
}

int _mdp_visit_subtype(_mdp_inner *inner, mol2_cursor_t value,
                       mol2_cursor_t sub_type_name, mol2_num_t *consumed_size);

int _mdp_visit_option(_mdp_inner *inner, mol2_cursor_t value,
                      struct OptionDefinitionType *t,
                      mol2_num_t *consumed_size) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  _mdp_send_indents(inner);
  _mdp_send_cursor_to_feeder(inner, t->t->name(t));
  _mdp_send_literal(inner, "(option):");

  if (value.size > 0) {
    /* Some */
    mol2_cursor_t sub_type = t->t->item(t);
    _mdp_send_newline(inner);
    inner->indent_levels++;
    _mdp_visit_subtype(inner, value, sub_type, consumed_size);
    inner->indent_levels--;
  } else {
    /* None */
    _mdp_send_literal(inner, " None");
    *consumed_size = 0;
  }
  return inner->last_error;
}

int _mdp_visit_union(_mdp_inner *inner, mol2_cursor_t value,
                     struct UnionDefinitionType *t, mol2_num_t *consumed_size) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  if (value.size < 4) {
    MDP_DEBUG(
        "Union requires at least 4 bytes for ID but the value only has length: "
        "%u\n",
        value.size);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }
  mol2_num_t union_id = mol2_unpack_number(&value);
  struct UnionPairType pair;
  int ret = _mdp_find_union(t, union_id, &pair);
  if (ret != MDP_OK) {
    MDP_RETURN_ERROR(ret);
  }

  _mdp_send_indents(inner);
  _mdp_send_cursor_to_feeder(inner, t->t->name(t));

  {
    // handle Address builtin type
    int match = 1;
    int ret = _mdp_cursor_s_cmp(t->t->name(t), "Address", &match);
    if (ret != 0) {
      MDP_RETURN_ERROR(ret);
    }
    if (match == 0) {
      if (union_id != 0) {
        MDP_DEBUG("Address type only supports Script variant for now");
        MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
      }
      // Now we can simply treat the content of the cursor as a table
      // of 3 items.
      mol2_cursor_t script_table_value = value;
      mol2_add_offset(&script_table_value, 4);
      mol2_sub_size(&script_table_value, 4);

      mol2_num_t full_size = mol2_unpack_number(&script_table_value);
      mol2_cursor_t code_hash =
          mol2_table_slice_by_index(&script_table_value, 0);
      if (code_hash.size != 32) {
        MDP_DEBUG("Invalid code hash in address type!");
        MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
      }
      mol2_cursor_t hash_type =
          mol2_table_slice_by_index(&script_table_value, 1);
      if (hash_type.size != 1) {
        MDP_DEBUG("Invalid hash type in address type!");
        MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
      }
      mol2_cursor_t args = mol2_table_slice_by_index(&script_table_value, 2);
      if (args.size < 4) {
        MDP_DEBUG("Invalid args in address type!");
        MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
      }
      mol2_num_t args_length = mol2_unpack_number(&args);
      if (args.size != args_length + 4) {
        MDP_DEBUG("Invalid args length in address type!");
        MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
      }
      if (code_hash.size + hash_type.size + args.size + 16 != full_size) {
        MDP_DEBUG("Invalid address type!");
        MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
      }
      mol2_cursor_t actual_args = args;
      mol2_add_offset(&actual_args, 4);
      mol2_sub_size(&actual_args, 4);

      // Actual visit CKB address
      _mdp_send_literal(inner, ": ");

      mol2_data_source_t index_source = _mdp_make_memory_source("\0", 1);
      mol2_cursor_t index_cursor = _mdp_cursor_from_source(&index_source);
      mol2_cursor_t cursors[4] = {index_cursor, code_hash, hash_type, args};
      cursors_bech32m_inputter_context inputter;
      cursors_bech32m_inputter_context_initialize(&inputter, cursors, 4);

      bech32m_raw_to_5bits_inputter_context inputter2;
      bech32m_initialize_raw_to_5bits_inputter(
          &inputter2, cursors_bech32m_inputter, &inputter);

      int ret = bech32m_encode(
          inner->context->hrp, bech32m_raw_to_5bits_inputter, &inputter2,
          inner->context->feeder, inner->context->feeder_context);
      if (ret != 0) {
        MDP_DEBUG("bech32m encoding process throws an error: %d!", ret);
        MDP_RETURN_ERROR(MDP_ERROR_BECH32M);
      }

      *consumed_size = full_size + 4;
      return inner->last_error;
    }
  }

  _mdp_send_literal(inner, "(variant ");
  mol2_cursor_t subtype = pair.t->typ(&pair);
  _mdp_send_cursor_to_feeder(inner, subtype);
  _mdp_send_printf(inner, ", id = %u):\n", union_id);

  mol2_cursor_t value2 = value;
  mol2_add_offset(&value2, 4);
  mol2_sub_size(&value2, 4);

  inner->indent_levels++;
  mol2_num_t inner_consumed_size = 0;
  ret = _mdp_visit_subtype(inner, value2, subtype, &inner_consumed_size);
  if (ret != MDP_OK) {
    MDP_RETURN_ERROR(ret);
  }
  *consumed_size = inner_consumed_size + 4;
  inner->indent_levels--;

  return inner->last_error;
}

int _mdp_visit_array(_mdp_inner *inner, mol2_cursor_t value,
                     struct ArrayDefinitionType *t, mol2_num_t *consumed_size) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  uint64_t item_count64 = t->t->item_count(t);
  if (item_count64 > 0xFFFFFFFF) {
    MDP_DEBUG("Item count %ld is too large!\n", item_count64);
    MDP_RETURN_ERROR(MDP_ERROR_SCHEMA_ENCODING);
  }
  mol2_num_t item_count = (mol2_num_t)item_count64;
  if ((value.size % item_count) != 0) {
    MDP_DEBUG(
        "Array should have %u items, but the length %u cannot be divided by "
        "item count\n",
        item_count, value.size);
    MDP_RETURN_ERROR(MDP_ERROR_SCHEMA_ENCODING);
  }

  _mdp_send_indents(inner);
  _mdp_send_cursor_to_feeder(inner, t->t->name(t));

  {
    // handle builtin types here
    int match = 1;
    int ret = _mdp_cursor_s_cmp(t->t->name(t), "Byte32", &match);
    if (ret != 0) {
      MDP_RETURN_ERROR(ret);
    }
    if (match == 0) {
      if (item_count != 32) {
        MDP_DEBUG(
            "Byte32 must be an array of 32 bytes, but the schema differs");
        MDP_RETURN_ERROR(MDP_ERROR_SCHEMA_ENCODING);
      }
      uint8_t data[32];
      if (mol2_read_at(&value, data, 32) != 32) {
        MDP_DEBUG("Reading 32 bytes from cursor results in error!\n");
        MDP_RETURN_ERROR(MDP_ERROR_MOL2_IO);
      }
      _mdp_send_literal(inner, ": 0x");
      for (int i = 0; i < 8; i++) {
        _mdp_send_printf(inner, "%02x%02x%02x%02x", data[i * 4],
                         data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]);
      }

      *consumed_size = 32;
      return inner->last_error;
    }
  }

  // Sub-type is not a builtin one, visit its content recursively
  _mdp_send_printf(inner, "(array, len = %u): [\n", item_count);
  inner->indent_levels++;
  mol2_cursor_t subtype = t->t->item(t);
  int match = 1;
  int ret = _mdp_cursor_s_cmp(subtype, "byte", &match);
  if (ret != 0) {
    MDP_RETURN_ERROR(ret);
  }
  if (match == 0) {
    if (value.size < item_count) {
      MDP_DEBUG("Byte array of %u items has invalid length %u!\n", item_count,
                value.size);
      MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
    }

    mol2_cursor_t value2 = value;
    value2.size = item_count;
    _mdp_send_raw_bytes(inner, value2);
    *consumed_size = item_count;
  } else {
    mol2_num_t total_consumed = 0;
    for (mol2_num_t i = 0; i < item_count; i++) {
      mol2_cursor_t value2 = value;
      mol2_add_offset(&value2, total_consumed);
      mol2_sub_size(&value2, total_consumed);

      mol2_num_t current_consumed = 0;
      ret = _mdp_visit_subtype(inner, value2, subtype, &current_consumed);
      if (ret != 0) {
        MDP_RETURN_ERROR(ret);
      }
      if (current_consumed > value2.size) {
        MDP_DEBUG(
            "Array item %u consumed %u bytes but buffer only has %u bytes\n", i,
            current_consumed, value2.size);
        MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
      }
      total_consumed += current_consumed;

      if (i != item_count - 1) {
        _mdp_send_literal(inner, ",");
      }
      _mdp_send_literal(inner, "\n");
    }
    *consumed_size = total_consumed;
  }
  inner->indent_levels--;

  _mdp_send_indents(inner);
  _mdp_send_literal(inner, "]");

  return inner->last_error;
}

int _mdp_visit_struct(_mdp_inner *inner, mol2_cursor_t value,
                      struct StructDefinitionType *t,
                      mol2_num_t *consumed_size) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  _mdp_send_indents(inner);
  _mdp_send_cursor_to_feeder(inner, t->t->name(t));
  _mdp_send_literal(inner, "(struct):\n");

  inner->indent_levels++;
  mol2_num_t total_consumed = 0;
  struct FieldPairVecType fields = t->t->fields(t);
  for (mol2_num_t i = 0; i < fields.t->len(&fields); i++) {
    bool found = false;
    struct FieldPairType field = fields.t->get(&fields, i, &found);
    if (!found) {
      MDP_DEBUG(
          "Struct definition has %u entries but accessing entry %u results in "
          "failure\n",
          fields.t->len(&fields), i);
      MDP_RETURN_ERROR(MDP_ERROR_SCHEMA_ENCODING);
    }
    _mdp_send_indents(inner);
    _mdp_send_cursor_to_feeder(inner, field.t->name(&field));
    _mdp_send_literal(inner, ":\n");

    inner->indent_levels++;
    mol2_cursor_t value2 = value;
    mol2_add_offset(&value2, total_consumed);
    mol2_sub_size(&value2, total_consumed);

    mol2_num_t current_consumed = 0;
    int ret = _mdp_visit_subtype(inner, value2, field.t->typ(&field),
                                 &current_consumed);
    if (ret != 0) {
      MDP_RETURN_ERROR(ret);
    }
    if (current_consumed > value2.size) {
      MDP_DEBUG(
          "Struct item #%u consumed %u bytes but buffer only has %u bytes\n", i,
          current_consumed, value2.size);
      MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
    }
    total_consumed += current_consumed;
    inner->indent_levels--;
  }
  inner->indent_levels--;

  *consumed_size = total_consumed;

  return inner->last_error;
}

int _mdp_visit_fixvec(_mdp_inner *inner, mol2_cursor_t value,
                      struct FixvecDefinitionType *t,
                      mol2_num_t *consumed_size) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  if (value.size < 4) {
    MDP_DEBUG(
        "Fixvec requires at least 4 bytes for item count but the value only "
        "has length: "
        "%u\n",
        value.size);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }
  mol2_num_t item_count = mol2_unpack_number(&value);

  _mdp_send_indents(inner);
  _mdp_send_cursor_to_feeder(inner, t->t->name(t));
  _mdp_send_printf(inner, "(fixvec, len = %u): [\n", item_count);

  inner->indent_levels++;
  mol2_cursor_t subtype = t->t->item(t);
  int match = 1;
  int ret = _mdp_cursor_s_cmp(subtype, "byte", &match);
  if (ret != 0) {
    MDP_RETURN_ERROR(ret);
  }
  if (match == 0) {
    if (value.size < item_count + 4) {
      MDP_DEBUG("Byte vec of %u items has invalid length %u!\n", item_count,
                value.size);
      MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
    }

    mol2_cursor_t value2 = value;
    mol2_add_offset(&value2, 4);
    value2.size = item_count;
    _mdp_send_raw_bytes(inner, value2);
    *consumed_size = item_count + 4;
  } else {
    mol2_num_t total_consumed = 4;
    for (mol2_num_t i = 0; i < item_count; i++) {
      mol2_cursor_t value2 = value;
      mol2_add_offset(&value2, total_consumed);
      mol2_sub_size(&value2, total_consumed);

      mol2_num_t current_consumed = 0;
      int ret = _mdp_visit_subtype(inner, value2, subtype, &current_consumed);
      if (ret != 0) {
        MDP_RETURN_ERROR(ret);
      }
      if (current_consumed > value2.size) {
        MDP_DEBUG(
            "Fixvec item %u consumed %u bytes but buffer only has %u bytes\n",
            i, current_consumed, value2.size);
        MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
      }
      total_consumed += current_consumed;

      if (i != item_count - 1) {
        _mdp_send_literal(inner, ",");
      }
      _mdp_send_literal(inner, "\n");
    }
    *consumed_size = total_consumed;
  }
  inner->indent_levels--;

  _mdp_send_indents(inner);
  _mdp_send_literal(inner, "]");

  return inner->last_error;
}

int _mdp_visit_dynvec(_mdp_inner *inner, mol2_cursor_t value,
                      struct DynvecDefinitionType *t,
                      mol2_num_t *consumed_size) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  if (value.size < 4) {
    MDP_DEBUG(
        "Dynvec requires at least 4 bytes for full size but the value only "
        "has length: "
        "%u\n",
        value.size);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }
  mol2_num_t full_size = mol2_unpack_number(&value);
  if (full_size > value.size) {
    MDP_DEBUG("Dynvec requires %u bytes but buffer only has %u bytes!\n",
              full_size, value.size);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }

  if (full_size == 4) {
    // Empty vec
    *consumed_size = 4;
    return inner->last_error;
  }
  if (full_size < 8) {
    MDP_DEBUG(
        "Non-empty dynvec requires an offset at least, but length %u is not "
        "enough!\n",
        full_size);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }

  mol2_cursor_t tvalue = value;
  mol2_add_offset(&tvalue, 4);
  mol2_num_t first_offset = mol2_unpack_number(&tvalue);
  if ((first_offset % 4) != 0 || first_offset < 8) {
    MDP_DEBUG("Invalid dynvec first offset: %u!\n", first_offset);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }

  mol2_num_t item_count = first_offset / 4 - 1;
  if (value.size < 4 * (item_count + 1)) {
    MDP_DEBUG(
        "A dynvec of %u items requires minimal %u bytes, but actual length is "
        "%u!\n",
        item_count, 4 * (item_count + 1), value.size);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }

  _mdp_send_indents(inner);
  _mdp_send_cursor_to_feeder(inner, t->t->name(t));
  _mdp_send_printf(inner, "(dynvec, len = %u): [\n", item_count);

  inner->indent_levels++;
  mol2_num_t total_consumed = first_offset;
  mol2_cursor_t subtype = t->t->item(t);
  for (mol2_num_t i = 0; i < item_count; i++) {
    mol2_num_t end;
    if (i < item_count - 1) {
      tvalue = value;
      mol2_add_offset(&tvalue, 4 + 4 * (i + 1));
      end = mol2_unpack_number(&tvalue);
    } else {
      end = full_size;
    }
    if (end < total_consumed || end > value.size) {
      MDP_DEBUG("Dynvec item %u has invalid offset: (%u, %u)!\n", i,
                total_consumed, end);
      MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
    }

    mol2_cursor_t value2 = value;
    mol2_add_offset(&value2, total_consumed);
    value2.size = end - total_consumed;

    mol2_num_t current_consumed = 0;
    int ret = _mdp_visit_subtype(inner, value2, subtype, &current_consumed);
    if (ret != 0) {
      MDP_RETURN_ERROR(ret);
    }
    if (current_consumed != value2.size) {
      MDP_DEBUG(
          "Dynvec item %u consumed incorrect bytes, actual: %u, expected: %u\n",
          i, current_consumed, value2.size);
      MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
    }
    total_consumed += current_consumed;

    if (i != item_count - 1) {
      _mdp_send_literal(inner, ",");
    }
    _mdp_send_literal(inner, "\n");
  }
  if (total_consumed != full_size) {
    MDP_DEBUG("Dynvec's full size is %u but only consumed %u bytes!\n",
              full_size, total_consumed);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }

  *consumed_size = total_consumed;
  inner->indent_levels--;

  _mdp_send_indents(inner);
  _mdp_send_literal(inner, "]");

  return inner->last_error;
}

int _mdp_visit_table(_mdp_inner *inner, mol2_cursor_t value,
                     struct TableDefinitionType *t, mol2_num_t *consumed_size) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  if (value.size < 8) {
    MDP_DEBUG(
        "Table requires at least 8 bytes for full size but the value only "
        "has length: "
        "%u\n",
        value.size);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }
  mol2_num_t full_size = mol2_unpack_number(&value);
  if (full_size > value.size) {
    MDP_DEBUG("Table requires %u bytes but buffer only has %u bytes!\n",
              full_size, value.size);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }

  if (full_size < 8) {
    MDP_DEBUG(
        "Table requires an offset at least, but length %u is not "
        "enough!\n",
        full_size);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }

  mol2_cursor_t tvalue = value;
  mol2_add_offset(&tvalue, 4);
  mol2_num_t first_offset = mol2_unpack_number(&tvalue);
  if ((first_offset % 4) != 0 || first_offset < 8) {
    MDP_DEBUG("Invalid table first offset: %u!\n", first_offset);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }

  mol2_num_t field_count = first_offset / 4 - 1;
  if (value.size < 4 * (field_count + 1)) {
    MDP_DEBUG(
        "A table of %u fields requires minimal %u bytes, but actual length is "
        "%u!\n",
        field_count, 4 * (field_count + 1), value.size);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }

  struct FieldPairVecType fields = t->t->fields(t);
  if (field_count != fields.t->len(&fields)) {
    /* TODO: do we need compatible support? */
    MDP_DEBUG("Table requires %u fields, but actual data has %u fields!\n",
              fields.t->len(&fields), field_count);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }

  _mdp_send_indents(inner);
  _mdp_send_cursor_to_feeder(inner, t->t->name(t));
  _mdp_send_literal(inner, "(table): {\n");

  inner->indent_levels++;
  mol2_num_t total_consumed = first_offset;
  for (mol2_num_t i = 0; i < field_count; i++) {
    mol2_num_t end;
    if (i < field_count - 1) {
      tvalue = value;
      mol2_add_offset(&tvalue, 4 + 4 * (i + 1));
      end = mol2_unpack_number(&tvalue);
    } else {
      end = full_size;
    }
    if (end < total_consumed || end > value.size) {
      MDP_DEBUG("Table field %u has invalid offset: (%u, %u)!\n", i,
                total_consumed, end);
      MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
    }

    mol2_cursor_t value2 = value;
    mol2_add_offset(&value2, total_consumed);
    value2.size = end - total_consumed;

    bool found = false;
    struct FieldPairType field = fields.t->get(&fields, i, &found);
    if (!found) {
      MDP_DEBUG(
          "Table definition has %u entries but accessing entry %u results in "
          "failure\n",
          fields.t->len(&fields), i);
      MDP_RETURN_ERROR(MDP_ERROR_SCHEMA_ENCODING);
    }
    _mdp_send_indents(inner);
    _mdp_send_cursor_to_feeder(inner, field.t->name(&field));
    _mdp_send_literal(inner, ":\n");

    inner->indent_levels++;
    mol2_num_t current_consumed = 0;
    int ret = _mdp_visit_subtype(inner, value2, field.t->typ(&field),
                                 &current_consumed);
    if (ret != 0) {
      MDP_RETURN_ERROR(ret);
    }
    if (current_consumed != value2.size) {
      MDP_DEBUG(
          "Dynvec item %u consumed incorrect bytes, actual: %u, expected: %u\n",
          i, current_consumed, value2.size);
      MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
    }
    total_consumed += current_consumed;

    if (i != field_count - 1) {
      _mdp_send_literal(inner, ",");
    }
    _mdp_send_literal(inner, "\n");
    inner->indent_levels--;
  }
  if (total_consumed != full_size) {
    MDP_DEBUG("Dynvec's full size is %u but only consumed %u bytes!\n",
              full_size, total_consumed);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
  }

  *consumed_size = total_consumed;
  inner->indent_levels--;

  _mdp_send_indents(inner);
  _mdp_send_literal(inner, "}");

  return inner->last_error;
}

int _mdp_visit_subtype(_mdp_inner *inner, mol2_cursor_t value,
                       mol2_cursor_t sub_type_name, mol2_num_t *consumed_size) {
  if (inner->last_error != MDP_OK) {
    return inner->last_error;
  }

  int match = 1;
  int ret = _mdp_cursor_s_cmp(sub_type_name, "byte", &match);
  if (ret != 0) {
    MDP_RETURN_ERROR(ret);
  }
  if (match == 0) {
    _mdp_send_indents(inner);
    uint8_t c;
    if (mol2_read_at(&value, &c, 1) != 1) {
      MDP_DEBUG("Reading a single byte from cursor results in error!\n");
      MDP_RETURN_ERROR(MDP_ERROR_MOL2_IO);
    }
    mol2_add_offset(&value, 1);
    mol2_sub_size(&value, 1);

    _mdp_send_printf(inner, "0x%x", c);
    *consumed_size = 1;
    return inner->last_error;
  }

  struct DefinitionVecType vec = inner->defs.t->definitions(&inner->defs);
  for (uint32_t i = 0; i < vec.t->len(&vec); i++) {
    bool found = false;
    struct DefinitionType d = vec.t->get(&vec, i, &found);
    if (!found) {
      MDP_DEBUG(
          "Definitions have %u entries but accessing entry %u results in "
          "failure\n",
          vec.t->len(&vec), i);
      MDP_RETURN_ERROR(MDP_ERROR_SCHEMA_ENCODING);
    }

    switch (d.t->item_id(&d)) {
      case 0: {
        struct OptionDefinitionType t = d.t->as_OptionDefinition(&d);
        int match = 1;
        int ret = _mdp_cursor_cmp(sub_type_name, t.t->name(&t), &match);
        if (ret != MDP_OK) {
          MDP_RETURN_ERROR(ret);
        }
        if (match == 0) {
          return _mdp_visit_option(inner, value, &t, consumed_size);
        }
      } break;
      case 1: {
        struct UnionDefinitionType t = d.t->as_UnionDefinition(&d);
        int match = 1;
        int ret = _mdp_cursor_cmp(sub_type_name, t.t->name(&t), &match);
        if (ret != MDP_OK) {
          MDP_RETURN_ERROR(ret);
        }
        if (match == 0) {
          return _mdp_visit_union(inner, value, &t, consumed_size);
        }
      } break;
      case 2: {
        struct ArrayDefinitionType t = d.t->as_ArrayDefinition(&d);
        int match = 1;
        int ret = _mdp_cursor_cmp(sub_type_name, t.t->name(&t), &match);
        if (ret != MDP_OK) {
          MDP_RETURN_ERROR(ret);
        }
        if (match == 0) {
          return _mdp_visit_array(inner, value, &t, consumed_size);
        }
      } break;
      case 3: {
        struct StructDefinitionType t = d.t->as_StructDefinition(&d);
        int match = 1;
        int ret = _mdp_cursor_cmp(sub_type_name, t.t->name(&t), &match);
        if (ret != MDP_OK) {
          MDP_RETURN_ERROR(ret);
        }
        if (match == 0) {
          return _mdp_visit_struct(inner, value, &t, consumed_size);
        }
      } break;
      case 4: {
        struct FixvecDefinitionType t = d.t->as_FixvecDefinition(&d);
        int match = 1;
        int ret = _mdp_cursor_cmp(sub_type_name, t.t->name(&t), &match);
        if (ret != MDP_OK) {
          MDP_RETURN_ERROR(ret);
        }
        if (match == 0) {
          return _mdp_visit_fixvec(inner, value, &t, consumed_size);
        }
      } break;
      case 5: {
        struct DynvecDefinitionType t = d.t->as_DynvecDefinition(&d);
        int match = 1;
        int ret = _mdp_cursor_cmp(sub_type_name, t.t->name(&t), &match);
        if (ret != MDP_OK) {
          MDP_RETURN_ERROR(ret);
        }
        if (match == 0) {
          return _mdp_visit_dynvec(inner, value, &t, consumed_size);
        }
      } break;
      case 6: {
        struct TableDefinitionType t = d.t->as_TableDefinition(&d);
        int match = 1;
        int ret = _mdp_cursor_cmp(sub_type_name, t.t->name(&t), &match);
        if (ret != MDP_OK) {
          MDP_RETURN_ERROR(ret);
        }
        if (match == 0) {
          return _mdp_visit_table(inner, value, &t, consumed_size);
        }
      } break;
      default: {
        MDP_DEBUG("Invalid union for Definitions id: %u", d.t->item_id(&d));
        MDP_RETURN_ERROR(MDP_ERROR_SCHEMA_ENCODING);
      } break;
    }
  }

  MDP_DEBUG("Target type cannot be found!\n");
  MDP_RETURN_ERROR(MDP_ERROR_SCHEMA_ENCODING);
}

int mdp_visit(mdp_context context) {
  _mdp_inner inner_s;
  inner_s.context = &context;
  inner_s.defs = make_Definitions(&context.schema);
  inner_s.indent_levels = 0;
  inner_s.last_error = MDP_OK;
  _mdp_inner *inner = &inner_s;

  uint64_t syntax_version = inner->defs.t->syntax_version(&inner->defs);
  if (syntax_version != 1) {
    MDP_DEBUG("Expected syntax version: %d, actual syntax version: %ld\n", 1,
              syntax_version);
    MDP_RETURN_ERROR(MDP_ERROR_SCHEMA_ENCODING);
  }

  mol2_num_t consumed_size = 0;
  int ret = _mdp_visit_subtype(inner, context.data,
                               inner->defs.t->top_level_type(&inner->defs),
                               &consumed_size);
  if (ret != MDP_OK) {
    MDP_RETURN_ERROR(ret);
  }
  if (consumed_size != context.data.size) {
    MDP_DEBUG("Value has %u bytes but only consumed %u bytes!",
              context.data.size, consumed_size);
    MDP_RETURN_ERROR(MDP_ERROR_MOLECULE_ENCODING);
    return MDP_ERROR_MOLECULE_ENCODING;
  }
  _mdp_send_literal(inner, "\n");
  return inner->last_error;
}

#endif /* MOLECULE_DYNAMIC_PARSER_H_ */
