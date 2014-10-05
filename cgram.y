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
#include "clex.h"
#include "tree.h"

void yyerror(const char *s);
void typenames_insert_tree(struct tree *t, int category);

extern struct tree *yyprogram;

#define P(name, ...) tree_initv(NULL, #name, __VA_ARGS__)
#define E() NULL

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
	INTEGER	    { $$ = P(literal-integer, 1, $1); }
	| CHARACTER { $$ = P(literal-character, 1, $1); }
	| FLOATING  { $$ = P(literal-floating, 1, $1); }
	| STRING    { $$ = P(literal-string, 1, $1); }
	| boolean   { $$ = P(literal, 1, $1); }
	;

boolean:
	TRUE    { $$ = P(bool1, 1, $1); }
	| FALSE { $$ = P(bool2, 1, $1); }
	;

program:
	declaration_seq_opt { $$ = P(program, 1, $1); yyprogram = $$; }
	;

/*----------------------------------------------------------------------
 * Expressions.
 *----------------------------------------------------------------------*/

primary_expression:
	literal		     { $$ = P(primary-expr1, 1, $1); }
	| '(' expression ')' { $$ = P(primary-expr2, 3, $1, $2, $3); }
	| id_expression	     { $$ = P(primary-expr3, 1, $1); }
	;

id_expression:
	unqualified_id { $$ = P(id-expr1, 1, $1); }
	| qualified_id { $$ = P(id-expr2, 1, $1); }
	;

unqualified_id:
	IDENTIFIER	 { $$ = P(unqualid-1, 1, $1); }
	| '~' CLASS_NAME { $$ = P(unqualid-2, 2, $1, $2); }
	;

qualified_id:
	nested_name_specifier unqualified_id { $$ = P(qual-id, 2, $1, $2); }
	;

nested_name_specifier:
	CLASS_NAME COLONCOLON nested_name_specifier { $$ = P(nested-name1, 3, $1, $2, $3); }
	| CLASS_NAME COLONCOLON			    { $$ = P(nested-name2, 2, $1, $2); }
	;

postfix_expression:
	primary_expression				    { $$ = P(postfix-expr1, 1, $1); }
	| postfix_expression '[' expression ']'		    { $$ = P(postfix-expr2, 4, $1, $2, $3, $4); }
	| postfix_expression '(' expression_list_opt ')'    { $$ = P(postfix-expr3, 4, $1, $2, $3, $4); }
	| simple_type_specifier '(' expression_list_opt ')' { $$ = P(postfix-expr4, 4, $1, $2, $3, $4); }
	| postfix_expression '.' COLONCOLON id_expression   { $$ = P(postfix-expr5, 3, $1, $2, $3); }
	| postfix_expression '.' id_expression		    { $$ = P(postfix-expr6, 3, $1, $2, $3); }
	| postfix_expression ARROW COLONCOLON id_expression { $$ = P(postfix-expr7, 4, $1, $2, $3, $4); }
	| postfix_expression ARROW id_expression	    { $$ = P(postfix-expr8, 3, $1, $2, $3); }
	| postfix_expression PLUSPLUS			    { $$ = P(postfix-expr9, 2, $1, $2); }
	| postfix_expression MINUSMINUS			    { $$ = P(postfix-expr10, 2, $1, $2); }
	;

expression_list:
	assignment_expression			    { $$ = P(expr-list1, 1, $1); }
	| expression_list ',' assignment_expression { $$ = P(expr-list2, 3, $1, $2, $3); }
	;

unary_expression:
	postfix_expression		  { $$ = P(unary-expr1, 1, $1); }
	| PLUSPLUS unary_expression	  { $$ = P(unary-expr2, 2, $1, $2); }
	| MINUSMINUS unary_expression	  { $$ = P(unary-expr3, 2, $1, $2); }
	| '*' unary_expression		  { $$ = P(unary-expr4, 2, $1, $2); }
	| '&' unary_expression		  { $$ = P(unary-expr5, 2, $1, $2); }
	| unary_operator unary_expression { $$ = P(unary-expr6, 2, $1, $2); }
	| SIZEOF unary_expression	  { $$ = P(unary-expr7, 2, $1, $2); }
	| SIZEOF '(' type_id ')'	  { $$ = P(unary-expr8, 4, $1, $2, $3, $4); }
	| new_expression		  { $$ = P(unary-expr9, 1, $1); }
	| delete_expression		  { $$ = P(unary-expr10, 1, $1); }
	;

