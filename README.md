# compiler

Note: Use Joel's `compilerFastCheck.sh`! Read the instructions. I should have the config set up properly but you'll need to copy the `stage0` dat files:

From the `compiler` directory on `csunix`:

```bash
cp /usr/local/4301/data/stage0 input
```

Then, while still in the `compiler` directory:

```bash
mkdir output
```

Once you `make` `stage0` you can run `./compilerFastCheck.sh` and it will print all kinds of magical information!

## DIFF CHECKLIST
- [x] 001 
- [x] 002
- [x] 003
- [x] 004
- [x] 005
- [x] 006
- [x] 007
- [x] 008
- [ ] 009 ? - (we catch the `=` error, but we need to catch the two `{` error first)
- [x] 010
- [x] 011
- [x] 012
- [x] 013
- [x] 014
- [x]
- [ ] 016 ? - we catch the error but no text is written to asm. Is that okay?
- [x] 017
- [x] 018
- [x] 019
- [x] 020
- [x] 021
- [x] 022
- [ ] 023 - no asm written, might be fine
- [x] 024 - no asm written, I think that's fine on this one
- [ ] 025 - I don't think the error's right on this one, should be catching the missing comment bracket
- [x] 026
- [x] 027
- [x] 028
- [x] 029
- [x] 030
- [x] 031
- [x] 032
- [x] 033
- [x] 034
- [x] 035
- [x] 036
- [x] 037
- [x] 038
- [x] 039 - double check! Looks good but no guidance in .dat
- [x] 040
- [x] 041
- [x] 042
- [x] 043
- [x] 044
- [x] 045
- [x] 046
- [x] 047
- [x] 048
- [x] 049
- [ ] 050 - I think this one is supposed to be a multiply defined error
- [ ] 051 - we should just check this one, there's a lot going on in this file
- [x] 052 - Joel submitted this one last year!

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