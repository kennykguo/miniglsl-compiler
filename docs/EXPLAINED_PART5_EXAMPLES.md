# The Complete Compiler Walkthrough - Part 5: Complete Examples & Data Flow

**End-to-End Compilation Tracing**

---

## Purpose of This Part

Parts 1-4 explained each stage separately. Now we'll trace **complete examples** from start to finish, showing exactly how data transforms through the entire pipeline.

---

## Example 1: Simple Variable Declaration

**Input code:**
```c
{
    float x = 1.5;
}
```

### Stage 1: Scanner

**Input characters:**
```
{ \n   f l o a t   x   =   1 . 5 ; \n }
```

**Token stream:**
```
LBRACE
FLOAT_T
ID("x")
ASSIGN
FLOAT_C(1.5)
SEMICOLON
RBRACE
```

**Details:**
- `float` matches keyword pattern → `FLOAT_T`
- `x` matches identifier pattern `[a-zA-Z_][a-zA-Z0-9_]*` → `ID("x")`
- `1.5` matches float pattern `[0-9]+\.[0-9]+` → `FLOAT_C(1.5)`
- Whitespace (spaces, newlines) skipped

### Stage 2: Parser

**Token stream → AST**

**Grammar match:**
```yacc
scope: '{' statement_list '}'
statement_list: statement_list declaration
declaration: FLOAT_T ID ASSIGN expression SEMICOLON
expression: FLOAT_LITERAL
```

**AST produced:**
```
SCOPE_NODE
  ├─ declarations:
  │   └─ DECLARATION_NODE
  │       ├─ type: FLOAT_T (type_code=FLOAT_T, vec_size=0)
  │       ├─ identifier: "x"
  │       ├─ is_const: false
  │       └─ initializer: FLOAT_LITERAL_NODE
  │           └─ val: 1.5
  └─ statements: NULL
```

**In memory (simplified):**
```c
node *scope = allocate_node(SCOPE_NODE);
scope->scope.declarations = allocate_node(DECLARATION_NODE);
scope->scope.declarations->declaration.type = {FLOAT_T, 0};
scope->scope.declarations->declaration.identifier = "x";
scope->scope.declarations->declaration.initializer = allocate_node(FLOAT_LITERAL_NODE);
scope->scope.declarations->declaration.initializer->float_literal.val = 1.5;
scope->scope.statements = NULL;
```

### Stage 3: Semantic Analysis

**Pass 1: Add to symbol table**
```c
symbol_entry *x_entry = {
    name: "x",
    type_code: FLOAT_T,
    vec_size: 0,
    is_const: 0,
    is_read_only: 0,
    is_write_only: 0
};
symbol_table_insert(current_scope, x_entry);
```

**Pass 2: Check initializer**
```c
check_expression(FLOAT_LITERAL_NODE(1.5))
  → returns FLOAT_T
  → matches declaration type ✓
```

**Symbol table state:**
```
Current scope:
  { "x" → (float, not const) }
Parent scope:
  { "gl_Color" → (vec4, read-only),
    "gl_FragColor" → (vec4, write-only),
    ... }
```

**Result:** Validation passed ✓

### Stage 4: Code Generation

**Phase 1: Allocate variables**
```c
next_id = 100
var_id = gen_id()  // 100

Add to local_vars: {"x", 100, FLOAT_T, 0}

Output: %100 = OpVariable %t_ptr_func_float Function
```

**Phase 2: Initialize**
```c
// Generate initializer expression
gen_expression(FLOAT_LITERAL_NODE(1.5))
  → const_id = gen_id()  // 101
  → add_const(101, "%101 = OpConstant %t_float 1.5\n")
  → return 101

// Store initializer
Output: OpStore %100 %101
```

**Final SPIR-V:**
```spirv
; ===== Prologue (fixed) =====
OpCapability Shader
%ext = OpExtInstImport "GLSL.std.450"
...
%t_float = OpTypeFloat 32
%t_ptr_func_float = OpTypePointer Function %t_float
...

; ===== Constants =====
%101 = OpConstant %t_float 1.5

; ===== Function =====
%main = OpFunction %t_void None %t_fn
%entry = OpLabel

; Variables
%100 = OpVariable %t_ptr_func_float Function

; Initialization
OpStore %100 %101

OpReturn
OpFunctionEnd
```

---

## Example 2: Binary Expression with Temporary