unary_operator:
	'+'   { $$ = P(unary-op, 1, $1); }
	| '-' { $$ = P(unary-op, 1, $1); }
	| '!' { $$ = P(unary-op, 1, $1); }
	| '~' { $$ = P(unary-op, 1, $1); }
	;

new_expression:
	NEW new_placement_opt new_type_id new_initializer_opt		   { $$ = P(new-expr1, 4, $1, $2, $3, $4); }
	| COLONCOLON NEW new_placement_opt new_type_id new_initializer_opt { $$ = P(new-expr2, 5, $1, $2, $3, $4, $5); }
	;

new_placement:
	'(' expression_list ')' { $$ = P(new-placement, 3, $1, $2, $3); }
	;

new_type_id:
	type_specifier_seq new_declarator_opt { $$ = P(new-type-id, 2, $1, $2); }
	;

new_declarator:
	ptr_operator new_declarator_opt { $$ = P(new-decl1, 2, $1, $2); }
	| direct_new_declarator	        { $$ = P(new-decl2, 1, $1); }
	;

direct_new_declarator:
	'[' expression ']'				    { $$ = P(direct-new-decl1, 3, $1, $2, $3); }
	| direct_new_declarator '[' constant_expression ']' { $$ = P(direct-new-decl1, 4, $1, $2, $3, $4); }
	;

new_initializer:
	'(' expression_list_opt ')' { $$ = P(new-init, 3, $1, $2, $3); }
	;

delete_expression:
	DELETE unary_expression			     { $$ = P(delete-expr1, 2, $1, $2); }
	| COLONCOLON DELETE unary_expression	     { $$ = P(delete-expr2, 3, $1, $2, $3); }
	| DELETE '[' ']' unary_expression	     { $$ = P(delete-expr3, 4, $1, $2, $3, $4); }
	| COLONCOLON DELETE '[' ']' unary_expression { $$ = P(delete-expr4, 5, $1, $2, $3, $4, $5); }
	;

pm_expression:
	unary_expression			   { $$ = P(pm-expr1, 1, $1); }
	| pm_expression DOTSTAR unary_expression   { $$ = P(pm-expr2, 3, $1, $2, $3); }
	| pm_expression ARROWSTAR unary_expression { $$ = P(pm-expr3, 3, $1, $2, $3); }
	;

multiplicative_expression:
	pm_expression				      { $$ = P(mult-expr1, 1, $1); }
	| multiplicative_expression '*' pm_expression { $$ = P(mult-expr2, 3, $1, $2, $3); }
	| multiplicative_expression '/' pm_expression { $$ = P(mult-expr3, 3, $1, $2, $3); }
	| multiplicative_expression '%' pm_expression { $$ = P(mult-expr4, 3, $1, $2, $3); }
	;

additive_expression:
	multiplicative_expression			    { $$ = P(add-expr1, 1, $1); }
	| additive_expression '+' multiplicative_expression { $$ = P(add-expr2, 3, $1, $2, $3); }
	| additive_expression '-' multiplicative_expression { $$ = P(add-expr3, 3, $1, $2, $3); }
	;

shift_expression:
	additive_expression			  { $$ = P(shift-expr1, 1, $1); }
	| shift_expression SL additive_expression { $$ = P(shift-expr2, 3, $1, $2, $3); }
	| shift_expression SR additive_expression { $$ = P(shift-expr3, 3, $1, $2, $3); }
	;

relational_expression:
	shift_expression			      { $$ = P(rel-expr1, 1, $1); }
	| relational_expression '<' shift_expression  { $$ = P(rel-expr2, 3, $1, $2, $3); }
	| relational_expression '>' shift_expression  { $$ = P(rel-expr3, 3, $1, $2, $3); }
	| relational_expression LTEQ shift_expression { $$ = P(rel-expr4, 3, $1, $2, $3); }
	| relational_expression GTEQ shift_expression { $$ = P(rel-expr5, 3, $1, $2, $3); }
	;

