/*
 * cgram.y - Merged adaptation of Dr. Jeffery's adaptation of Sigala's
 * grammar.
 */

/*
 * Grammar for 120++, a subset of C++ used in CS 120 at University of Idaho
 *
 * Adaptation by Clinton Jeffery, with help from Matthew Brown, Ranger
 * Adams, and Shea Newton.
 *
 * Based on Sandro Sigala's transcription of the ISO C++ 1996 draft standard.
 *
 */

/*
 * Copyright (c) 1997 Sandro Sigala <ssigala@globalnet.it>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * ISO C++ parser.
 *
 * Based on the ISO C++ draft standard of December '96.
 */

%{
#include <stdio.h>

void yyerror(const char *s);
%}

%union {
    struct tree *t;
}

%defines

%token <t> IDENTIFIER INTEGER FLOATING CHARACTER STRING CLASS_NAME
%token <t> COLONCOLON DOTSTAR ADDEQ SUBEQ MULEQ DIVEQ MODEQ XOREQ
%token <t> ANDEQ OREQ SL SR SREQ SLEQ EQ NOTEQ LTEQ GTEQ ANDAND OROR
%token <t> PLUSPLUS MINUSMINUS ARROWSTAR ARROW BOOL BREAK CASE CHAR
%token <t> CLASS CONTINUE DEFAULT DELETE DO DOUBLE ELSE FALSE FLOAT
%token <t> FOR IF INT LONG NEW PRIVATE PROTECTED PUBLIC RETURN SHORT
%token <t> SIGNED SIZEOF STRUCT SWITCH TRUE UNSIGNED VOID WHILE

%type <t> identifier literal integer_literal character_literal
%type <t> floating_literal string_literal boolean_literal
%type <t> translation_unit primary_expression id_expression
%type <t> unqualified_id qualified_id nested_name_specifier
%type <t> postfix_expression expression_list unary_expression
%type <t> unary_operator new_expression new_placement new_type_id
%type <t> new_declarator direct_new_declarator new_initializer
%type <t> delete_expression pm_expression multiplicative_expression
%type <t> additive_expression shift_expression relational_expression
%type <t> equality_expression and_expression exclusive_or_expression
%type <t> inclusive_or_expression logical_and_expression
%type <t> logical_or_expression conditional_expression
%type <t> assignment_expression assignment_operator expression
%type <t> constant_expression statement labeled_statement
%type <t> expression_statement compound_statement statement_seq
%type <t> selection_statement condition iteration_statement
%type <t> for_init_statement jump_statement declaration_statement
%type <t> declaration_seq declaration block_declaration
%type <t> simple_declaration decl_specifier decl_specifier_seq
%type <t> type_specifier simple_type_specifier type_name
%type <t> elaborated_type_specifier init_declarator_list
%type <t> init_declarator declarator direct_declarator ptr_operator
%type <t> declarator_id type_id type_specifier_seq abstract_declarator
%type <t> direct_abstract_declarator parameter_declaration_clause
%type <t> parameter_declaration_list parameter_declaration
%type <t> function_definition function_body initializer
%type <t> initializer_clause initializer_list class_specifier
%type <t> class_head class_key member_specification member_declaration
%type <t> member_declarator_list member_declarator
%type <t> constant_initializer base_clause base_specifier_list
%type <t> base_specifier access_specifier ctor_initializer
%type <t> mem_initializer_list mem_initializer mem_initializer_id
%type <t> declaration_seq_opt expression_list_opt new_placement_opt
%type <t> new_initializer_opt new_declarator_opt expression_opt
%type <t> statement_seq_opt condition_opt initializer_opt
%type <t> constant_expression_opt abstract_declarator_opt
%type <t> type_specifier_seq_opt ctor_initializer_opt COMMA_opt
%type <t> member_specification_opt SEMICOLON_opt

%start translation_unit

%%

/*----------------------------------------------------------------------
 * Context-dependent identifiers.
 *----------------------------------------------------------------------*/

class_name:
	/* identifier */
	CLASS_NAME
	;