**Input code:**
```c
{
    float x = 1.0;
    float y = x + 2.0;
}
```

### Stage 1: Scanner

**Tokens (only the relevant part):**
```
...
FLOAT_T ID("y") ASSIGN ID("x") '+' FLOAT_C(2.0) SEMICOLON
...
```

### Stage 2: Parser

**Declaration AST for `y`:**
```
DECLARATION_NODE
  ├─ type: FLOAT_T
  ├─ identifier: "y"
  └─ initializer: BINARY_EXPR_NODE
      ├─ op: '+'
      ├─ left: VAR_NODE("x")
      └─ right: FLOAT_LITERAL_NODE(2.0)
```

### Stage 3: Semantic Analysis

**Pass 1:** Add "x" and "y" to symbol table

**Pass 2:** Check y's initializer
```c
check_expression(BINARY_EXPR_NODE('+'))
  → check_expression(VAR_NODE("x"))
      → lookup "x" in symbol table → found ✓
      → return FLOAT_T
  → check_expression(FLOAT_LITERAL(2.0))
      → return FLOAT_T
  → left type == right type ✓
  → return FLOAT_T (result type)
  
Result type (FLOAT_T) matches declaration type ✓
```

### Stage 4: Code Generation

**Phase 1: Allocate**
```spirv
%100 = OpVariable %t_ptr_func_float Function  ; x
%101 = OpVariable %t_ptr_func_float Function  ; y
```

**Phase 2: Initialize**

**For x = 1.0:**
```spirv
%102 = OpConstant %t_float 1.0  ; (added to const pool)
OpStore %100 %102
```

**For y = x + 2.0:**
```c
// Generate right side: x + 2.0
gen_expression(BINARY_EXPR_NODE('+'))
  
  // Generate left: x
  → gen_expression(VAR_NODE("x"))
      → var_id = lookup_var("x")  // 100
      → result_id = gen_id()  // 103
      → Output: %103 = OpLoad %t_float %100
      → return 103
      
  // Generate right: 2.0
  → gen_expression(FLOAT_LITERAL(2.0))
      → const_id = gen_id()  // 104
      → add_const(104, "%104 = OpConstant %t_float 2.0\n")
      → return 104
  
  // Generate addition
  → result_id = gen_id()  // 105
  → Output: %105 = OpFAdd %t_float %103 %104
  → return 105

// Store in y
Output: OpStore %101 %105
```

**Final SPIR-V:**
```spirv
; Constants
%102 = OpConstant %t_float 1.0
%104 = OpConstant %t_float 2.0

; Function
%main = OpFunction %t_void None %t_fn
%entry = OpLabel

; Variables
%100 = OpVariable %t_ptr_func_float Function  ; x
%101 = OpVariable %t_ptr_func_float Function  ; y

; Initialization
; x = 1.0
OpStore %100 %102

; y = x + 2.0
%103 = OpLoad %t_float %100          ; Load x
%105 = OpFAdd %t_float %103 %104     ; x + 2.0
OpStore %101 %105                     ; Store in y

OpReturn
OpFunctionEnd
```

**Notice:** The temporary %103 (loaded x) and %105 (addition result) are automatically created during expression generation!

---

## Example 3: Nested Expression

**Input code:**
```c
{
    float x = 1.0;
    float y = 2.0;
    float z = (x + y) * 3.0;
}
```

**Focus on z's initializer:** `(x + y) * 3.0`

### Parser AST

```
BINARY_EXPR_NODE('*')
  ├─ left: BINARY_EXPR_NODE('+')
  │   ├─ left: VAR_NODE("x")
  │   └─ right: VAR_NODE("y")
  └─ right: FLOAT_LITERAL_NODE(3.0)
```

### Code Generation (Recursive!)

```c
gen_expression(BINARY_EXPR_NODE('*'))
  
  // Generate left: (x + y)
  left_id = gen_expression(BINARY_EXPR_NODE('+'))
    
    // Generate left: x
    → gen_expression(VAR_NODE("x"))
        → %100 = OpLoad %t_float %x_var
        → return 100
    
    // Generate right: y
    → gen_expression(VAR_NODE("y"))
        → %101 = OpLoad %t_float %y_var
        → return 101
    
    // Generate add
    → %102 = OpFAdd %t_float %100 %101
    → return 102  // This becomes left_id!
  
  // Generate right: 3.0
  right_id = gen_expression(FLOAT_LITERAL(3.0))
    → %103 = OpConstant %t_float 3.0
    → return 103
  
  // Generate multiply
  →%104 = OpFMul %t_float %102 %103  // Multiply (x+y) by 3.0
  → return 104
```

