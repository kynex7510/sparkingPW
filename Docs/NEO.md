# Sparking! NEO docs

## Format

Passwords are 32 chars long. Each character represents a 6 bit code, thus 192 bits of data are serialized into a readable string.

- 0-32: random seed
- 32-40: character ID
- 40-110: Z items
- 110-113: AI
- 113: target
- 114-118: country
- 118: password type
- 119-128: checksum
- 128-192: signature

Checksum is computed by adding each bit from 0 to 128. In reality one can stop at 119, because the checksum field itself is initialized to 0, so it never affects the computation.

Signature is made by xoring the first 8 bytes with the last 8 bytes.

Everything is enciphered using mersenne twister rng, the seed is based on the random seed.

## Characters

TODO: incomplete.

| Name                    | ID  |
| ----------------------- | --- |
| Goku                    | 0   |
| Kid Gohan               | 4   |
| Teen Gohan              | 5   |
| Piccolo                 | 8   |
| Krillin                 | 9   |
| Yamcha                  | 10  |
| Tien                    | 11  |
| Chiaotzu                | 12  |
| Raditz                  | 13  |
| Saibamen                | 14  |
| Nappa                   | 15  |
| Vegeta                  | 16  |
| Zarbon                  | 19  |
| Dodoria                 | 21  |
| Ginyu                   | 22  |
| Recoome                 | 23  |
| Burter                  | 24  |
| Jeice                   | 25  |
| Guldo                   | 26  |
| Frieza                  | 27  |
| Mecha Frieza            | 32  |
| Trunks (Sword)          | 33  |
| Android #16             | 36  |
| Android #17             | 37  |
| Android #18             | 38  |
| Android #19             | 39  |
| Dr. Gero                | 40  |
| Cell                    | 41  |
| Cell Jr.                | 45  |
| Hercule                 | 46  |
| Gohan                   | 47  |
| Ultimate Gohan          | 51  |
| Goten                   | 52  |
| Kid Trunks              | 54  |
| Videl                   | 59  |
| Goku SSJ4               | 75  |
| Bardock                 | 78  |
| Vegeta (Scouter)        | 79  |
| Majin Vegeta (2nd Form) | 80  |
| Trunks                  | 81  |
| Kid Goku                | 85  |
| Garlic Jr.              | 90  |
| Pan                     | 92  |
| Meta Cooler             | 94  |
| Salza                   | 100 |
| Broly                   | 101 |
| Bojack                  | 103 |
| Janemba                 | 104 |
| Cui                     | 111 |
| Pikkon                  | 112 |
| Syn Shenron             | 116 |
| Turles                  | 120 |
| Supreme Kai             | 122 |
| Vegeta (2nd Form)       | 127 |
| Baby Vegeta             | 129 |
| Random                  | 130 |
| Password Character      | 132 |

## Z Items

| Name       | ID  |
| ---------- | --- |
| Health +1  | 1   |
| Health +7  | 7   |
| Attack +7  | 45  |
| Defense +7 | 64  |