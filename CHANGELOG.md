# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.5.0]

### Added
- Null Move Pruning

### Tested Platformms
- macOS Ventura on 2.3 GHz Dual-Core Intel Core i5 64-bit CPU with 8 GB 2133 MHz LPDDR3 Memory
- Ubuntu 24.04 (LTS) x64 on 4-core Intel processor with 8 GB Memory

### SPRT Results
```
Score of pixie_v0.5.0 vs pixie_v0.4.0: 159 - 86 - 117  [0.601] 362
...      pixie_v0.5.0 playing White: 107 - 23 - 52  [0.731] 182
...      pixie_v0.5.0 playing Black: 52 - 63 - 65  [0.469] 180
...      White vs Black: 170 - 75 - 117  [0.631] 362
Elo difference: 71.0 +/- 29.8, LOS: 100.0 %, DrawRatio: 32.3 %
SPRT: llr 2.95 (100.2%), lbound -2.94, ubound 2.94 - H1 was accepted
```

## [0.4.0]

### Added
- Aspiration Windows in iterative deepening search
- Divide function in perft
- Debug metrics

### Fixed
- NPS calculation

### Tested Platformms
- macOS Ventura on 2.3 GHz Dual-Core Intel Core i5 64-bit CPU with 8 GB 2133 MHz LPDDR3 Memory
- Ubuntu 24.04 (LTS) x64 on 4-core Intel processor with 8 GB Memory

### SPRT Results
```
Score of pixie_v0.4.0 vs pixie_v0.3.0: 334 - 380 - 458  [0.480] 1172
...      pixie_v0.4.0 playing White: 193 - 128 - 265  [0.555] 586
...      pixie_v0.4.0 playing Black: 141 - 252 - 193  [0.405] 586
...      White vs Black: 445 - 269 - 458  [0.575] 1172
Elo difference: -13.6 +/- 15.5, LOS: 4.3 %, DrawRatio: 39.1 %
SPRT: llr -2.97 (-101.0%), lbound -2.94, ubound 2.94 - H0 was accepted
```

## [0.3.0]

### Added
- Transposition Table entry replacement using aging
- Late Move Reduction to search

### Tested Platformms
- macOS Ventura on 2.3 GHz Dual-Core Intel Core i5 64-bit CPU with 8 GB 2133 MHz LPDDR3 Memory
- Ubuntu 24.04 (LTS) x64 on 4-core Intel processor with 8 GB Memory

### SPRT Results
```
Score of pixie_v0.3.0 vs pixie_v0.2.2: 267 - 191 - 253  [0.553] 711
...      pixie_v0.3.0 playing White: 152 - 79 - 125  [0.603] 356
...      pixie_v0.3.0 playing Black: 115 - 112 - 128  [0.504] 355
...      White vs Black: 264 - 194 - 253  [0.549] 711
Elo difference: 37.3 +/- 20.5, LOS: 100.0 %, DrawRatio: 35.6 %
SPRT: llr 2.96 (100.5%), lbound -2.94, ubound 2.94 - H1 was accepted
```

## [0.2.3]

### Fixed
- EP move bug in string to move
- Move score bug in string to move

### Tested Platformms
- macOS Ventura on 2.3 GHz Dual-Core Intel Core i5 64-bit CPU with 8 GB 2133 MHz LPDDR3 Memory
- Ubuntu 24.04 (LTS) x64 on 4-core Intel processor with 8 GB Memory

## [0.2.2]

### Fixed
- Move ordering bug

### Changed
- Zobrist keys generation
- UCI output to include time and nps
- Invalid move handling

### Tested Platformms
- macOS Ventura on 2.3 GHz Dual-Core Intel Core i5 64-bit CPU with 8 GB 2133 MHz LPDDR3 Memory
- Ubuntu 24.04 (LTS) x64 on 4-core Intel processor with 8 GB Memory