**Generated SPIR-V:**
```spirv
%100 = OpLoad %t_float %x_var        ; x
%101 = OpLoad %t_float %y_var        ; y
%102 = OpFAdd %t_float %100 %101     ; x + y
%103 = OpConstant %t_float 3.0       ; 3.0
%104 = OpFMul %t_float %102 %103     ; (x + y) * 3.0
OpStore %z_var %104
```

**Key insight:** Recursive calls to `gen_expression` automatically handle nesting! The call stack matches the tree structure.

---

## Example 4: If Statement with OpSelect

**Input code:**
```c
{
    float x = 1.0;
    if (x < 2.0) {
        x = 10.0;
    } else {
        x = 20.0;
    }
}
```

### Parser AST

```
SCOPE_NODE
  ├─ declarations:
  │   └─ DECLARATION_NODE("x", float, 1.0)
  └─ statements:
      └─ IF_STMT_NODE
          ├─ condition: BINARY_EXPR_NODE('<')
          │   ├─ left: VAR_NODE("x")
          │   └─ right: FLOAT_LITERAL(2.0)
          ├─ then_stmt: ASSIGNMENT_NODE
          │   ├─ variable: VAR_NODE("x")
          │   └─ expr: FLOAT_LITERAL(10.0)
          └─ else_stmt: ASSIGNMENT_NODE
              ├─ variable: VAR_NODE("x")
              └─ expr: FLOAT_LITERAL(20.0)
```

### Code Generation Step-by-Step

**1. Allocate x:**
```spirv
%100 = OpVariable %t_ptr_func_float Function
```

**2. Initialize x:**
```spirv
%101 = OpConstant %t_float 1.0
OpStore %100 %101
```

**3. Generate if statement:**

```c
gen_statement(IF_STMT_NODE)
  
  // 1. Generate condition: x < 2.0
  cond_id = gen_expression(BINARY_EXPR('<'))
    → left_id = gen_expression(VAR_NODE("x"))
        → %102 = OpLoad %t_float %100
        → return 102
    → right_id = gen_expression(FLOAT_LITERAL(2.0))
        → %103 = OpConstant %t_float 2.0
        → return 103
    → %104 = OpFOrdLessThan %t_bool %102 %103
    → return 104  // This is cond_id
  
  // 2. Generate then value: 10.0
  then_val = gen_expression(FLOAT_LITERAL(10.0))
    → %105 = OpConstant %t_float 10.0
    → return 105
  
  // 3. Generate else value: 20.0
  else_val = gen_expression(FLOAT_LITERAL(20.0))
    → %106 = OpConstant %t_float 20.0
    → return 106
  
  // 4. Select based on condition
  result = gen_id()  // 107
  Output: %107 = OpSelect %t_float %104 %105 %106
  
  // 5. Store in x
  Output: OpStore %100 %107
```

**Final SPIR-V:**
```spirv
; Constants
%101 = OpConstant %t_float 1.0
%103 = OpConstant %t_float 2.0
%105 = OpConstant %t_float 10.0
%106 = OpConstant %t_float 20.0

; Function
%main = OpFunction %t_void None %t_fn
%entry = OpLabel

; Variable
%100 = OpVariable %t_ptr_func_float Function  ; x

; Initialize
OpStore %100 %101  ; x = 1.0

; If statement
%102 = OpLoad %t_float %100                      ; Load x
%104 = OpFOrdLessThan %t_bool %102 %103        ; x < 2.0
%107 = OpSelect %t_float %104 %105 %106         ; Select 10.0 or 20.0
OpStore %100 %107                                ; Store result in x

OpReturn
OpFunctionEnd
```

**Execution trace (if x=1.0):**
1. `%104 = OpFOrdLessThan %102 %103` → `%104 = true` (1.0 < 2.0)
2. `%107 = OpSelect %104 %105 %106` → `%107 = %105` (select true value)
3. `OpStore %100 %107` → x becomes 10.0

**No branching!** Linear flow from top to bottom.

---

## Example 5: Vector Operations

