# Sparking! Meteor docs

## Character password format

- Encoded length: 34 chars
- Decoded length: 26 bytes, 208 bits (204 actually used)

Offsets and size are in bits.

| Offset | Size | Description  |
| ------ | ---- | ------------ |
| 0      | 32   | Random seed  |
| 32     | 8    | Character ID |
| 40     | 72   | Z items      |
| 112    | 4    | (unknown)    |
| 116    | 1    | (unknown)    |
| 119    | 8    | Checksum     |
| 128    | 64   | Signature    |
| 196    | 8    | Checksum2    |
| 204    | 4    | Padding      |

Checksums and signature are generated the same way as in [Sparking! NEO](NEO.md). The difference with checksum2 is to stop at bit 196.

Padding is made of the lower 4 bits of checksum2. It's discarded by the game and only there for alignment purposes.

## Characters

TODO

## Z items

TODO