# compiler

## DIFF CHECKLIST
- [x] 001 
- [x] 002
- [x] 003
- [x] 004
- [ ] 005
- [ ] 006
- [ ] 007
- [ ] 008
- [ ] 009
- [ ] 010
- [ ] 011
- [ ] 012
- [ ] 013
- [ ] 014
- [ ] 015
- [ ] 016
- [ ] 017
- [x] 018
- [ ] 019
- [ ] 020
- [ ] 021
- [ ] 022
- [ ] 023
- [ ] 024
- [ ] 025
- [x] 026
- [ ] 027
- [x] 028
- [x] 029
- [ ] 030
- [x] 031
- [x] 032
- [x] 033
- [x] 034
- [x] 035
- [x] 036
- [ ] 037
- [ ] 038
- [ ] 039
- [ ] 040
- [ ] 041
- [ ] 042
- [ ] 043
- [ ] 044
- [ ] 045
- [ ] 046
- [ ] 047
- [ ] 048
- [ ] 049
- [ ] 050
- [ ] 051
- [ ] 052

## Compiler project for Algorithmic Languages and Compilers — Fall 2022

CS 4301
Jeff Caldwell
Kangmin Kim

# Lecture references
## First look at project
Week 9, October 20, 50:00

## Producing two other files
- Listing file
- Object file
- Errors should be printed to listing file and compilation should stop
  - Motl says "You should find that first error and stop"

- Symbol table entries will be stored as maps
  - key will be the name the user has given to the symbol (external name)
  - value: internal name, data type, data mode, value, allocation, units
    - Values added during `insert statement` in `progStmt`

_Womack gave us some scripts!_

`setup.sh` and `createData.sh`

To run them:

```bash
chmod 771 setup.sh
chmod 771 createData.sh
```

"setup can be ran anywhere and will make a data folder in that directory, createData needs to be moved into the data folder and will create an output file and diffs and whatnot"

"Note that to use to use shell scripts after the chmod they can be activated like:"

```bash
./setup.sh
```
"Note that this does not make a stage0.cpp in the data folder. You need to copy or move one in there before calling createData 

Also calling setup again will clean out and remake said data folder"

"Also if your wondering why it says you have diffs and there's no obvious difference, instead of using"

```bash
<< "\n"

Do instead 

<< "\r\n"
```

# Notes

The focus of Stage 0 is just the _declaration of our data_.

_We have the benefit of being in very different time zones! One of us can be working on the project at almost all times this weekend._

_Check the lecture from November 3, 2022 — Dr. Motl talks about the need to patch something that's not in the pseudocode._

Page 9 of `Overall Compiler Structure` near the code block:

```
if (y == "not")
{
if (nextToken() is not a BOOLEAN)
processError(boolean expected after “not”)
if (token == "true")
y = "false"
else
y = "true"
}
```

He also mentions something about a `while` loop

_Motl says `unit` will be either `0` or `1`_