# Number-System-Mappings
# CS 3503 Assignment 2 - Data Representation and Mapping

## Author
[Nyasha Muzerengi]

## Description
Advanced data representation functions for Owl Tech ’s CPU design team.
My main file and convert was an update of my files in assignment 1 and in this case i was running additional functions which are:
oct_to_bin (octal → binary), oct_to_hex (octal → hexadecimal), hex_to_bin (hexadecimal → binary), and the signed number conversions to_sign_magnitude, to_ones_complement, and to_twos_complement (all 32-bit representations).

## What’s New
- Direct mapping functions (oct/hex/bin)
- Signed number representations
- Reused test framework from A1

## Build Instructions
‘‘‘ bash
gcc -o convert convert . c main . c
./ convert
‘‘‘

##Test Results 
Test 1: div_convert(104, 5) -> Expected: "404", Got: "404" [PASS]
Test 2: div_convert(156, 8) -> Expected: "234", Got: "234" [PASS]
Test 3: div_convert(255, 16) -> Expected: "FF", Got: "FF" [PASS]
...
Test 64: print_tables(0) -> [FORMATTED OUTPUT CHECK] Original: Binary=0 Octal=0 Decimal=0 Hex=0
Left Shift by 3: Binary=0 Octal=0 Decimal=0 Hex=0
AND with 0xFF: Binary=0 Octal=0 Decimal=0 Hex=0
[PASS]
Test 65: print_tables(1) -> [FORMATTED OUTPUT CHECK] Original: Binary=1 Octal=1 Decimal=1 Hex=1
Left Shift by 3: Binary=1000 Octal=10 Decimal=8 Hex=8
AND with 0xFF: Binary=1 Octal=1 Decimal=1 Hex=1
[PASS]
...
Summary: 100/100 tests passed

