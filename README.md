# compiler
## Compiler project for Algorithmic Languages and Compilers — Fall 2022

CS 4301
Jeff Caldwell
Kangmin Kim

# Lecture references
## First look at project
Week 9, October 20, 50:00

# Notes

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