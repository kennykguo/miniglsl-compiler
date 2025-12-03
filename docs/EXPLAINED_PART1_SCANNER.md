# The Complete Compiler Walkthrough - Part 1: Overview & Scanner

**Understanding Every Step of the MiniGLSL to SPIR-V Compiler**

---

## What is a Compiler?

A compiler translates human-readable code (source language) into machine-readable instructions (target language).

**In our case:**
- **Source:** MiniGLSL (fragment shader code like `float x = 1.0;`)
- **Target:** SPIR-V assembly (GPU instructions like `%100 = OpConstant %t_float 1.0`)

**The Journey:**
```
Your shader code (.frag file)
        ↓
[SCANNER] - Breaks text into tokens
        ↓
[PARSER] - Organizes tokens into a tree structure (AST)
        ↓
[SEMANTIC ANALYZER] - Checks if code makes sense (types, variables exist, etc.)
        ↓
[CODE GENERATOR] - Converts tree to SPIR-V instructions
        ↓
SPIR-V assembly (.spvasm file)
```

Each stage transforms the input in a different way. Let's understand each one.

---

## Stage 1: The Scanner (Lexical Analysis)

**File:** `src/scanner.l` (179 lines)

### What Does the Scanner Do?

The scanner reads your shader code **character by character** and groups them into meaningful chunks called **tokens**.

**Think of it like reading a sentence:**
- You don't read letter by letter: "T...h...e... ...c...a...t"
- You read word by word: "The" "cat" "sat"

The scanner does the same for code.

### Example: Breaking Down Input

**Input text:**
```c
float x = 1.0;
```

**Scanner's view (character by character):**
```
'f' 'l' 'o' 'a' 't' ' ' 'x' ' ' '=' ' ' '1' '.' '0' ';'
```

**Scanner's output (tokens):**
```
FLOAT_T         (keyword)
ID("x")         (identifier)
ASSIGN          (operator)
FLOAT_C(1.0)    (literal number)
SEMICOLON       (punctuation)
```

### How Does It Work?

The scanner uses **patterns** (regular expressions) to recognize different types of tokens.

#### Pattern 1: Keywords

**In scanner.l (lines 38-58):**
```c
"if"        { return IF; }
"else"      { return ELSE; }
"float"     { return FLOAT_T; }
"vec2"      { return VEC2_T; }
"vec3"      { return VEC3_T; }
"vec4"      { return VEC4_T; }
```

**What this means:**
- When scanner sees the exact text `"float"`, it creates a `FLOAT_T` token
- When scanner sees `"if"`, it creates an `IF` token

**Why it matters:** Keywords are special words in the language. `float` means "declare a floating-point variable" - it's not just any word.

#### Pattern 2: Operators

**In scanner.l (lines 59-78):**
```c
"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"<"         { return '<'; }
"<="        { return LEQ; }
"=="        { return EQ; }
"&&"        { return AND; }
"||"        { return OR; }
```

**What this means:**
- Single-character operators like `+` return themselves
- Multi-character operators like `<=` get special token names

**Why it matters:** We need to distinguish `<` from `<=` - they do different things!

#### Pattern 3: Numbers

**Float pattern (line 87):**
```c
[0-9]+\.[0-9]+ { 
    yylval.as_float = ParseFloat(yytext); 
    return FLOAT_C; 
}
```

**Breaking this down:**

**The pattern:** `[0-9]+\.[0-9]+`
- `[0-9]+` - One or more digits
- `\.` - A literal dot (escaped because `.` is special in regex)
- `[0-9]+` - One or more digits again

**Matches:** `1.0`, `3.14`, `42.5`
**Doesn't match:** `1` (no decimal), `.5` (no leading digit), `1.` (no trailing digit)

**The action:** `ParseFloat(yytext)`
- `yytext` contains the matched text (e.g., "1.0")
- `ParseFloat` converts the string "1.0" to the actual number 1.0
- Stores it in `yylval.as_float` so the parser can access it

**Why validation matters:**

**ParseFloat function (lines 160-172):**
```c
float ParseFloat(const char *text) {
    float result = strtof(text, NULL);
    if (errno == ERANGE) {
        fprintf(errorFile, "Float %s out of range\n", text);
        errorOccurred = 1;
        return 0.0;
    }
    return result;
}
```

This catches numbers that are too big or too small for a float. For example:
- `1e500` - Too big! Would overflow
- `1e-500` - Too small! Would underflow

#### Pattern 4: Identifiers (Variable Names)

**Pattern (line 101):**
```c
[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.as_str = ParseIdent(yytext);
    return ID;
}
```

**Breaking this down:**

**The pattern:** `[a-zA-Z_][a-zA-Z0-9_]*`
- `[a-zA-Z_]` - First character must be letter or underscore
- `[a-zA-Z0-9_]*` - Following characters can be letters, digits, or underscore

**Matches:** `x`, `my_variable`, `color`, `_temp`, `base123`
**Doesn't match:** `123abc` (starts with digit), `my-var` (has hyphen)