**Input code:**
```c
{
    vec4 color = vec4(1.0, 0.5, 0.2, 1.0);
    float r = color[0];
    color[1] = 0.8;
}
```

### Interesting Parts

**1. Vector constructor:**

**AST:**
```
CONSTRUCTOR_NODE(vec4)
  arguments: LIST_NODE chain of 4 FLOAT_LITERAL nodes
```

**Code generation:**
```c
gen_expression(CONSTRUCTOR_NODE)
  → %100 = OpConstant %t_float 1.0
  → %101 = OpConstant %t_float 0.5
  → %102 = OpConstant %t_float 0.2
  → %103 = OpConstant %t_float 1.0
  → %104 = OpCompositeConstruct %t_vec4 %100 %101 %102 %103
  → return 104
```

**2. Vector indexing (read): `color[0]`**

**AST:**
```
VAR_NODE("color", is_array=true, array_index=0)
```

**Code generation:**
```c
gen_expression(VAR_NODE with is_array=true)
  → loaded = gen_id()  // 105
  → Output: %105 = OpLoad %t_vec4 %color_var
  → result = gen_id()  // 106
  → Output: %106 = OpCompositeExtract %t_float %105 0
  → return 106
```

**SPIR-V:**
```spirv
%105 = OpLoad %t_vec4 %color_var_id        ; Load [1.0, 0.5, 0.2, 1.0]
%106 = OpCompositeExtract %t_float %105 0  ; Extract component 0 → 1.0
```

**3. Vector indexing (write): `color[1] = 0.8`**

**AST:**
```
ASSIGNMENT_NODE
  ├─ variable: VAR_NODE("color", is_array=true, array_index=1)
  └─ expr: FLOAT_LITERAL(0.8)
```

**Code generation:**
```c
gen_statement(ASSIGNMENT with is_array=true)
  → rhs_id = gen_expression(FLOAT_LITERAL(0.8))
      → %107 = OpConstant %t_float 0.8
      → return 107
  
  → loaded = gen_id()  // 108
  → Output: %108 = OpLoad %t_vec4 %color_var_id
  
  → updated = gen_id()  // 109
  → Output: %109 = OpCompositeInsert %t_vec4 %107 %108 1
  
  → Output: OpStore %color_var_id %109
```

**SPIR-V:**
```spirv
%107 = OpConstant %t_float 0.8                  ; New value
%108 = OpLoad %t_vec4 %color_var_id             ; Load [1.0, 0.5, 0.2, 1.0]
%109 = OpCompositeInsert %t_vec4 %107 %108 1   ; Insert 0.8 at index 1
OpStore %color_var_id %109                      ; Store [1.0, 0.8, 0.2, 1.0]
```

**Why 3 steps?**
- Can't modify vector in-place (SSA = immutable)
- Must create new vector with modified component
- `OpCompositeInsert` creates new vector

---

## Example 6: Function Call (dp3)

**Input code:**
```c
{
    vec3 a = vec3(1.0, 0.0, 0.0);
    vec3 b = vec3(0.0, 1.0, 0.0);
    float dot = dp3(a, b);
}
```

### dp3 Call Generation

**AST:**
```
FUNCTION_NODE(DP3)
  arguments:
    ├─ VAR_NODE("a")
    └─ VAR_NODE("b")
```

**Code generation:**
```c
gen_expression(FUNCTION_NODE with DP3)
  → arg1_id = gen_expression(VAR_NODE("a"))
      → %100 = OpLoad %t_vec3 %a_var
      → return 100
  
  → arg2_id = gen_expression(VAR_NODE("b"))
      → %101 = OpLoad %t_vec3 %b_var
      → return 101
  
  → result = gen_id()  // 102
  → Output: %102 = OpExtInst %t_float %ext Dot %100 %101
  → return 102
```

**SPIR-V:**
```spirv
%100 = OpLoad %t_vec3 %a_var                   ; Load a
%101 = OpLoad %t_vec3 %b_var                   ; Load b
%102 = OpExtInst %t_float %ext Dot %100 %101  ; dot(a, b)
OpStore %dot_var %102
```

**What `OpExtInst` does:**
- Calls instruction from GLSL.std.450 extended instruction set
- `Dot` is instruction ID 4
- Takes two vec3 arguments
- Returns float (dot product result)

**Math:** `dot([1,0,0], [0,1,0]) = 1*0 + 0*1 + 0*0 = 0.0`

---

## Example 7: Complete Shader