equality_expression:
	relational_expression				  { $$ = P(equal-expr1, 1, $1); }
	| equality_expression EQ relational_expression	  { $$ = P(equal-expr2, 3, $1, $2, $3); }
	| equality_expression NOTEQ relational_expression { $$ = P(equal-expr3, 3, $1, $2, $3); }
	;

and_expression:
	equality_expression			 { $$ = P(and-expr1, 1, $1); }
	| and_expression '&' equality_expression { $$ = P(and-expr2, 3, $1, $2, $3); }
	;

exclusive_or_expression:
	and_expression				     { $$ = P(xor-expr1, 1, $1); }
	| exclusive_or_expression '^' and_expression { $$ = P(xor-expr2, 3, $1, $2, $3); }
	;

inclusive_or_expression:
	exclusive_or_expression				      { $$ = P(or-expr1, 1, $1); }
	| inclusive_or_expression '|' exclusive_or_expression { $$ = P(or-expr2, 3, $1, $2, $3); }
	;

logical_and_expression:
	inclusive_or_expression				        { $$ = P(logical-and-expr1, 1, $1); }
	| logical_and_expression ANDAND inclusive_or_expression { $$ = P(logical-and-expr2, 3, $1, $2, $3); }
	;

logical_or_expression:
	logical_and_expression				    { $$ = P(logical-or-expr1, 1, $1); }
	| logical_or_expression OROR logical_and_expression { $$ = P(logical-or-expr2, 3, $1, $2, $3); }
	;

conditional_expression:
	logical_or_expression						 { $$ = P(cond-expr1, 1, $1); }
	| logical_or_expression '?' expression ':' assignment_expression { $$ = P(cond-expr2, 5, $1, $2, $3, $4, $5); }
	;

assignment_expression:
	conditional_expression						  { $$ = P(assign-expr1, 1, $1); }
	| logical_or_expression assignment_operator assignment_expression { $$ = P(assign-expr2, 3, $1, $2, $3); }
	;

assignment_operator:
	'='     { $$ = P(assign-op1, 1, $1); }
	| MULEQ { $$ = P(assign-op2, 1, $1); }
	| DIVEQ { $$ = P(assign-op3, 1, $1); }
	| MODEQ { $$ = P(assign-op4, 1, $1); }
	| ADDEQ { $$ = P(assign-op5, 1, $1); }
	| SUBEQ { $$ = P(assign-op6, 1, $1); }
	| SREQ  { $$ = P(assign-op7, 1, $1); }
	| SLEQ  { $$ = P(assign-op8, 1, $1); }
	| ANDEQ { $$ = P(assign-op9, 1, $1); }
	| XOREQ { $$ = P(assign-op10, 1, $1); }
	| OREQ  { $$ = P(assign-op11, 1, $1); }
	;

expression:
	assignment_expression		       { $$ = P(expr1, 1, $1); }
	| expression ',' assignment_expression { $$ = P(expr2, 3, $1, $2, $3); }
	;

constant_expression:
	conditional_expression { $$ = P(constant-expr, 1, $1); }
	;

/*----------------------------------------------------------------------
 * Statements.
 *----------------------------------------------------------------------*/

statement:
	labeled_statement       { $$ = P(statement1, 1, $1); }
	| expression_statement  { $$ = P(statement2, 1, $1); }
	| compound_statement    { $$ = P(statement3, 1, $1); }
	| selection_statement   { $$ = P(statement4, 1, $1); }
	| iteration_statement   { $$ = P(statement5, 1, $1); }
	| jump_statement        { $$ = P(statement6, 1, $1); }
	| declaration_statement { $$ = P(statement7, 1, $1); }
	;

labeled_statement:
	CASE constant_expression ':' statement { $$ = P(labeled-statement1, 4, $1, $2, $3, $4); }
	| DEFAULT ':' statement		       { $$ = P(labeled-statement2, 3, $1, $2, $3); }
	;

expression_statement:
	expression_opt ';' { $$ = P(expr-statement, 2, $1, $2); }
	;

