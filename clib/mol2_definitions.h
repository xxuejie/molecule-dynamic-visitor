
#ifndef _DEFINITIONS_API2_H_
#define _DEFINITIONS_API2_H_

#define MOLECULEC2_VERSION 7002
#define MOLECULE2_API_VERSION_MIN 5000

#include "molecule2_reader.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// ----forward declaration--------
struct Uint64Type;
struct Uint64VTable;
struct Uint64VTable *GetUint64VTable(void);
struct Uint64Type make_Uint64(mol2_cursor_t *cur);
uint32_t Uint64_len_impl(struct Uint64Type *);
uint8_t Uint64_get_impl(struct Uint64Type *, uint32_t, bool *);
struct StringType;
struct StringVTable;
struct StringVTable *GetStringVTable(void);
struct StringType make_String(mol2_cursor_t *cur);
uint32_t String_len_impl(struct StringType *);
uint8_t String_get_impl(struct StringType *, uint32_t, bool *);
struct FieldPairType;
struct FieldPairVTable;
struct FieldPairVTable *GetFieldPairVTable(void);
struct FieldPairType make_FieldPair(mol2_cursor_t *cur);
mol2_cursor_t FieldPair_get_name_impl(struct FieldPairType *);
mol2_cursor_t FieldPair_get_typ_impl(struct FieldPairType *);
struct FieldPairVecType;
struct FieldPairVecVTable;
struct FieldPairVecVTable *GetFieldPairVecVTable(void);
struct FieldPairVecType make_FieldPairVec(mol2_cursor_t *cur);
uint32_t FieldPairVec_len_impl(struct FieldPairVecType *);
struct FieldPairType FieldPairVec_get_impl(struct FieldPairVecType *, uint32_t,
                                           bool *);
struct OptionDefinitionType;
struct OptionDefinitionVTable;
struct OptionDefinitionVTable *GetOptionDefinitionVTable(void);
struct OptionDefinitionType make_OptionDefinition(mol2_cursor_t *cur);
mol2_cursor_t OptionDefinition_get_name_impl(struct OptionDefinitionType *);
mol2_cursor_t OptionDefinition_get_item_impl(struct OptionDefinitionType *);
struct UnionPairType;
struct UnionPairVTable;
struct UnionPairVTable *GetUnionPairVTable(void);
struct UnionPairType make_UnionPair(mol2_cursor_t *cur);
mol2_cursor_t UnionPair_get_typ_impl(struct UnionPairType *);
uint64_t UnionPair_get_id_impl(struct UnionPairType *);
struct UnionPairVecType;
struct UnionPairVecVTable;
struct UnionPairVecVTable *GetUnionPairVecVTable(void);
struct UnionPairVecType make_UnionPairVec(mol2_cursor_t *cur);
uint32_t UnionPairVec_len_impl(struct UnionPairVecType *);
struct UnionPairType UnionPairVec_get_impl(struct UnionPairVecType *, uint32_t,
                                           bool *);
struct UnionDefinitionType;
struct UnionDefinitionVTable;
struct UnionDefinitionVTable *GetUnionDefinitionVTable(void);
struct UnionDefinitionType make_UnionDefinition(mol2_cursor_t *cur);
mol2_cursor_t UnionDefinition_get_name_impl(struct UnionDefinitionType *);
struct UnionPairVecType UnionDefinition_get_items_impl(
    struct UnionDefinitionType *);
struct ArrayDefinitionType;
struct ArrayDefinitionVTable;
struct ArrayDefinitionVTable *GetArrayDefinitionVTable(void);
struct ArrayDefinitionType make_ArrayDefinition(mol2_cursor_t *cur);
mol2_cursor_t ArrayDefinition_get_name_impl(struct ArrayDefinitionType *);
mol2_cursor_t ArrayDefinition_get_item_impl(struct ArrayDefinitionType *);
uint64_t ArrayDefinition_get_item_count_impl(struct ArrayDefinitionType *);
struct StructDefinitionType;
struct StructDefinitionVTable;
struct StructDefinitionVTable *GetStructDefinitionVTable(void);
struct StructDefinitionType make_StructDefinition(mol2_cursor_t *cur);
mol2_cursor_t StructDefinition_get_name_impl(struct StructDefinitionType *);
struct FieldPairVecType StructDefinition_get_fields_impl(
    struct StructDefinitionType *);
