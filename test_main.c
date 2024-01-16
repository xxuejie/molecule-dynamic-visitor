#include "clib/molecule-dynamic-visitor.h"

typedef struct {
  uint8_t *data;
  size_t length;
} alloc_feeder;

int feed_data(const uint8_t *data, size_t length,
              void *feeder_context) {
  alloc_feeder *f = (alloc_feeder *) feeder_context;
  f->data = realloc(f->data, f->length + length);
  memcpy(&f->data[f->length], data, length);
  f->length += length;

  return 0;
}

mol2_data_source_t make_data_source(const void *memory, uint32_t size) {
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

mol2_cursor_t cursor_from_source(mol2_data_source_t* source) {
  mol2_cursor_t cur;
  cur.offset = 0;
  cur.size = source->total_size;
  cur.data_source = source;
  return cur;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Usage: %s <schema file> <data file>\n", argv[0]);
    return 1;
  }

  FILE *fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    printf("Cannot open file %s\n", argv[1]);
    return 1;
  }
  fseek(fp, 0L, SEEK_END);
  size_t schema_size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  void *schema = malloc(schema_size);
  fread(schema, schema_size, 1, fp);
  fclose(fp);
  printf("Schema size: %lu\n", schema_size);

  fp = fopen(argv[2], "rb");
  if (fp == NULL) {
    printf("Cannot open file %s\n", argv[2]);
    return 1;
  }
  fseek(fp, 0L, SEEK_END);
  size_t data_size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  void *data = malloc(data_size);
  fread(data, data_size, 1, fp);
  fclose(fp);
  printf("Data size: %lu\n", data_size);

  alloc_feeder context;
  context.data = NULL;
  context.length = 0;

  mol2_data_source_t schema_source = make_data_source(schema, (uint32_t) schema_size);
  mol2_data_source_t data_source = make_data_source(data, (uint32_t) data_size);

  mol2_cursor_t schema_cursor = cursor_from_source(&schema_source);
  mol2_cursor_t data_cursor = cursor_from_source(&data_source);

  mdp_context mcontext;
  mcontext.schema = schema_cursor;
  mcontext.data = data_cursor;
  mcontext.feeder = feed_data;
  mcontext.feeder_context = &context;

  int ret = mdp_visit(mcontext);
  free(schema);
  free(data);

  if (ret == MDP_OK) {
    printf("Success!\n");
    if (context.data != NULL) {
      feed_data((const uint8_t *) "\0", 1, &context);
      printf("Visited data:\n\n%s", context.data);
      free(context.data);
    } else {
      printf("No data\n");
    }
  } else {
    printf("Error: %d\n", ret);
  }

  return ret;
}
