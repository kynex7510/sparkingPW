# Sparking! NEO docs

## Character password format

- Encoded length: 32 chars
- Decoded length: 24 bytes, 192 bits

Offsets and size are in bits.

| Offset | Size | Description                     |
| ------ | ---- | ------------------------------- |
| 0      | 32   | Random seed                     |
| 32     | 8    | Character ID                    |
| 40     | 70   | Z items                         |
| 110    | 3    | AI logic                        |
| 113    | 1    | Target (unknown purpose)        |
| 114    | 4    | Country                         |
| 118    | 1    | Password type (unknown purpose) |
| 119    | 8    | Checksum                        |
| 128    | 64   | Signature                       |

Checksum is computed by adding each bit from 0 to 128. In reality one can stop at 119, because the checksum field itself is initialized to 0, so it never affects the computation.

Signature is made by xoring the first 8 bytes with the last 8 bytes.

### Characters

TODO

### Z items

TODO: incomplete.

| ID  | Name       |
| --- | ---------- |
| 1   | Health +1  |
| 7   | Health +7  |
| 45  | Attack +7  |
| 64  | Defense +7 |

### AI logic

| Value | Description |
| ----- | ----------- |
| 0     | Balance     |
| 1     | Powerful    |
| 2     | Technical   |

### Country

| Value | Name    |
| ----- | ------- |
| 0     | Japan   |
| 1     | America |
| 2     | UK      |
| 3     | France  |
| 4     | Italy   |
| 5     | Germany |
| 6     | Spain   |