struct FixvecDefinitionType;
struct FixvecDefinitionVTable;
struct FixvecDefinitionVTable *GetFixvecDefinitionVTable(void);
struct FixvecDefinitionType make_FixvecDefinition(mol2_cursor_t *cur);
mol2_cursor_t FixvecDefinition_get_name_impl(struct FixvecDefinitionType *);
mol2_cursor_t FixvecDefinition_get_item_impl(struct FixvecDefinitionType *);
struct DynvecDefinitionType;
struct DynvecDefinitionVTable;
struct DynvecDefinitionVTable *GetDynvecDefinitionVTable(void);
struct DynvecDefinitionType make_DynvecDefinition(mol2_cursor_t *cur);
mol2_cursor_t DynvecDefinition_get_name_impl(struct DynvecDefinitionType *);
mol2_cursor_t DynvecDefinition_get_item_impl(struct DynvecDefinitionType *);
struct TableDefinitionType;
struct TableDefinitionVTable;
struct TableDefinitionVTable *GetTableDefinitionVTable(void);
struct TableDefinitionType make_TableDefinition(mol2_cursor_t *cur);
mol2_cursor_t TableDefinition_get_name_impl(struct TableDefinitionType *);
struct FieldPairVecType TableDefinition_get_fields_impl(
    struct TableDefinitionType *);
struct DefinitionType;
struct DefinitionVTable;
struct DefinitionVTable *GetDefinitionVTable(void);
struct DefinitionType make_Definition(mol2_cursor_t *cur);
uint32_t Definition_item_id_impl(struct DefinitionType *);
struct OptionDefinitionType Definition_as_OptionDefinition_impl(
    struct DefinitionType *);
struct UnionDefinitionType Definition_as_UnionDefinition_impl(
    struct DefinitionType *);
struct ArrayDefinitionType Definition_as_ArrayDefinition_impl(
    struct DefinitionType *);
struct StructDefinitionType Definition_as_StructDefinition_impl(
    struct DefinitionType *);
struct FixvecDefinitionType Definition_as_FixvecDefinition_impl(
    struct DefinitionType *);
struct DynvecDefinitionType Definition_as_DynvecDefinition_impl(
    struct DefinitionType *);
struct TableDefinitionType Definition_as_TableDefinition_impl(
    struct DefinitionType *);
struct DefinitionVecType;
struct DefinitionVecVTable;
struct DefinitionVecVTable *GetDefinitionVecVTable(void);
struct DefinitionVecType make_DefinitionVec(mol2_cursor_t *cur);
uint32_t DefinitionVec_len_impl(struct DefinitionVecType *);
struct DefinitionType DefinitionVec_get_impl(struct DefinitionVecType *,
                                             uint32_t, bool *);
struct DefinitionsType;
struct DefinitionsVTable;
struct DefinitionsVTable *GetDefinitionsVTable(void);
struct DefinitionsType make_Definitions(mol2_cursor_t *cur);
struct DefinitionVecType Definitions_get_definitions_impl(
    struct DefinitionsType *);
mol2_cursor_t Definitions_get_top_level_type_impl(struct DefinitionsType *);
uint64_t Definitions_get_syntax_version_impl(struct DefinitionsType *);

// ----definition-----------------
typedef struct Uint64VTable {
  uint32_t (*len)(struct Uint64Type *);
  uint8_t (*get)(struct Uint64Type *, uint32_t, bool *);
} Uint64VTable;
typedef struct Uint64Type {
  mol2_cursor_t cur;
  Uint64VTable *t;
} Uint64Type;

typedef struct StringVTable {
  uint32_t (*len)(struct StringType *);
  uint8_t (*get)(struct StringType *, uint32_t, bool *);
} StringVTable;
typedef struct StringType {
  mol2_cursor_t cur;
  StringVTable *t;
} StringType;

typedef struct FieldPairVTable {
  mol2_cursor_t (*name)(struct FieldPairType *);
  mol2_cursor_t (*typ)(struct FieldPairType *);
} FieldPairVTable;
typedef struct FieldPairType {
  mol2_cursor_t cur;
  FieldPairVTable *t;
} FieldPairType;

typedef struct FieldPairVecVTable {
  uint32_t (*len)(struct FieldPairVecType *);
  struct FieldPairType (*get)(struct FieldPairVecType *, uint32_t, bool *);
} FieldPairVecVTable;
typedef struct FieldPairVecType {
  mol2_cursor_t cur;
  FieldPairVecVTable *t;
} FieldPairVecType;