compound_statement:
	'{' statement_seq_opt '}' { $$ = P(compound-statement, 3, $1, $2, $3); }
	;

statement_seq:
	statement		  { $$ = P(statement-seq1, 1, $1); }
	| statement_seq statement { $$ = P(statement-seq2, 2, $1, $2); }
	;

selection_statement:
	IF '(' condition ')' statement		        { $$ = P(select1, 5, $1, $2, $3, $4, $5); }
	| IF '(' condition ')' statement ELSE statement { $$ = P(select2, 7, $1, $2, $3, $4, $5, $6, $7); }
	| SWITCH '(' condition ')' statement	        { $$ = P(select3, 5, $1, $2, $3, $4, $5); }
	;

condition:
	expression						  { $$ = P(condition1, 1, $1); }
	| type_specifier_seq declarator '=' assignment_expression { $$ = P(condition2, 4, $1, $2, $3, $4); }
	;

iteration_statement:
	WHILE '(' condition ')' statement					    { $$ = P(iter1, 5, $1, $2, $3, $4, $5); }
	| DO statement WHILE '(' expression ')' ';'				    { $$ = P(iter2, 7, $1, $2, $3, $4, $5, $6, $7); }
	| FOR '(' for_init_statement condition_opt ';' expression_opt ')' statement { $$ = P(iter3, 8, $1, $2, $3, $4, $5, $6, $7, $8); }
	;

for_init_statement:
	expression_statement { $$ = P(forinit1, 1, $1); }
	| simple_declaration { $$ = P(forinit2, 1, $1); }
	;

jump_statement:
	BREAK ';'		    { $$ = P(jump1, 2, $1, $2); }
	| CONTINUE ';'		    { $$ = P(jump2, 2, $1, $2); }
	| RETURN expression_opt ';' { $$ = P(jump3, 3, $1, $2, $3); }
	;

declaration_statement:
	block_declaration { $$ = P(decl-statement, 1, $1); }
	;

/*----------------------------------------------------------------------
 * Declarations.
 *----------------------------------------------------------------------*/

declaration_seq:
	declaration		      { $$ = P(decl-seq1, 1, $1); }
	| declaration_seq declaration { $$ = P(decl-seq2, 2, $1, $2); }
	;

declaration:
	block_declaration     { $$ = P(decl1, 1, $1); }
	| function_definition { $$ = P(decl2, 1, $1); }
	;

block_declaration:
	simple_declaration { $$ = P(block-decl, 1, $1); }
	;

simple_declaration:
	decl_specifier_seq init_declarator_list ';' { $$ = P(simple-decl1, 3, $1, $2, $3); }
	| decl_specifier_seq ';'		    { $$ = P(simple-decl2, 2, $1, $2); }
	;

decl_specifier:
	type_specifier { $$ = P(decl-spec, 1, $1); }
	;

decl_specifier_seq:
	decl_specifier			    { $$ = P(decl-spec-seq1, 1, $1); }
	| decl_specifier_seq decl_specifier { $$ = P(decl-spec-seq2, 2, $1, $2); }
	;

type_specifier:
	simple_type_specifier	    { $$ = P(type-spec1, 1, $1); }
	| class_specifier	    { $$ = P(type-spec2, 1, $1); }
	| elaborated_type_specifier { $$ = P(type-spec3, 1, $1); }
	;

simple_type_specifier:
	CLASS_NAME				      { $$ = P(simple-type-spec1, 1, $1); }
	| nested_name_specifier CLASS_NAME	      { $$ = P(simple-type-spec2, 2, $1, $2); }
	| COLONCOLON nested_name_specifier CLASS_NAME { $$ = P(simple-type-spec3, 3, $1, $2, $3); }
	| COLONCOLON CLASS_NAME			      { $$ = P(simple-type-spec4, 2, $1, $2); }
	| CHAR					      { $$ = P(simple-type-spec5, 1, $1); }
	| BOOL					      { $$ = P(simple-type-spec6, 1, $1); }
	| SHORT					      { $$ = P(simple-type-spec7, 1, $1); }
	| INT					      { $$ = P(simple-type-spec8, 1, $1); }
	| LONG					      { $$ = P(simple-type-spec9, 1, $1); }
	| SIGNED				      { $$ = P(simple-type-spec1, 1, $1); }
	| UNSIGNED				      { $$ = P(simple-type-spec1, 1, $1); }
	| FLOAT					      { $$ = P(simple-type-spec1, 1, $1); }
	| DOUBLE				      { $$ = P(simple-type-spec1, 1, $1); }
	| VOID					      { $$ = P(simple-type-spec1, 1, $1); }
	;