/*----------------------------------------------------------------------
 * Lexical elements.
 *----------------------------------------------------------------------*/

identifier:
	IDENTIFIER
	;

literal:
	integer_literal
	| character_literal
	| floating_literal
	| string_literal
	| boolean_literal
	;

integer_literal:
	INTEGER
	;

character_literal:
	CHARACTER
	;

floating_literal:
	FLOATING
	;

string_literal:
	STRING
	;

boolean_literal:
	TRUE
	| FALSE
	;

/*----------------------------------------------------------------------
 * Translation unit.
 *----------------------------------------------------------------------*/

translation_unit:
	declaration_seq_opt
	;

/*----------------------------------------------------------------------
 * Expressions.
 *----------------------------------------------------------------------*/

primary_expression:
	literal
	| '(' expression ')'
	| id_expression
	;

id_expression:
	unqualified_id
	| qualified_id
	;

unqualified_id:
	identifier
	| '~' class_name
	;

qualified_id:
	nested_name_specifier unqualified_id
	;

nested_name_specifier:
	class_name COLONCOLON
	| class_name COLONCOLON nested_name_specifier
	;

postfix_expression:
	primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' expression_list_opt ')'
	| simple_type_specifier '(' expression_list_opt ')'
	| postfix_expression '.' COLONCOLON id_expression
	| postfix_expression '.' id_expression
	| postfix_expression ARROW COLONCOLON id_expression
	| postfix_expression ARROW id_expression
	| postfix_expression PLUSPLUS
	| postfix_expression MINUSMINUS
	;

expression_list:
	assignment_expression
	| expression_list ',' assignment_expression
	;

unary_expression:
	postfix_expression
	| PLUSPLUS unary_expression
	| MINUSMINUS unary_expression
	| unary_operator unary_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_id ')'
	| new_expression
	| delete_expression
	;

unary_operator:
	'*'
	| '&'
	| '+'
	| '-'
	| '!'
	| '~'
	;

new_expression:
	  NEW new_placement_opt new_type_id new_initializer_opt
	| COLONCOLON NEW new_placement_opt new_type_id new_initializer_opt
	;

new_placement:
	'(' expression_list ')'
	;

new_type_id:
	type_specifier_seq new_declarator_opt
	;

new_declarator:
	ptr_operator new_declarator_opt
	| direct_new_declarator
	;

direct_new_declarator:
	'[' expression ']'
	| direct_new_declarator '[' constant_expression ']'
	;

new_initializer:
	'(' expression_list_opt ')'
	;

delete_expression:
	  DELETE unary_expression
	| COLONCOLON DELETE unary_expression
	| DELETE '[' ']' unary_expression
	| COLONCOLON DELETE '[' ']' unary_expression
	;

pm_expression:
	unary_expression
	| pm_expression DOTSTAR unary_expression
	| pm_expression ARROWSTAR unary_expression
	;

multiplicative_expression:
	pm_expression
	| multiplicative_expression '*' pm_expression
	| multiplicative_expression '/' pm_expression
	| multiplicative_expression '%' pm_expression
	;

additive_expression:
	multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

shift_expression:
	additive_expression
	| shift_expression SL additive_expression
	| shift_expression SR additive_expression
	;

relational_expression:
	shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LTEQ shift_expression
	| relational_expression GTEQ shift_expression
	;

equality_expression:
	relational_expression
	| equality_expression EQ relational_expression
	| equality_expression NOTEQ relational_expression
	;

and_expression:
	equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression:
	and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression:
	exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression:
	inclusive_or_expression
	| logical_and_expression ANDAND inclusive_or_expression
	;

logical_or_expression:
	logical_and_expression
	| logical_or_expression OROR logical_and_expression
	;

conditional_expression:
	logical_or_expression
	| logical_or_expression  '?' expression ':' assignment_expression
	;

assignment_expression:
	conditional_expression
	| logical_or_expression assignment_operator assignment_expression
	;