typedef struct OptionDefinitionVTable {
  mol2_cursor_t (*name)(struct OptionDefinitionType *);
  mol2_cursor_t (*item)(struct OptionDefinitionType *);
} OptionDefinitionVTable;
typedef struct OptionDefinitionType {
  mol2_cursor_t cur;
  OptionDefinitionVTable *t;
} OptionDefinitionType;

typedef struct UnionPairVTable {
  mol2_cursor_t (*typ)(struct UnionPairType *);
  uint64_t (*id)(struct UnionPairType *);
} UnionPairVTable;
typedef struct UnionPairType {
  mol2_cursor_t cur;
  UnionPairVTable *t;
} UnionPairType;

typedef struct UnionPairVecVTable {
  uint32_t (*len)(struct UnionPairVecType *);
  struct UnionPairType (*get)(struct UnionPairVecType *, uint32_t, bool *);
} UnionPairVecVTable;
typedef struct UnionPairVecType {
  mol2_cursor_t cur;
  UnionPairVecVTable *t;
} UnionPairVecType;

typedef struct UnionDefinitionVTable {
  mol2_cursor_t (*name)(struct UnionDefinitionType *);
  struct UnionPairVecType (*items)(struct UnionDefinitionType *);
} UnionDefinitionVTable;
typedef struct UnionDefinitionType {
  mol2_cursor_t cur;
  UnionDefinitionVTable *t;
} UnionDefinitionType;

typedef struct ArrayDefinitionVTable {
  mol2_cursor_t (*name)(struct ArrayDefinitionType *);
  mol2_cursor_t (*item)(struct ArrayDefinitionType *);
  uint64_t (*item_count)(struct ArrayDefinitionType *);
} ArrayDefinitionVTable;
typedef struct ArrayDefinitionType {
  mol2_cursor_t cur;
  ArrayDefinitionVTable *t;
} ArrayDefinitionType;

typedef struct StructDefinitionVTable {
  mol2_cursor_t (*name)(struct StructDefinitionType *);
  struct FieldPairVecType (*fields)(struct StructDefinitionType *);
} StructDefinitionVTable;
typedef struct StructDefinitionType {
  mol2_cursor_t cur;
  StructDefinitionVTable *t;
} StructDefinitionType;

typedef struct FixvecDefinitionVTable {
  mol2_cursor_t (*name)(struct FixvecDefinitionType *);
  mol2_cursor_t (*item)(struct FixvecDefinitionType *);
} FixvecDefinitionVTable;
typedef struct FixvecDefinitionType {
  mol2_cursor_t cur;
  FixvecDefinitionVTable *t;
} FixvecDefinitionType;

typedef struct DynvecDefinitionVTable {
  mol2_cursor_t (*name)(struct DynvecDefinitionType *);
  mol2_cursor_t (*item)(struct DynvecDefinitionType *);
} DynvecDefinitionVTable;
typedef struct DynvecDefinitionType {
  mol2_cursor_t cur;
  DynvecDefinitionVTable *t;
} DynvecDefinitionType;

typedef struct TableDefinitionVTable {
  mol2_cursor_t (*name)(struct TableDefinitionType *);
  struct FieldPairVecType (*fields)(struct TableDefinitionType *);
} TableDefinitionVTable;
typedef struct TableDefinitionType {
  mol2_cursor_t cur;
  TableDefinitionVTable *t;
} TableDefinitionType;

typedef struct DefinitionVTable {
  uint32_t (*item_id)(struct DefinitionType *);
  struct OptionDefinitionType (*as_OptionDefinition)(struct DefinitionType *);
  struct UnionDefinitionType (*as_UnionDefinition)(struct DefinitionType *);
  struct ArrayDefinitionType (*as_ArrayDefinition)(struct DefinitionType *);
  struct StructDefinitionType (*as_StructDefinition)(struct DefinitionType *);
  struct FixvecDefinitionType (*as_FixvecDefinition)(struct DefinitionType *);
  struct DynvecDefinitionType (*as_DynvecDefinition)(struct DefinitionType *);
  struct TableDefinitionType (*as_TableDefinition)(struct DefinitionType *);
} DefinitionVTable;
typedef struct DefinitionType {
  mol2_cursor_t cur;
  DefinitionVTable *t;
} DefinitionType;

