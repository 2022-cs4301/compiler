# compiler
## Compiler project for Algorithmic Languages and Compilers — Fall 2022

CS 4301
Jeff Caldwell
Kangmin Kim


# Notes

_Check lecture from November 3, 2022 — Dr Motl talks about the need to patch something that's not in the pseudocode._

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

_Motl says `unit` will be either `0` or `1`_