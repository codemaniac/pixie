# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Board represented using Bitboards
- Move generation using Magic Bitboards
- Perft and Divide to test move generation, make, and unmake moves
- Negamax search with alpha-beta pruning and Transposition Tables
- Quiescence to tackle horizon effect
- Integration with UCI protocol

### Tested Platformms
- macOS Ventura on 2.3 GHz Dual-Core Intel Core i5 64-bit CPU with 8 GB 2133 MHz LPDDR3 Memory
- Ubuntu 24.04 (LTS) x64 on 4-core Intel processor with 8 GB Memory
