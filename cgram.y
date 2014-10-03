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

#include "clex.h"
#include "tree.h"

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
%token <t> ';' '{' '}' ',' ':' '=' '(' ')' '[' ']' '.' '&' '!' '~' '-'
%token <t> '+' '*' '/' '%' '<' '>' '^' '|' '?'

%type <t> literal boolean program primary_expression id_expression
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
%type <t> type_specifier simple_type_specifier
%type <t> elaborated_type_specifier init_declarator_list
%type <t> init_declarator declarator direct_declarator ptr_operator
%type <t> declarator_id type_id type_specifier_seq abstract_declarator
%type <t> direct_abstract_declarator parameter_declaration_clause
%type <t> parameter_declaration_list parameter_declaration
%type <t> function_definition function_body initializer
%type <t> initializer_clause initializer_list class_specifier
%type <t> class_head class_key member_specification member_declaration
%type <t> member_declarator_list member_declarator
%type <t> constant_initializer access_specifier ctor_initializer
%type <t> mem_initializer_list mem_initializer mem_initializer_id
%type <t> declaration_seq_opt expression_list_opt new_placement_opt
%type <t> new_initializer_opt new_declarator_opt expression_opt
%type <t> statement_seq_opt condition_opt initializer_opt
%type <t> constant_expression_opt abstract_declarator_opt
%type <t> type_specifier_seq_opt ctor_initializer_opt COMMA_opt
%type <t> member_specification_opt SEMICOLON_opt

%start program

%%

/*----------------------------------------------------------------------
 * Lexical elements.
 *----------------------------------------------------------------------*/

literal:
	INTEGER
	| CHARACTER
	| FLOATING
	| STRING
	| boolean
	;

boolean:
	TRUE
	| FALSE
	;

program:
	declaration_seq_opt
	;

/*----------------------------------------------------------------------
 * Expressions.
 *----------------------------------------------------------------------*/

primary_expression:
	literal
	| '(' expression ')' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| id_expression
	;

id_expression:
	unqualified_id
	| qualified_id
	;

