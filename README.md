# molecule-dynamic-visitor

A dynamic [molecule](https://github.com/nervosnetwork/molecule) schema visitor for specific purposes.

## Usage

For now, you need to manually compile moleculec binary from git (no earlier than [this commit](https://github.com/nervosnetwork/molecule/commit/526745650ccb2a10d6b2e883f74e90169decfa10)), and make sure the compiled moleculec binary is in `PATH`. When molecule releases 0.8.0, this might not be needed.

```
$ cargo build
$ clang-16 -O3 -g -Wall -Werror test_main.c -o test_main
$ moleculec --format json --language - --schema-file schemas/spore.mol > spore.json
$ ./target/debug/molecule-schema-compacter --input-files spore.json --top-level-type SporeAction --output-file spore.data
$ ./target/debug/spore-data-generator --output-file actual.data
Seed: 1705381261466965560
Mint spore id: 0xfbbb4863c9a75be259d8e39447ac4809f62516f64ff27469aaa3a17c3451d86a
Mint data hash: 0x44141dd5b981c2533cd11e51e925be954111b81c0e95b60fe0830adc9ef75393
$ ./test_main spore.data actual.data
Schema size: 2408
Data size: 202
Success!
Visited data:

SporeAction(variant MintSpore, id = 0):
  MintSpore(table): {
    spore_id:
      Byte32: 0xfbbb4863c9a75be259d8e39447ac4809f62516f64ff27469aaa3a17c3451d86a,
    to:
      Address(variant Script, id = 0):
        Script(table): {
          code_hash:
            Byte32: 0xaa753c908b743d9d8b7dc05517c0689ce8544ff90fe33d9b229db45cd8c9c0aa,
          hash_type:
            0x1,
          args:
            Bytes(fixvec, len = 61): [
              0x9c, 0x6e, 0xc7, 0xda, 0x1b, 0x94, 0x97, 0xba,
              0x98, 0x49, 0xee, 0x72, 0x4a, 0xce, 0x91, 0x8a,
              0xa0, 0xd8, 0x6e, 0xf1, 0x51, 0xbb, 0x12, 0x25,
              0xd2, 0x63, 0x6a, 0x36, 0x55, 0x94, 0xbc, 0xb4,
              0x1b, 0x48, 0x8e, 0xcd, 0x0b, 0xf0, 0x7c, 0x6b,
              0x22, 0x62, 0x9d, 0x30, 0x7c, 0x36, 0xc0, 0xe4,
              0xd8, 0x18, 0xe8, 0x12, 0xa7, 0xf6, 0x84, 0xe3,
              0x93, 0xdb, 0xd2, 0x55, 0x8b
            ]
        },
    data_hash:
      Byte32: 0x44141dd5b981c2533cd11e51e925be954111b81c0e95b60fe0830adc9ef75393
  }
$ ./target/debug/spore-data-generator --output-file actual.data
Seed: 1705381449765628512
Transfer spore id: 0xbae4dd4eb85870c2328ea59b0b87c5db50638aad5eb02b878b2e4da5232acb14
$ ./test_main spore.data actual.data
Schema size: 2408
Data size: 300
Success!
Visited data:

SporeAction(variant TransferSpore, id = 1):
  TransferSpore(table): {
    spore_id:
      Byte32: 0xbae4dd4eb85870c2328ea59b0b87c5db50638aad5eb02b878b2e4da5232acb14,
    from:
      Address(variant Script, id = 0):
        Script(table): {
          code_hash:
            Byte32: 0xb44777f47097e0c923d73f9130dadcfb4a338900eee8bb71f331723e4d897d25,
          hash_type:
            0x0,
          args:
            Bytes(fixvec, len = 94): [
              0x2a, 0xb2, 0xce, 0xa3, 0xfa, 0x41, 0x0a, 0xfe,
              0xaf, 0x71, 0xc8, 0xe4, 0x82, 0x3e, 0xc3, 0xb4,
              0x9a, 0xe1, 0xf8, 0xa5, 0x04, 0x19, 0xc9, 0x41,
              0xc9, 0xac, 0xde, 0x89, 0x47, 0xfb, 0x01, 0xe7,
              0x3a, 0xef, 0x29, 0xb0, 0x3f, 0xcf, 0x6c, 0xaa,
              0x0d, 0xfe, 0xd6, 0xb1, 0x5c, 0x88, 0x49, 0x3f,
              0x33, 0x5b, 0xd5, 0x21, 0xf2, 0xfd, 0xcb, 0x6f,
              0x82, 0x2b, 0x0b, 0x0d, 0x2e, 0xc4, 0xbc, 0x24,
              0xab, 0x82, 0x88, 0xed, 0xe2, 0xb7, 0xdc, 0x51,
              0xf9, 0xe3, 0x00, 0x93, 0xde, 0x17, 0xde, 0x6c,
              0xc3, 0x18, 0xcd, 0x14, 0x9c, 0xa6, 0xeb, 0xae,
              0x15, 0xb5, 0xca, 0xee, 0x23, 0x05
            ]
        },
    to:
      Address(variant Script, id = 0):
        Script(table): {
          code_hash:
            Byte32: 0x32c799447930ccdcbc91c0c0bcdf017e540dd2fbd2c9a8cbf48a59b240f729fa,
          hash_type:
            0x0,
          args:
            Bytes(fixvec, len = 40): [
              0xee, 0x24, 0x47, 0x41, 0x19, 0xf3, 0x48, 0x9a,
              0xec, 0xf4, 0x05, 0x8d, 0xcb, 0xa7, 0x6f, 0x0a,
              0x77, 0x2e, 0xf6, 0x0d, 0xa6, 0x85, 0x0b, 0x8d,
              0xff, 0xb8, 0x3b, 0x63, 0xa4, 0x7e, 0xc3, 0x34,
              0x6d, 0x0c, 0x23, 0x61, 0xc3, 0x33, 0xd4, 0xb5
            ]
        }
  }
```

For now, a native binary aids the testing purpose. The actual code is written in a cross platform way, and is ready for CKB-VM environment.

## TODOs

* Settling down the actual text format
* More built-in types support