elaborated_type_specifier:
	class_key COLONCOLON nested_name_specifier IDENTIFIER { $$ = P(elab-type-spec1, 4, $1, $2, $3, $4); }
	| class_key COLONCOLON IDENTIFIER		      { $$ = P(elab-type-spec2, 3, $1, $2, $3); }
	;

/*----------------------------------------------------------------------
 * Declarators.
 *----------------------------------------------------------------------*/

init_declarator_list:
	init_declarator				   { $$ = P(init-decl-list1, 1, $1); }
	| init_declarator_list ',' init_declarator { $$ = P(init-decl-list2, 3, $1, $2, $3); }
	;

init_declarator:
	declarator initializer_opt { $$ = P(init-decl, 2, $1, $2); }
	;

declarator:
	direct_declarator	  { $$ = P(decl1, 1, $1); }
	| ptr_operator declarator { $$ = P(decl2, 2, $1, $2); }
	;

direct_declarator:
	declarator_id								   { $$ = P(direct-decl1, 1, $1); }
	| direct_declarator '(' parameter_declaration_clause ')'		   { $$ = P(direct-decl2, 4, $1, $2, $3, $4); }
	| CLASS_NAME '(' parameter_declaration_clause ')'			   { $$ = P(direct-decl3, 4, $1, $2, $3, $4); }
	| CLASS_NAME COLONCOLON declarator_id '(' parameter_declaration_clause ')' { $$ = P(direct-decl4, 6, $1, $2, $3, $4, $5, $6); }
	| CLASS_NAME COLONCOLON CLASS_NAME '(' parameter_declaration_clause ')'	   { $$ = P(direct-decl5, 6, $1, $2, $3, $4, $5, $6); }
	| direct_declarator '[' constant_expression_opt ']'			   { $$ = P(direct-decl6, 4, $1, $2, $3, $4); }
	| '(' declarator ')'							   { $$ = P(direct-decl7, 3, $1, $2, $3); }
	;

ptr_operator:
	'*'				       { $$ = P(ptr1, 1, $1); }
	| '&'				       { $$ = P(ptr2, 1, $1); }
	| nested_name_specifier '*'	       { $$ = P(ptr3, 2, $1, $2); }
	| COLONCOLON nested_name_specifier '*' { $$ = P(ptr4, 3, $1, $2, $3); }
	;

declarator_id:
	id_expression				      { $$ = P(decl-d1, 1, $1); }
	| COLONCOLON id_expression		      { $$ = P(decl-d2, 2, $1, $2); }
	| COLONCOLON nested_name_specifier CLASS_NAME { $$ = P(decl-d3, 3, $1, $2, $3); }
	| COLONCOLON CLASS_NAME			      { $$ = P(decl-d4, 2, $1, $2); }
	;

type_id:
	type_specifier_seq abstract_declarator_opt { $$ = P(type-id, 2, $1, $2); }
	;

type_specifier_seq:
	type_specifier type_specifier_seq_opt { $$ = P(type-spec-seq, 2, $1, $2); }
	;

abstract_declarator:
	ptr_operator abstract_declarator_opt { $$ = P(abstract-decl1, 2, $1, $2); }
	| direct_abstract_declarator	     { $$ = P(abstract-decl2, 1, $1); }
	;

direct_abstract_declarator:
	direct_abstract_declarator '(' parameter_declaration_clause ')' { $$ = P(direct-abstract-decl1, 4, $1, $2, $3, $4); }
	| '(' parameter_declaration_clause ')'			        { $$ = P(direct-abstract-decl2, 3, $1, $2, $3); }
	| direct_abstract_declarator '[' constant_expression_opt ']'    { $$ = P(direct-abstract-decl3, 4, $1, $2, $3, $4); }
	| '[' constant_expression_opt ']'			        { $$ = P(direct-abstract-decl4, 3, $1, $2, $3); }
	| '(' abstract_declarator ')'				        { $$ = P(direct-abstract-decl5, 3, $1, $2, $3); }
	;