typedef struct DefinitionVecVTable {
  uint32_t (*len)(struct DefinitionVecType *);
  struct DefinitionType (*get)(struct DefinitionVecType *, uint32_t, bool *);
} DefinitionVecVTable;
typedef struct DefinitionVecType {
  mol2_cursor_t cur;
  DefinitionVecVTable *t;
} DefinitionVecType;

typedef struct DefinitionsVTable {
  struct DefinitionVecType (*definitions)(struct DefinitionsType *);
  mol2_cursor_t (*top_level_type)(struct DefinitionsType *);
  uint64_t (*syntax_version)(struct DefinitionsType *);
} DefinitionsVTable;
typedef struct DefinitionsType {
  mol2_cursor_t cur;
  DefinitionsVTable *t;
} DefinitionsType;

#ifndef MOLECULEC_C2_DECLARATION_ONLY

// ----implementation-------------
struct Uint64Type make_Uint64(mol2_cursor_t *cur) {
  Uint64Type ret;
  ret.cur = *cur;
  ret.t = GetUint64VTable();
  return ret;
}
struct Uint64VTable *GetUint64VTable(void) {
  static Uint64VTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.len = Uint64_len_impl;
  s_vtable.get = Uint64_get_impl;
  return &s_vtable;
}
uint32_t Uint64_len_impl(Uint64Type *this) { return 8; }
uint8_t Uint64_get_impl(Uint64Type *this, uint32_t index, bool *existing) {
  uint8_t ret = {0};
  mol2_cursor_res_t res = mol2_slice_by_offset2(&this->cur, 1 * index, 1);
  if (res.errno != MOL2_OK) {
    *existing = false;
    return ret;
  } else {
    *existing = true;
  }
  ret = convert_to_Uint8(&res.cur);
  return ret;
}
struct StringType make_String(mol2_cursor_t *cur) {
  StringType ret;
  ret.cur = *cur;
  ret.t = GetStringVTable();
  return ret;
}
struct StringVTable *GetStringVTable(void) {
  static StringVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.len = String_len_impl;
  s_vtable.get = String_get_impl;
  return &s_vtable;
}
uint32_t String_len_impl(StringType *this) {
  return mol2_fixvec_length(&this->cur);
}
uint8_t String_get_impl(StringType *this, uint32_t index, bool *existing) {
  uint8_t ret = {0};
  mol2_cursor_res_t res = mol2_fixvec_slice_by_index(&this->cur, 1, index);
  if (res.errno != MOL2_OK) {
    *existing = false;
    return ret;
  } else {
    *existing = true;
  }
  ret = convert_to_Uint8(&res.cur);
  return ret;
}
struct FieldPairType make_FieldPair(mol2_cursor_t *cur) {
  FieldPairType ret;
  ret.cur = *cur;
  ret.t = GetFieldPairVTable();
  return ret;
}
struct FieldPairVTable *GetFieldPairVTable(void) {
  static FieldPairVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.name = FieldPair_get_name_impl;
  s_vtable.typ = FieldPair_get_typ_impl;
  return &s_vtable;
}
mol2_cursor_t FieldPair_get_name_impl(FieldPairType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 0);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
mol2_cursor_t FieldPair_get_typ_impl(FieldPairType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 1);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
struct FieldPairVecType make_FieldPairVec(mol2_cursor_t *cur) {
  FieldPairVecType ret;
  ret.cur = *cur;
  ret.t = GetFieldPairVecVTable();
  return ret;
}
struct FieldPairVecVTable *GetFieldPairVecVTable(void) {
  static FieldPairVecVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.len = FieldPairVec_len_impl;
  s_vtable.get = FieldPairVec_get_impl;
  return &s_vtable;
}
uint32_t FieldPairVec_len_impl(FieldPairVecType *this) {
  return mol2_dynvec_length(&this->cur);
}
FieldPairType FieldPairVec_get_impl(FieldPairVecType *this, uint32_t index,
                                    bool *existing) {
  FieldPairType ret = {0};
  mol2_cursor_res_t res = mol2_dynvec_slice_by_index(&this->cur, index);
  if (res.errno != MOL2_OK) {
    *existing = false;
    return ret;
  } else {
    *existing = true;
  }
  ret.cur = res.cur;
  ret.t = GetFieldPairVTable();
  return ret;
}
struct OptionDefinitionType make_OptionDefinition(mol2_cursor_t *cur) {
  OptionDefinitionType ret;
  ret.cur = *cur;
  ret.t = GetOptionDefinitionVTable();
  return ret;
}
struct OptionDefinitionVTable *GetOptionDefinitionVTable(void) {
  static OptionDefinitionVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.name = OptionDefinition_get_name_impl;
  s_vtable.item = OptionDefinition_get_item_impl;
  return &s_vtable;
}
mol2_cursor_t OptionDefinition_get_name_impl(OptionDefinitionType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 0);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
mol2_cursor_t OptionDefinition_get_item_impl(OptionDefinitionType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 1);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
struct UnionPairType make_UnionPair(mol2_cursor_t *cur) {
  UnionPairType ret;
  ret.cur = *cur;
  ret.t = GetUnionPairVTable();
  return ret;
}
struct UnionPairVTable *GetUnionPairVTable(void) {
  static UnionPairVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.typ = UnionPair_get_typ_impl;
  s_vtable.id = UnionPair_get_id_impl;
  return &s_vtable;
}
mol2_cursor_t UnionPair_get_typ_impl(UnionPairType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 0);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
uint64_t UnionPair_get_id_impl(UnionPairType *this) {
  uint64_t ret;
  mol2_cursor_t ret2 = mol2_table_slice_by_index(&this->cur, 1);
  ret = convert_to_Uint64(&ret2);
  return ret;
}
struct UnionPairVecType make_UnionPairVec(mol2_cursor_t *cur) {
  UnionPairVecType ret;
  ret.cur = *cur;
  ret.t = GetUnionPairVecVTable();
  return ret;
}
struct UnionPairVecVTable *GetUnionPairVecVTable(void) {
  static UnionPairVecVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.len = UnionPairVec_len_impl;
  s_vtable.get = UnionPairVec_get_impl;
  return &s_vtable;
}
uint32_t UnionPairVec_len_impl(UnionPairVecType *this) {
  return mol2_dynvec_length(&this->cur);
}
UnionPairType UnionPairVec_get_impl(UnionPairVecType *this, uint32_t index,
                                    bool *existing) {
  UnionPairType ret = {0};
  mol2_cursor_res_t res = mol2_dynvec_slice_by_index(&this->cur, index);
  if (res.errno != MOL2_OK) {
    *existing = false;
    return ret;
  } else {
    *existing = true;
  }
  ret.cur = res.cur;
  ret.t = GetUnionPairVTable();
  return ret;
}
struct UnionDefinitionType make_UnionDefinition(mol2_cursor_t *cur) {
  UnionDefinitionType ret;
  ret.cur = *cur;
  ret.t = GetUnionDefinitionVTable();
  return ret;
}
struct UnionDefinitionVTable *GetUnionDefinitionVTable(void) {
  static UnionDefinitionVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.name = UnionDefinition_get_name_impl;
  s_vtable.items = UnionDefinition_get_items_impl;
  return &s_vtable;
}
mol2_cursor_t UnionDefinition_get_name_impl(UnionDefinitionType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 0);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
UnionPairVecType UnionDefinition_get_items_impl(UnionDefinitionType *this) {
  UnionPairVecType ret;
  mol2_cursor_t cur = mol2_table_slice_by_index(&this->cur, 1);
  ret.cur = cur;
  ret.t = GetUnionPairVecVTable();
  return ret;
}
struct ArrayDefinitionType make_ArrayDefinition(mol2_cursor_t *cur) {
  ArrayDefinitionType ret;
  ret.cur = *cur;
  ret.t = GetArrayDefinitionVTable();
  return ret;
}
struct ArrayDefinitionVTable *GetArrayDefinitionVTable(void) {
  static ArrayDefinitionVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.name = ArrayDefinition_get_name_impl;
  s_vtable.item = ArrayDefinition_get_item_impl;
  s_vtable.item_count = ArrayDefinition_get_item_count_impl;
  return &s_vtable;
}
mol2_cursor_t ArrayDefinition_get_name_impl(ArrayDefinitionType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 0);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
mol2_cursor_t ArrayDefinition_get_item_impl(ArrayDefinitionType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 1);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
uint64_t ArrayDefinition_get_item_count_impl(ArrayDefinitionType *this) {
  uint64_t ret;
  mol2_cursor_t ret2 = mol2_table_slice_by_index(&this->cur, 2);
  ret = convert_to_Uint64(&ret2);
  return ret;
}
struct StructDefinitionType make_StructDefinition(mol2_cursor_t *cur) {
  StructDefinitionType ret;
  ret.cur = *cur;
  ret.t = GetStructDefinitionVTable();
  return ret;
}
struct StructDefinitionVTable *GetStructDefinitionVTable(void) {
  static StructDefinitionVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.name = StructDefinition_get_name_impl;
  s_vtable.fields = StructDefinition_get_fields_impl;
  return &s_vtable;
}
mol2_cursor_t StructDefinition_get_name_impl(StructDefinitionType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 0);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
FieldPairVecType StructDefinition_get_fields_impl(StructDefinitionType *this) {
  FieldPairVecType ret;
  mol2_cursor_t cur = mol2_table_slice_by_index(&this->cur, 1);
  ret.cur = cur;
  ret.t = GetFieldPairVecVTable();
  return ret;
}
struct FixvecDefinitionType make_FixvecDefinition(mol2_cursor_t *cur) {
  FixvecDefinitionType ret;
  ret.cur = *cur;
  ret.t = GetFixvecDefinitionVTable();
  return ret;
}
struct FixvecDefinitionVTable *GetFixvecDefinitionVTable(void) {
  static FixvecDefinitionVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.name = FixvecDefinition_get_name_impl;
  s_vtable.item = FixvecDefinition_get_item_impl;
  return &s_vtable;
}
mol2_cursor_t FixvecDefinition_get_name_impl(FixvecDefinitionType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 0);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
mol2_cursor_t FixvecDefinition_get_item_impl(FixvecDefinitionType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 1);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
struct DynvecDefinitionType make_DynvecDefinition(mol2_cursor_t *cur) {
  DynvecDefinitionType ret;
  ret.cur = *cur;
  ret.t = GetDynvecDefinitionVTable();
  return ret;
}
struct DynvecDefinitionVTable *GetDynvecDefinitionVTable(void) {
  static DynvecDefinitionVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.name = DynvecDefinition_get_name_impl;
  s_vtable.item = DynvecDefinition_get_item_impl;
  return &s_vtable;
}
mol2_cursor_t DynvecDefinition_get_name_impl(DynvecDefinitionType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 0);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
mol2_cursor_t DynvecDefinition_get_item_impl(DynvecDefinitionType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 1);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
struct TableDefinitionType make_TableDefinition(mol2_cursor_t *cur) {
  TableDefinitionType ret;
  ret.cur = *cur;
  ret.t = GetTableDefinitionVTable();
  return ret;
}
struct TableDefinitionVTable *GetTableDefinitionVTable(void) {
  static TableDefinitionVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.name = TableDefinition_get_name_impl;
  s_vtable.fields = TableDefinition_get_fields_impl;
  return &s_vtable;
}
mol2_cursor_t TableDefinition_get_name_impl(TableDefinitionType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 0);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
FieldPairVecType TableDefinition_get_fields_impl(TableDefinitionType *this) {
  FieldPairVecType ret;
  mol2_cursor_t cur = mol2_table_slice_by_index(&this->cur, 1);
  ret.cur = cur;
  ret.t = GetFieldPairVecVTable();
  return ret;
}
struct DefinitionType make_Definition(mol2_cursor_t *cur) {
  DefinitionType ret;
  ret.cur = *cur;
  ret.t = GetDefinitionVTable();
  return ret;
}
struct DefinitionVTable *GetDefinitionVTable(void) {
  static DefinitionVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.item_id = Definition_item_id_impl;
  s_vtable.as_OptionDefinition = Definition_as_OptionDefinition_impl;
  s_vtable.as_UnionDefinition = Definition_as_UnionDefinition_impl;
  s_vtable.as_ArrayDefinition = Definition_as_ArrayDefinition_impl;
  s_vtable.as_StructDefinition = Definition_as_StructDefinition_impl;
  s_vtable.as_FixvecDefinition = Definition_as_FixvecDefinition_impl;
  s_vtable.as_DynvecDefinition = Definition_as_DynvecDefinition_impl;
  s_vtable.as_TableDefinition = Definition_as_TableDefinition_impl;
  return &s_vtable;
}
uint32_t Definition_item_id_impl(DefinitionType *this) {
  return mol2_unpack_number(&this->cur);
}
OptionDefinitionType Definition_as_OptionDefinition_impl(DefinitionType *this) {
  OptionDefinitionType ret;
  mol2_union_t u = mol2_union_unpack(&this->cur);
  ret.cur = u.cursor;
  ret.t = GetOptionDefinitionVTable();
  return ret;
}
UnionDefinitionType Definition_as_UnionDefinition_impl(DefinitionType *this) {
  UnionDefinitionType ret;
  mol2_union_t u = mol2_union_unpack(&this->cur);
  ret.cur = u.cursor;
  ret.t = GetUnionDefinitionVTable();
  return ret;
}
ArrayDefinitionType Definition_as_ArrayDefinition_impl(DefinitionType *this) {
  ArrayDefinitionType ret;
  mol2_union_t u = mol2_union_unpack(&this->cur);
  ret.cur = u.cursor;
  ret.t = GetArrayDefinitionVTable();
  return ret;
}
StructDefinitionType Definition_as_StructDefinition_impl(DefinitionType *this) {
  StructDefinitionType ret;
  mol2_union_t u = mol2_union_unpack(&this->cur);
  ret.cur = u.cursor;
  ret.t = GetStructDefinitionVTable();
  return ret;
}
FixvecDefinitionType Definition_as_FixvecDefinition_impl(DefinitionType *this) {
  FixvecDefinitionType ret;
  mol2_union_t u = mol2_union_unpack(&this->cur);
  ret.cur = u.cursor;
  ret.t = GetFixvecDefinitionVTable();
  return ret;
}
DynvecDefinitionType Definition_as_DynvecDefinition_impl(DefinitionType *this) {
  DynvecDefinitionType ret;
  mol2_union_t u = mol2_union_unpack(&this->cur);
  ret.cur = u.cursor;
  ret.t = GetDynvecDefinitionVTable();
  return ret;
}
TableDefinitionType Definition_as_TableDefinition_impl(DefinitionType *this) {
  TableDefinitionType ret;
  mol2_union_t u = mol2_union_unpack(&this->cur);
  ret.cur = u.cursor;
  ret.t = GetTableDefinitionVTable();
  return ret;
}
struct DefinitionVecType make_DefinitionVec(mol2_cursor_t *cur) {
  DefinitionVecType ret;
  ret.cur = *cur;
  ret.t = GetDefinitionVecVTable();
  return ret;
}
struct DefinitionVecVTable *GetDefinitionVecVTable(void) {
  static DefinitionVecVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.len = DefinitionVec_len_impl;
  s_vtable.get = DefinitionVec_get_impl;
  return &s_vtable;
}
uint32_t DefinitionVec_len_impl(DefinitionVecType *this) {
  return mol2_dynvec_length(&this->cur);
}
DefinitionType DefinitionVec_get_impl(DefinitionVecType *this, uint32_t index,
                                      bool *existing) {
  DefinitionType ret = {0};
  mol2_cursor_res_t res = mol2_dynvec_slice_by_index(&this->cur, index);
  if (res.errno != MOL2_OK) {
    *existing = false;
    return ret;
  } else {
    *existing = true;
  }
  ret.cur = res.cur;
  ret.t = GetDefinitionVTable();
  return ret;
}
struct DefinitionsType make_Definitions(mol2_cursor_t *cur) {
  DefinitionsType ret;
  ret.cur = *cur;
  ret.t = GetDefinitionsVTable();
  return ret;
}
struct DefinitionsVTable *GetDefinitionsVTable(void) {
  static DefinitionsVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.definitions = Definitions_get_definitions_impl;
  s_vtable.top_level_type = Definitions_get_top_level_type_impl;
  s_vtable.syntax_version = Definitions_get_syntax_version_impl;
  return &s_vtable;
}
DefinitionVecType Definitions_get_definitions_impl(DefinitionsType *this) {
  DefinitionVecType ret;
  mol2_cursor_t cur = mol2_table_slice_by_index(&this->cur, 0);
  ret.cur = cur;
  ret.t = GetDefinitionVecVTable();
  return ret;
}
mol2_cursor_t Definitions_get_top_level_type_impl(DefinitionsType *this) {
  mol2_cursor_t ret;
  mol2_cursor_t re2 = mol2_table_slice_by_index(&this->cur, 1);
  ret = convert_to_rawbytes(&re2);
  return ret;
}
uint64_t Definitions_get_syntax_version_impl(DefinitionsType *this) {
  uint64_t ret;
  mol2_cursor_t ret2 = mol2_table_slice_by_index(&this->cur, 2);
  ret = convert_to_Uint64(&ret2);
  return ret;
}
#endif  // MOLECULEC_C2_DECLARATION_ONLY

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // _DEFINITIONS_API2_H_