**Why these rules?** 
- Starting with a letter/underscore is a C/C++ convention
- Can't start with digit because `123` should be a number, not a variable name

### Special Case: Comments

**Pattern (lines 104-106):**
```c
"//".*\n         { ParseComment(2); }
"/*"             { ParseComment(1); }
```

**Two types of comments:**

**Single-line (`//`):**
```c
float x = 1.0;  // This is a comment
```
Everything from `//` to end of line is ignored.

**Multi-line (`/* */`):**
```c
/*
 * This is a
 * multi-line comment
 */
float x = 1.0;
```
Everything between `/*` and `*/` is ignored.

**Why ignore comments?** They're notes for humans, not instructions for the computer.

### How Scanner.l Structure Works

**Flex (the scanner generator) sections:**

**Section 1: Definitions (lines 1-9)**
```c
%{
#include <stdio.h>
#include "parser.tab.h"
...
%}
```
C code that gets copied into the generated scanner. We include headers we need.

**Section 2: Options (lines 28-33)**
```
%option yylineno
%option noyywrap
```
- `yylineno` - Track line numbers for error messages
- `noyywrap` - Only scan one file (don't ask for more input)

**Section 3: Rules (lines 35-106)**
```
pattern { action }
```
Each rule is: "when you see this pattern, do this action"

**Section 4: C Code (lines 138-177)**
```c
%%
int yywrap(void) { return 1; }
float ParseFloat(const char *text) { ... }
...
```
Helper functions used by the rules.

### Important Detail: Why Line 1 MUST Start at Column 0

**WRONG (causes errors):**
```c
   %{
   ^^ spaces before %{
```

**RIGHT:**
```c
%{
^^ starts at column 0
```

**Why?** Flex (the scanner generator) uses `%{` as a marker. It MUST be at the start of the line with no leading whitespace. This is a Flex syntax rule, not our choice.

**What happens if you violate this?**
```
scanner.l:1: bad character: 
```
Flex sees the spaces as part of the pattern section and gets confused.

### Complete Example: Scanning a Line of Code

**Input:**
```c
vec4 color = vec4(1.0, 0.5, 0.2, 1.0);
```

**Scanner's step-by-step processing:**

1. See `v` `e` `c` `4` → matches pattern `"vec4"` → output: `VEC4_T`
2. See ` ` (space) → whitespace rule → skip it (line 109)
3. See `c` `o` `l` `o` `r` → matches identifier pattern → output: `ID("color")`
4. See ` ` → skip
5. See `=` → matches `"="` → output: `ASSIGN`
6. See ` ` → skip
7. See `v` `e` `c` `4` → output: `VEC4_T`
8. See `(` → output: `LPAREN`
9. See `1.0` → matches float pattern → output: `FLOAT_C(1.0)`
10. See `,` → output: `COMMA`
11. See `0.5` → output: `FLOAT_C(0.5)`
12. ... and so on

**Final token stream:**
```
VEC4_T ID("color") ASSIGN VEC4_T LPAREN 
FLOAT_C(1.0) COMMA FLOAT_C(0.5) COMMA 
FLOAT_C(0.2) COMMA FLOAT_C(1.0) RPAREN SEMICOLON
```

This stream of tokens is what the **parser** receives as input.

### Why Scanning is Separate from Parsing

**Question:** Why not just parse the characters directly?

**Answer:** Separation of concerns!

**Scanner's job:** Recognize the "words" of the language
- Is "123.45" a valid number?
- Is "float" a keyword or variable name?
- Where do comments start and end?

**Parser's job:** Understand the "grammar" of those words
- Is `float x = 1.0;` a valid declaration?
- Does `if (x < 2.0)` have the right structure?

**Analogy:** 
- **Scanner** = Recognizing individual words in English
- **Parser** = Understanding sentence structure

You need to know what the words are before you can understand the sentence!

---

## Summary: Scanner (scanner.l)

**Purpose:** Convert text into tokens

**Input:** Raw shader code as a string of characters

**Output:** Stream of tokens with types and values

**Key Responsibilities:**
1. **Pattern matching** - Recognize keywords, operators, numbers, identifiers
2. **Whitespace handling** - Skip spaces, tabs, newlines
3. **Comment removal** - Ignore `//` and `/* */` comments
4. **Validation** - Check numbers are in valid range
5. **Position tracking** - Remember line numbers for error messages

**Important Patterns:**
- Keywords: `float`, `if`, `else`, `vec4`
- Operators: `+`, `-`, `*`, `/`, `<`, `<=`, `==`, `&&`
- Numbers: `123` (int), `1.5` (float), `true`/`false` (bool)
- Identifiers: `x`, `color`, `my_var`
- Punctuation: `{`, `}`, `(`, `)`, `;`, `,`

**Critical Syntax Rule:** Line 1 must start `%{` at column 0 (Flex requirement)

**Next Stage:** Tokens go to the Parser, which organizes them into a tree structure (AST)

---

**Continue to Part 2 for Parser explanation...**