parameter_declaration_clause:
	parameter_declaration_list { $$ = P(param-decl, 1, $1); }
	| %empty		   { $$ = E(); }
	;

parameter_declaration_list:
	parameter_declaration				       { $$ = P(param-decl-list1, 1, $1); }
	| parameter_declaration_list ',' parameter_declaration { $$ = P(param-decl-list1, 3, $1, $2, $3); }
	;

parameter_declaration:
	decl_specifier_seq declarator					       { $$ = P(param-decl1, 1, $1); }
	| decl_specifier_seq declarator '=' assignment_expression	       { $$ = P(param-decl2, 4, $1, $2, $3, $4); }
	| decl_specifier_seq abstract_declarator_opt			       { $$ = P(param-decl3, 2, $1, $2); }
	| decl_specifier_seq abstract_declarator_opt '=' assignment_expression { $$ = P(param-decl4, 4, $1, $2, $3, $4); }
	;

function_definition:
	declarator ctor_initializer_opt function_body			   { $$ = P(function-def1, 3, $1, $2, $3); }
	| decl_specifier_seq declarator ctor_initializer_opt function_body { $$ = P(function-def2, 4, $1, $2, $3, $4); }
	;

function_body:
	compound_statement { $$ = P(function-body, 1, $1); }
	;

initializer:
	'=' initializer_clause	  { $$ = P(init1, 2, $1, $2); }
	| '(' expression_list ')' { $$ = P(init2, 3, $1, $2, $3); }
	;

initializer_clause:
	assignment_expression		     { $$ = P(init-clause1, 1, $1); }
	| '{' initializer_list COMMA_opt '}' { $$ = P(init-clause2, 3, $1, $2, $3); }
	| '{' '}'			     { $$ = P(init-clause3, 2, $1, $2); }
	;

initializer_list:
	initializer_clause			  { $$ = P(init-list1, 1, $1); }
	| initializer_list ',' initializer_clause { $$ = P(init-list2, 3, $1, $2, $3); }
	;

/*----------------------------------------------------------------------
 * Classes.
 *----------------------------------------------------------------------*/

class_specifier:
	class_head '{' member_specification_opt '}' { $$ = P(class-spec, 4, $1, $2, $3, $4); }
	;

class_head:
	class_key IDENTIFIER			     { $$ = P(class-head1, 2, $1, $2); typenames_insert_tree($2, CLASS_NAME); }
	| class_key nested_name_specifier IDENTIFIER { $$ = P(class-head2, 3, $1, $2, $3); typenames_insert_tree($3, CLASS_NAME); }
	;

class_key:
	CLASS	 { $$ = P(class-key1, 1, $1); }
	| STRUCT { $$ = P(class-key2, 1, $1); }
	;

member_specification:
	member_declaration member_specification_opt     { $$ = P(member-spec1, 2, $1, $2); }
	| access_specifier ':' member_specification_opt { $$ = P(member-spec2, 3, $1, $2, $3); }
	;

member_declaration:
	decl_specifier_seq member_declarator_list ';' { $$ = P(member-decl1, 3, $1, $2, $3); }
	| decl_specifier_seq ';'		      { $$ = P(member-decl2, 2, $1, $2); }
	| member_declarator_list ';'		      { $$ = P(member-decl3, 2, $1, $2); }
	| ';'					      { $$ = P(member-decl4, 1, $1); }
	| function_definition SEMICOLON_opt	      { $$ = P(member-decl5, 2, $1, $2); }
	| qualified_id ';'			      { $$ = P(member-decl6, 2, $1, $2); }
	;

member_declarator_list:
	member_declarator			       { $$ = P(member-decl-list1, 1, $1); }
	| member_declarator_list ',' member_declarator { $$ = P(member-decl-list2, 3, $1, $2, $3); }
	;