assignment_operator:
	'='
	| MULEQ
	| DIVEQ
	| MODEQ
	| ADDEQ
	| SUBEQ
	| SREQ
	| SLEQ
	| ANDEQ
	| XOREQ
	| OREQ
	;

expression:
	assignment_expression
	| expression ',' assignment_expression
	;

constant_expression:
	conditional_expression
	;

/*----------------------------------------------------------------------
 * Statements.
 *----------------------------------------------------------------------*/

statement:
	labeled_statement
	| expression_statement
	| compound_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	| declaration_statement
	;

labeled_statement:
	CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

expression_statement:
	expression_opt ';'
	;

compound_statement:
	'{' statement_seq_opt '}'
	;

statement_seq:
	statement
	| statement_seq statement
	;

selection_statement:
	IF '(' condition ')' statement
	| IF '(' condition ')' statement ELSE statement
	| SWITCH '(' condition ')' statement
	;

condition:
	expression
	| type_specifier_seq declarator '=' assignment_expression
	;

iteration_statement:
	WHILE '(' condition ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' for_init_statement condition_opt ';' expression_opt ')' statement
	;

for_init_statement:
	expression_statement
	| simple_declaration
	;

jump_statement:
	BREAK ';'
	| CONTINUE ';'
	| RETURN expression_opt ';'
	;

declaration_statement:
	block_declaration
	;

/*----------------------------------------------------------------------
 * Declarations.
 *----------------------------------------------------------------------*/

declaration_seq:
	declaration
	| declaration_seq declaration
	;

declaration:
	block_declaration
	| function_definition
	;

block_declaration:
	simple_declaration
	;

simple_declaration:
	  decl_specifier_seq init_declarator_list ';'
	|  decl_specifier_seq ';'
	;

decl_specifier:
	type_specifier
	;

decl_specifier_seq:
	  decl_specifier
	| decl_specifier_seq decl_specifier
	;

type_specifier:
	simple_type_specifier
	| class_specifier
	| elaborated_type_specifier
	;

simple_type_specifier:
	  type_name
	| nested_name_specifier type_name
	| CHAR
	| BOOL
	| SHORT
	| INT
	| LONG
	| SIGNED
	| UNSIGNED
	| FLOAT
	| DOUBLE
	| VOID
	;

type_name:
	class_name
	;

elaborated_type_specifier:
	  class_key COLONCOLON nested_name_specifier identifier
	| class_key COLONCOLON identifier
	;

/*----------------------------------------------------------------------
 * Declarators.
 *----------------------------------------------------------------------*/

init_declarator_list:
	init_declarator
	| init_declarator_list ',' init_declarator
	;

init_declarator:
	declarator initializer_opt
	;

declarator:
	direct_declarator
	| ptr_operator declarator
	;

direct_declarator:
	declarator_id
	| direct_declarator '('parameter_declaration_clause ')'
	| direct_declarator '[' constant_expression_opt ']'
	| '(' declarator ')'
	;

ptr_operator:
	'*'
	| '&'
	| nested_name_specifier '*'
	| COLONCOLON nested_name_specifier '*'
	;

declarator_id:
	  id_expression
	| COLONCOLON id_expression
	| COLONCOLON nested_name_specifier type_name
	| COLONCOLON type_name
	;

type_id:
	type_specifier_seq abstract_declarator_opt
	;

type_specifier_seq:
	type_specifier type_specifier_seq_opt
	;

abstract_declarator:
	ptr_operator abstract_declarator_opt
	| direct_abstract_declarator
	;

direct_abstract_declarator:
         '(' parameter_declaration_clause ')'
	| direct_abstract_declarator '(' parameter_declaration_clause ')'
	| '[' constant_expression_opt ']'
	| direct_abstract_declarator '[' constant_expression_opt ']'
	| '(' abstract_declarator ')'
	;

parameter_declaration_clause:
	%empty
	| parameter_declaration_list
	;

parameter_declaration_list:
	parameter_declaration
	| parameter_declaration_list ',' parameter_declaration
	;