unqualified_id:
	IDENTIFIER
	| '~' CLASS_NAME { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

qualified_id:
	nested_name_specifier unqualified_id { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

nested_name_specifier:
	CLASS_NAME COLONCOLON { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
| CLASS_NAME COLONCOLON nested_name_specifier { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

postfix_expression:
	primary_expression
	| postfix_expression '[' expression ']' { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	| postfix_expression '(' expression_list_opt ')' { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	| simple_type_specifier '(' expression_list_opt ')' { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	| postfix_expression '.' COLONCOLON id_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| postfix_expression '.' id_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| postfix_expression ARROW COLONCOLON id_expression { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	| postfix_expression ARROW id_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| postfix_expression PLUSPLUS { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| postfix_expression MINUSMINUS { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

expression_list:
	assignment_expression
	| expression_list ',' assignment_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

unary_expression:
	postfix_expression
	| PLUSPLUS unary_expression { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| MINUSMINUS unary_expression { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| unary_operator unary_expression { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| SIZEOF unary_expression { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
        | SIZEOF '(' type_id ')' { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
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
	NEW new_placement_opt new_type_id new_initializer_opt { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
        | COLONCOLON NEW new_placement_opt new_type_id new_initializer_opt { $$ = tree_initv(NULL, NULL, 5, $1, $2, $3, $4, $5); }
	;

new_placement:
	'(' expression_list ')' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

new_type_id:
	type_specifier_seq new_declarator_opt { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

new_declarator:
	ptr_operator new_declarator_opt { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| direct_new_declarator
	;

direct_new_declarator:
        '[' expression ']' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| direct_new_declarator '[' constant_expression ']' { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	;

new_initializer:
	'(' expression_list_opt ')' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

delete_expression:
	DELETE unary_expression { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| COLONCOLON DELETE unary_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| DELETE '[' ']' unary_expression { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	| COLONCOLON DELETE '[' ']' unary_expression { $$ = tree_initv(NULL, NULL, 5, $1, $2, $3, $4, $5); }
	;

pm_expression:
	unary_expression
	| pm_expression DOTSTAR unary_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| pm_expression ARROWSTAR unary_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

multiplicative_expression:
	pm_expression
	| multiplicative_expression '*' pm_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| multiplicative_expression '/' pm_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| multiplicative_expression '%' pm_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

additive_expression:
	multiplicative_expression
	| additive_expression '+' multiplicative_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| additive_expression '-' multiplicative_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

shift_expression:
	additive_expression
	| shift_expression SL additive_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| shift_expression SR additive_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

relational_expression:
	shift_expression
	| relational_expression '<' shift_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| relational_expression '>' shift_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| relational_expression LTEQ shift_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| relational_expression GTEQ shift_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

equality_expression:
	relational_expression
	| equality_expression EQ relational_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| equality_expression NOTEQ relational_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

and_expression:
	equality_expression
	| and_expression '&' equality_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

exclusive_or_expression:
	and_expression
	| exclusive_or_expression '^' and_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

inclusive_or_expression:
	exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

logical_and_expression:
	inclusive_or_expression
	| logical_and_expression ANDAND inclusive_or_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

logical_or_expression:
	logical_and_expression
	| logical_or_expression OROR logical_and_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

conditional_expression:
	logical_or_expression
	| logical_or_expression '?' expression ':' assignment_expression { $$ = tree_initv(NULL, NULL, 5, $1, $2, $3, $4, $5); }
	;

assignment_expression:
	conditional_expression
	| logical_or_expression assignment_operator assignment_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
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
	| expression ',' assignment_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
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
        CASE constant_expression ':' statement { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	| DEFAULT ':' statement { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

expression_statement:
	expression_opt ';' { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

compound_statement:
	'{' statement_seq_opt '}' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

statement_seq:
	statement
	| statement_seq statement { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

selection_statement:
        IF '(' condition ')' statement { $$ = tree_initv(NULL, NULL, 5, $1, $2, $3, $4, $5); }
        | IF '(' condition ')' statement ELSE statement { $$ = tree_initv(NULL, NULL, 7, $1, $2, $3, $4, $5, $6, $7); }
	| SWITCH '(' condition ')' statement { $$ = tree_initv(NULL, NULL, 5, $1, $2, $3, $4, $5); }
	;

condition:
	expression
	| type_specifier_seq declarator '=' assignment_expression { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	;

iteration_statement:
	WHILE '(' condition ')' statement { $$ = tree_initv(NULL, NULL, 5, $1, $2, $3, $4, $5); }
	| DO statement WHILE '(' expression ')' ';' { $$ = tree_initv(NULL, NULL, 7, $1, $2, $3, $4, $5, $6, $7); }
	| FOR '(' for_init_statement condition_opt ';' expression_opt ')' statement { $$ = tree_initv(NULL, NULL, 8, $1, $2, $3, $4, $5, $6, $7, $8); }
	;

for_init_statement:
	expression_statement
	| simple_declaration
	;

jump_statement:
	BREAK ';' { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| CONTINUE ';' { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| RETURN expression_opt ';' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

declaration_statement:
	block_declaration
	;

/*----------------------------------------------------------------------
 * Declarations.
 *----------------------------------------------------------------------*/

declaration_seq:
	declaration
	| declaration_seq declaration { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

declaration:
	block_declaration
	| function_definition
	;

block_declaration:
	simple_declaration
	;

simple_declaration:
        decl_specifier_seq init_declarator_list ';' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| decl_specifier_seq ';' { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

decl_specifier:
	type_specifier
	;

decl_specifier_seq:
	decl_specifier
	| decl_specifier_seq decl_specifier { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

type_specifier:
	simple_type_specifier
	| class_specifier
	| elaborated_type_specifier
	;

simple_type_specifier:
	CLASS_NAME
	| nested_name_specifier CLASS_NAME { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
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

elaborated_type_specifier:
        class_key COLONCOLON nested_name_specifier IDENTIFIER { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	| class_key COLONCOLON IDENTIFIER { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

/*----------------------------------------------------------------------
 * Declarators.
 *----------------------------------------------------------------------*/

init_declarator_list:
	init_declarator
	| init_declarator_list ',' init_declarator { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

init_declarator:
	declarator initializer_opt { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

declarator:
	direct_declarator
	| ptr_operator declarator { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

direct_declarator:
	declarator_id
	| direct_declarator '(' parameter_declaration_clause ')' { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	| direct_declarator '[' constant_expression_opt ']' { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	| '(' declarator ')' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

ptr_operator:
	'*'
	| '&'
	| nested_name_specifier '*' { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| COLONCOLON nested_name_specifier '*' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

declarator_id:
	id_expression
	| COLONCOLON id_expression { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| COLONCOLON nested_name_specifier CLASS_NAME { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| COLONCOLON CLASS_NAME { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

type_id:
	type_specifier_seq abstract_declarator_opt { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

type_specifier_seq:
	type_specifier type_specifier_seq_opt { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

abstract_declarator:
	ptr_operator abstract_declarator_opt { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| direct_abstract_declarator
	;

direct_abstract_declarator:
        '(' parameter_declaration_clause ')' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| direct_abstract_declarator '(' parameter_declaration_clause ')' { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	| '[' constant_expression_opt ']' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| direct_abstract_declarator '[' constant_expression_opt ']' { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	| '(' abstract_declarator ')' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

parameter_declaration_clause:
	%empty { $$ = NULL; }
	| parameter_declaration_list
	;

parameter_declaration_list:
	parameter_declaration
	| parameter_declaration_list ',' parameter_declaration { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

parameter_declaration:
	decl_specifier_seq declarator
	| decl_specifier_seq declarator '=' assignment_expression { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	| decl_specifier_seq abstract_declarator_opt { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| decl_specifier_seq abstract_declarator_opt '=' assignment_expression { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	;

function_definition:
	declarator ctor_initializer_opt function_body { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| decl_specifier_seq declarator ctor_initializer_opt function_body { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	;

function_body:
	compound_statement
	;

initializer:
	'=' initializer_clause { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| '(' expression_list ')' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

initializer_clause:
	assignment_expression
	| '{' initializer_list COMMA_opt '}' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| '{' '}' { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

initializer_list:
	initializer_clause
	| initializer_list ',' initializer_clause { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

/*----------------------------------------------------------------------
 * Classes.
 *----------------------------------------------------------------------*/

class_specifier:
        class_head '{' member_specification_opt '}' { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	;

class_head:
	class_key IDENTIFIER { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| class_key nested_name_specifier IDENTIFIER { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

class_key:
	CLASS
	| STRUCT
	;

member_specification:
	member_declaration member_specification_opt { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| access_specifier ':' member_specification_opt { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

member_declaration:
	decl_specifier_seq member_declarator_list ';' { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| decl_specifier_seq ';' { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| member_declarator_list ';' { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| ';'
	| function_definition SEMICOLON_opt { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| qualified_id ';' { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

member_declarator_list:
	member_declarator
	| member_declarator_list ',' member_declarator { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

member_declarator:
	declarator
	| declarator constant_initializer { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| IDENTIFIER ':' constant_expression { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

constant_initializer:
	'=' constant_expression { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
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
	':' mem_initializer_list { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	;

mem_initializer_list:
	mem_initializer
	| mem_initializer ',' mem_initializer_list { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	;

mem_initializer:
	mem_initializer_id '(' expression_list_opt ')' { $$ = tree_initv(NULL, NULL, 4, $1, $2, $3, $4); }
	;

mem_initializer_id:
	COLONCOLON nested_name_specifier CLASS_NAME { $$ = tree_initv(NULL, NULL, 3, $1, $2, $3); }
	| COLONCOLON CLASS_NAME { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| nested_name_specifier CLASS_NAME { $$ = tree_initv(NULL, NULL, 2, $1, $2); }
	| CLASS_NAME
	| IDENTIFIER
	;

/*----------------------------------------------------------------------
 * Epsilon (optional) definitions.
 *----------------------------------------------------------------------*/

declaration_seq_opt:
	%empty { $$ = NULL; }
	| declaration_seq
	;

expression_list_opt:
	%empty { $$ = NULL; }
	| expression_list
	;

new_placement_opt:
	%empty { $$ = NULL; }
	| new_placement
	;

new_initializer_opt:
	%empty { $$ = NULL; }
	| new_initializer
	;

new_declarator_opt:
	%empty { $$ = NULL; }
	| new_declarator
	;

expression_opt:
	%empty { $$ = NULL; }
	| expression
	;

statement_seq_opt:
	%empty { $$ = NULL; }
	| statement_seq
	;

condition_opt:
	%empty { $$ = NULL; }
	| condition
	;

initializer_opt:
	%empty { $$ = NULL; }
	| initializer
	;

constant_expression_opt:
	%empty { $$ = NULL; }
	| constant_expression
	;

abstract_declarator_opt:
	%empty { $$ = NULL; }
	| abstract_declarator
	;

type_specifier_seq_opt:
	%empty { $$ = NULL; }
	| type_specifier_seq
	;

ctor_initializer_opt:
	%empty { $$ = NULL; }
	| ctor_initializer
	;

COMMA_opt:
	%empty { $$ = NULL; }
	| ','
	;

member_specification_opt:
	%empty { $$ = NULL; }
	| member_specification
	;

SEMICOLON_opt:
	%empty { $$ = NULL; }
	| ';'
	;

%%