member_declarator:
	declarator			     { $$ = P(member-declarator1, 1, $1); }
	| declarator constant_initializer    { $$ = P(member-declarator2, 2, $1, $2); }
	| IDENTIFIER ':' constant_expression { $$ = P(member-declarator3, 3, $1, $2, $3); }
	;

constant_initializer:
	'=' constant_expression { $$ = P(constant-init, 2, $1, $2); }
	;

access_specifier:
	PRIVATE	    { $$ = P(access-spec1, 1, $1); }
	| PROTECTED { $$ = P(access-spec2, 1, $1); }
	| PUBLIC    { $$ = P(access-spec3, 1, $1); }
	;

/*----------------------------------------------------------------------
 * Special member functions.
 *----------------------------------------------------------------------*/

ctor_initializer:
	':' mem_initializer_list { $$ = P(ctor-init, 2, $1, $2); }
	;

mem_initializer_list:
	mem_initializer				   { $$ = P(mem-init-list1, 1, $1); }
	| mem_initializer ',' mem_initializer_list { $$ = P(mem-init-list2, 3, $1, $2, $3); }
	;

mem_initializer:
	mem_initializer_id '(' expression_list_opt ')' { $$ = P(mem-init, 4, $1, $2, $3, $4); }
	;

mem_initializer_id:
	COLONCOLON nested_name_specifier CLASS_NAME { $$ = P(mem-init-id1, 3, $1, $2, $3); }
	| COLONCOLON CLASS_NAME			    { $$ = P(mem-init-id2, 2, $1, $2); }
	| nested_name_specifier CLASS_NAME	    { $$ = P(mem-init-id3, 2, $1, $2); }
	| CLASS_NAME				    { $$ = P(mem-init-id4, 1, $1); }
	| IDENTIFIER				    { $$ = P(mem-init-id5, 1, $1); }
	;

/*----------------------------------------------------------------------
 * Epsilon (optional) definitions.
 *----------------------------------------------------------------------*/

declaration_seq_opt:
	%empty		  { $$ = E(); }
	| declaration_seq { $$ = P(decl-seq-opt, 1, $1); }
	;

expression_list_opt:
	%empty		  { $$ = E(); }
	| expression_list { $$ = P(expr-list-opt, 1, $1); }
	;

new_placement_opt:
	%empty	        { $$ = E(); }
	| new_placement { $$ = P(new-placement-opt, 1, $1); }
	;

new_initializer_opt:
	%empty		  { $$ = E(); }
	| new_initializer { $$ = P(new-init-opt, 1, $1); }
	;

new_declarator_opt:
	%empty		 { $$ = E(); }
	| new_declarator { $$ = P(new-decl-opt, 1, $1); }
	;

expression_opt:
	%empty	     { $$ = E(); }
	| expression { $$ = P(expr-opt, 1, $1); }
	;

statement_seq_opt:
	%empty	        { $$ = E(); }
	| statement_seq { $$ = P(statement-opt, 1, $1); }
	;

condition_opt:
	%empty	    { $$ = E(); }
	| condition { $$ = P(cond-opt, 1, $1); }
	;

initializer_opt:
	%empty	      { $$ = E(); }
	| initializer { $$ = P(init-opt, 1, $1); }
	;

constant_expression_opt:
	%empty		      { $$ = E(); }
	| constant_expression { $$ = P(constant-expr-opt, 1, $1); }
	;

abstract_declarator_opt:
	%empty		      { $$ = E(); }
	| abstract_declarator { $$ = P(abstract-decl-opt, 1, $1); }
	;

type_specifier_seq_opt:
	%empty		     { $$ = E(); }
	| type_specifier_seq { $$ = P(type-spec-seq-opt, 1, $1); }
	;

ctor_initializer_opt:
	%empty		   { $$ = E(); }
	| ctor_initializer { $$ = P(ctor-init-opt, 1, $1); }
	;

COMMA_opt:
	%empty { $$ = E(); }
	| ','  { $$ = P(comma-opt, 1, $1); }
	;

member_specification_opt:
	%empty		       { $$ = E(); }
	| member_specification { $$ = P(member-spec-opt, 1, $1); }
	;

SEMICOLON_opt:
	%empty { $$ = E(); }
	| ';'  { $$ = P(semicol-opt, 1, $1); }
	;

%%