### SPRT Results
```
Score of pixie_v0.2.2 vs pixie_v0.2.1: 134 - 52 - 47  [0.676] 233
...      pixie_v0.2.2 playing White: 87 - 15 - 15  [0.808] 117
...      pixie_v0.2.2 playing Black: 47 - 37 - 32  [0.543] 116
...      White vs Black: 124 - 62 - 47  [0.633] 233
Elo difference: 127.7 +/- 42.0, LOS: 100.0 %, DrawRatio: 20.2 %
SPRT: llr 2.95 (100.2%), lbound -2.94, ubound 2.94 - H1 was accepted
```

## [0.2.1]

### Added
- Killer Move Heuristic
- Histroy Heuristic

### Fixed
- Sort move logic

### Tested Platformms
- macOS Ventura on 2.3 GHz Dual-Core Intel Core i5 64-bit CPU with 8 GB 2133 MHz LPDDR3 Memory
- Ubuntu 24.04 (LTS) x64 on 4-core Intel processor with 8 GB Memory

### SPRT Results
```
Score of pixie_v0.2.1 vs pixie_v0.2.0: 358 - 263 - 132  [0.563] 753
...      pixie_v0.2.1 playing White: 240 - 55 - 82  [0.745] 377
...      pixie_v0.2.1 playing Black: 118 - 208 - 50  [0.380] 376
...      White vs Black: 448 - 173 - 132  [0.683] 753
Elo difference: 44.1 +/- 22.7, LOS: 100.0 %, DrawRatio: 17.5 %
SPRT: llr 2.95 (100.2%), lbound -2.94, ubound 2.94 - H1 was accepted
```

## [0.2.0]

### Added
- MVV LVA Move Ordering

### Changed
- Search algorithm from Negamax to PVS
- Evaluation function

### Tested Platformms
- macOS Ventura on 2.3 GHz Dual-Core Intel Core i5 64-bit CPU with 8 GB 2133 MHz LPDDR3 Memory
- Ubuntu 24.04 (LTS) x64 on 4-core Intel processor with 8 GB Memory

### SPRT Results
```
Score of pixie_v0.2.0 vs pixie_v0.1.0: 91 - 6 - 6  [0.913] 103
...      pixie_v0.2.0 playing White: 47 - 0 - 4  [0.961] 51
...      pixie_v0.2.0 playing Black: 44 - 6 - 2  [0.865] 52
...      White vs Black: 53 - 44 - 6  [0.544] 103
Elo difference: 407.6 +/- 120.2, LOS: 100.0 %, DrawRatio: 5.8 %
SPRT: llr 2.97 (100.7%), lbound -2.94, ubound 2.94 - H1 was accepted
```

## [0.1.0]

### Added
- Board represented using Bitboards
- Move generation using Magic Bitboards
- Multithreaded Perft to test move generation, make, and unmake moves
- Negamax search with alpha-beta pruning and Transposition Tables
- Quiescence to tackle horizon effect
- HCE with piece-square tables
- Integration with UCI protocol

### Tested Platformms
- macOS Ventura on 2.3 GHz Dual-Core Intel Core i5 64-bit CPU with 8 GB 2133 MHz LPDDR3 Memory
- Ubuntu 24.04 (LTS) x64 on 4-core Intel processor with 8 GB Memory

[0.1.0]: https://github.com/codemaniac/pixie/releases/tag/v0.1.0
[0.2.0]: https://github.com/codemaniac/pixie/releases/tag/v0.2.0
[0.2.1]: https://github.com/codemaniac/pixie/releases/tag/v0.2.1
[0.2.2]: https://github.com/codemaniac/pixie/releases/tag/v0.2.2
[0.2.3]: https://github.com/codemaniac/pixie/releases/tag/v0.2.3
[0.3.0]: https://github.com/codemaniac/pixie/releases/tag/v0.3.0
[0.4.0]: https://github.com/codemaniac/pixie/releases/tag/v0.4.0
[0.5.0]: https://github.com/codemaniac/pixie/releases/tag/v0.5.0
