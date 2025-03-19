This project was completed in the second year of college, first semester, as part of the Formal Languages, Automata, and Compilers course. For this project I worked together with a classmate, and we both had equal involvement in all of the parts. It is a basic compiler for a C++-like language, and it implements the following:

1) Syntax—YACC/BISON language

- type declarations:

	- predefined types (int, float, char, string, bool), 

	- array types 

- classes:

	- provides specific syntax to allow initialization and use of objects

	- provides specific syntax for accessing fields and methods

	- allows classes to be defined only in the global scope

- variable declarations/definitions, function definitions

- control statements (if, for, while), assignment statements 

- arithmetic and boolean expressions

- two predefined functions: Print(expr) and TypeOf(expr) 

2) Symbol tables—C++ language

- creates a symbol table for every scope in the program. 

3) Semantic analysis—YACC/BISON language

- Verifies that:

	- any variable or function used in a program has been previously defined

	- a variable should not be declared more than once in the same scope			- all the operands on the right side of an expression must have the same type (the language does not support casting)	

	- the left side of an assignment has the same type as the right side (the left side can be an element of an array, an identifier, etc)

	- the parameters of a function call have the types from the function definition

4) Evaluation of arithmetic expressions and boolean expressions—C++ language

- the evaluation of arithmetic expressions and boolean expressions using Abstract Syntax Trees (AST)