parameter_declaration:
	decl_specifier_seq declarator
	| decl_specifier_seq declarator '=' assignment_expression
	| decl_specifier_seq abstract_declarator_opt
	| decl_specifier_seq abstract_declarator_opt '=' assignment_expression
	;

function_definition:
	  declarator ctor_initializer_opt function_body
	| decl_specifier_seq declarator ctor_initializer_opt function_body
	;

function_body:
	compound_statement
	;

initializer:
	'=' initializer_clause
	| '(' expression_list ')'
	;

initializer_clause:
	assignment_expression
	| '{' initializer_list COMMA_opt '}'
	| '{' '}'
	;

initializer_list:
	initializer_clause
	| initializer_list ',' initializer_clause
	;

/*----------------------------------------------------------------------
 * Classes.
 *----------------------------------------------------------------------*/

class_specifier:
	class_head '{' member_specification_opt '}'
	;

class_head:
	  class_key identifier
	| class_key identifier base_clause
	| class_key nested_name_specifier identifier
	| class_key nested_name_specifier identifier base_clause
	;

class_key:
	CLASS
	| STRUCT
	;

member_specification:
	member_declaration member_specification_opt
	| access_specifier ':' member_specification_opt
	;

member_declaration:
	  decl_specifier_seq member_declarator_list ';'
	| decl_specifier_seq ';'
	| member_declarator_list ';'
	| ';'
	| function_definition SEMICOLON_opt
	| qualified_id ';'
	;

member_declarator_list:
	member_declarator
	| member_declarator_list ',' member_declarator
	;

member_declarator:
	declarator
	| declarator constant_initializer
	| identifier ':' constant_expression
	;

constant_initializer:
	'=' constant_expression
	;

/*----------------------------------------------------------------------
 * Derived classes.
 *----------------------------------------------------------------------*/

base_clause:
	':' base_specifier_list
	;

base_specifier_list:
	base_specifier
	| base_specifier_list ',' base_specifier
	;

base_specifier:
	  COLONCOLON nested_name_specifier class_name
	| COLONCOLON class_name
	| nested_name_specifier class_name
	| class_name
	| access_specifier COLONCOLON nested_name_specifier class_name
	| access_specifier COLONCOLON class_name
	| access_specifier nested_name_specifier class_name
	| access_specifier class_name
	;

access_specifier:
	PRIVATE
	| PROTECTED
	| PUBLIC
	;

/*----------------------------------------------------------------------
 * Special member functions.
 *----------------------------------------------------------------------*/

ctor_initializer:
	':' mem_initializer_list
	;

mem_initializer_list:
	mem_initializer
	| mem_initializer ',' mem_initializer_list
	;

mem_initializer:
	mem_initializer_id '(' expression_list_opt ')'
	;

mem_initializer_id:
	  COLONCOLON nested_name_specifier class_name
	| COLONCOLON class_name
	| nested_name_specifier class_name
	| class_name
	| identifier
	;

/*----------------------------------------------------------------------
 * Epsilon (optional) definitions.
 *----------------------------------------------------------------------*/

declaration_seq_opt:
	%empty
	| declaration_seq
	;

expression_list_opt:
	%empty
	| expression_list
	;

new_placement_opt:
	%empty
	| new_placement
	;

new_initializer_opt:
	%empty
	| new_initializer
	;

new_declarator_opt:
	%empty
	| new_declarator
	;

expression_opt:
	%empty
	| expression
	;

statement_seq_opt:
	%empty
	| statement_seq
	;

condition_opt:
	%empty
	| condition
	;

initializer_opt:
	%empty
	| initializer
	;

constant_expression_opt:
	%empty
	| constant_expression
	;

abstract_declarator_opt:
	%empty
	| abstract_declarator
	;

type_specifier_seq_opt:
	%empty
	| type_specifier_seq
	;

ctor_initializer_opt:
	%empty
	| ctor_initializer
	;

COMMA_opt:
	%empty
	| ','
	;

member_specification_opt:
	%empty
	| member_specification
	;

SEMICOLON_opt:
	%empty
	| ';'
	;

%%
