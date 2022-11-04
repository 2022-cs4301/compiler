# compiler
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
    - Values added during `insert statement`
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