**Input code:**
```c
{
    vec4 base = gl_Color;
    float intensity = dp3(vec3(1.0, 1.0, 1.0), base.xyz);
    
    if (intensity < 0.5) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);  // Black
    } else {
        gl_FragColor = base;
    }
}
```

### Complete Flow

**Scanner:** Text → Tokens (50+ tokens)

**Parser:** Tokens → AST with:
- 2 declarations (base, intensity)
- 1 if statement

**Semantic:**
- Pass 1: Add "base" and "intensity" to table
- Pass 2: Check all types
- validate gl_Color (Input, read-only)
- Validate gl_FragColor (Output, write-only)

**Codegen:**

**Generated SPIR-V (simplified):**
```spirv
; Constants
%c_float_0 = OpConstant %t_float 0.0
%c_float_1 = OpConstant %t_float 1.0
%c_half = OpConstant %t_float 0.5

; Function
%main = OpFunction %t_void None %t_fn
%entry = OpLabel

; Variables
%v_base = OpVariable %t_ptr_func_vec4 Function
%v_intensity = OpVariable %t_ptr_func_float Function

; base = gl_Color
%1 = OpLoad %t_vec4 %mglsl_Color
OpStore %v_base %1

; intensity = dp3(vec3(1,1,1), base.xyz)
%2 = OpCompositeConstruct %t_vec3 %c_float_1 %c_float_1 %c_float_1  ; vec3(1,1,1)
%3 = OpLoad %t_vec4 %v_base                                          ; base
%4 = OpCompositeExtract %t_float %3 0                                ; base.x
%5 = OpCompositeExtract %t_float %3 1                                ; base.y
%6 = OpCompositeExtract %t_float %3 2                                ; base.z
%7 = OpCompositeConstruct %t_vec3 %4 %5 %6                          ; base.xyz
%8 = OpExtInst %t_float %ext Dot %2 %7                               ; dot product
OpStore %v_intensity %8

; if (intensity < 0.5)
%9 = OpLoad %t_float %v_intensity                                   ; Load intensity
%10 = OpFOrdLessThan %t_bool %9 %c_half                             ; intensity < 0.5

; Generate then value: vec4(0,0,0,1)
%11 = OpCompositeConstruct %t_vec4 %c_float_0 %c_float_0 %c_float_0 %c_float_1

; Generate else value: base
%12 = OpLoad %t_vec4 %v_base

; Select
%13 = OpSelect %t_vec4 %10 %11 %12  ; Select black or base

; Store
OpStore %mglsl_FragColor %13

OpReturn
OpFunctionEnd
```

**Flow summary:**
1. Load gl_Color → store in base
2. Compute dot product → store in intensity
3. Compare intensity < 0.5
4. Evaluate both potential colors
5. Select based on condition
6. Store in gl_FragColor

**Linear!** No branching, flows top to bottom.

---

## Data Structure Summary

### Throughout the Pipeline

**Scanner:**
```
Characters → Tokens
"float" → FLOAT_T
"1.5" → FLOAT_C(1.5)
"x" → ID("x")
```

**Parser:**
```
Tokens → AST nodes
ID("x") → VAR_NODE{identifier="x"}
FLOAT_C(1.5) → FLOAT_LITERAL_NODE{val=1.5}
```

**Semantic:**
```
AST → Symbol table
VAR_NODE("x") → lookup → symbol_entry{name="x", type=FLOAT_T}
```

**Codegen:**
```
AST → SPIR-V text
VAR_NODE("x") → "%100 = OpLoad %t_float %x_var"
FLOAT_LITERAL(1.5) → "%101 = OpConstant %t_float 1.5"
```

---

## Summary: Complete Examples

**Key Takeaways:**

1. **Data flows linearly** through stages
2. **Each stage transforms** representation
3. **Recursion handles nesting** (expressions, scopes)
4. **SSA creates temporaries** automatically
5. **OpSelect is linear** - no branching needed

**Transformation chain:**
```
Text
 → Tokens (Scanner)
 → AST (Parser)
 → Validated AST (Semantic)
 → SPIR-V (Codegen)
```

**Each stage builds on previous:**
- Scanner doesn't know about types
- Parser doesn't know if variables exist
- Semantic doesn't generate code
- Codegen assumes valid AST

**Separation of concerns!**

---

**Continue to Part 6 for Design Decisions and Alternatives...**
