# molecule-dynamic-visitor

A dynamic [molecule](https://github.com/nervosnetwork/molecule) schema visitor for specific purposes.

## Usage

Make sure you have moleculec 0.7.5 or above installed.

```
$ cargo build
$ clang-16 -O3 -g -Wall -Werror test_main.c -o test_main
$ moleculec --format json --language - --schema-file schemas/spore.mol > spore.json
$ ./target/debug/molecule-schema-compacter --input-files spore.json --top-level-type SporeAction --output-file spore.data
$ ./target/debug/spore-data-generator --output-file actual.data
Seed: 1705393416521293029
Mint spore id: 0x03b4d22e8c56f18ca5111cc3f6b67bc150199f86b8c31d5dd5303f819f130c2a
Mint to: ckb1qq43t9serlu9h7plfqdmge746yxc9lwn2qtzwmydu7p4vyjx5s8ugqzxqqqqqq989ey4n94tze4up2w9hy2ydgfz2c2s4ujr2nfutf55j7yap95vd8czcmt59s24kecv7q4t4h572udsjfxrxdpzqnefwauw3w5enp8557uuhqx62ra0ujr
Mint data hash: 0xce647a870562ddeb0b04893a264526c2ea01b011bc0fe3489b0a145572dcef47
$ ./test_main spore.data actual.data
Schema size: 2408
Data size: 211
Success!
Visited data:

SporeAction(variant MintSpore, id = 0):
  MintSpore(table): {
    spore_id:
      Byte32: 0x03b4d22e8c56f18ca5111cc3f6b67bc150199f86b8c31d5dd5303f819f130c2a,
    to:
      Address: ckb1qq43t9serlu9h7plfqdmge746yxc9lwn2qtzwmydu7p4vyjx5s8ugqzxqqqqqq989ey4n94tze4up2w9hy2ydgfz2c2s4ujr2nfutf55j7yap95vd8czcmt59s24kecv7q4t4h572udsjfxrxdpzqnefwauw3w5enp8557uuhqx62ra0ujr,
    data_hash:
      Byte32: 0xce647a870562ddeb0b04893a264526c2ea01b011bc0fe3489b0a145572dcef47
  }
$ ./target/debug/spore-data-generator --output-file actual.data
Seed: 1705393444997846246
Transfer spore id: 0x3790217d4ef0958d5a1c872ecad8c59b2b4c8e0bdede972471eb99ae47b2b8a9
Transfer from: ckb1qptqcasa898c0y7u4ssj4wjud8mnu74wcgjnl2u7x5fdm6gqfvwp2qfzqqqqp0xt25cs5xm4ha9d70nvfhsxmlec0ykwszxvdd03zfx6hrp3dnqfeeuq8vyful
Transfer to: ckb1qpvp4jld95p0dvy82es5q34q3fs3q8cvvwxqxey3rsmt2avc3ehdwqqrqqqqq0f48g3rms2x
$ ./test_main spore.data actual.data
Schema size: 2408
Data size: 203
Success!
Visited data:

SporeAction(variant TransferSpore, id = 1):
  TransferSpore(table): {
    spore_id:
      Byte32: 0x3790217d4ef0958d5a1c872ecad8c59b2b4c8e0bdede972471eb99ae47b2b8a9,
    from:
      Address: ckb1qptqcasa898c0y7u4ssj4wjud8mnu74wcgjnl2u7x5fdm6gqfvwp2qfzqqqqp0xt25cs5xm4ha9d70nvfhsxmlec0ykwszxvdd03zfx6hrp3dnqfeeuq8vyful,
    to:
      Address: ckb1qpvp4jld95p0dvy82es5q34q3fs3q8cvvwxqxey3rsmt2avc3ehdwqqrqqqqq0f48g3rms2x
$ ./target/debug/spore-data-generator --output-file actual.data
Seed: 1705393470170362451
Burn spore id: 0x554632d507d1d30095a6b44854d132f567aec85dfe89321b278a125beba53a63
Burn from: ckb1qreg030xrqal8u7l76dqs7nfsunqywrpqlvjyelgdx04mrrapetqwqzgqqqqqjfhl7mfwn8yv8qu997e9acuyz3fmx4vq75t9fknkq5frz9k3mxrdfv3r6vx6c99j6upmyt6m7g739u7jrw0xq6g3et6qgd6ty2vm3wr7lavl9th75ck9ys7e7
$ ./test_main spore.data actual.data
Schema size: 2408
Data size: 177
Success!
Visited data:

SporeAction(variant BurnSpore, id = 2):
  BurnSpore(table): {
    spore_id:
      Byte32: 0x554632d507d1d30095a6b44854d132f567aec85dfe89321b278a125beba53a63,
    from:
      Address: ckb1qreg030xrqal8u7l76dqs7nfsunqywrpqlvjyelgdx04mrrapetqwqzgqqqqqjfhl7mfwn8yv8qu997e9acuyz3fmx4vq75t9fknkq5frz9k3mxrdfv3r6vx6c99j6upmyt6m7g739u7jrw0xq6g3et6qgd6ty2vm3wr7lavl9th75ck9ys7e7
  }
```

For now, a native binary aids the testing purpose. The actual code is written in a cross platform way, and is ready for CKB-VM environment.

## TODOs

* Settling down the actual text format
* More built-in types support
