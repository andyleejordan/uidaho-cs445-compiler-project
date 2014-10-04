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

#define p(name, ...) tree_initv(NULL, #name, __VA_ARGS__)

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
	INTEGER	    { $$ = p(literal-integer, 1, $1); }
	| CHARACTER { $$ = p(literal-character, 1, $1); }
	| FLOATING  { $$ = p(literal-floating, 1, $1); }
	| STRING    { $$ = p(literal-string, 1, $1); }
	| boolean   { $$ = p(literal, 1, $1); }
	;

boolean:
	TRUE    { $$ = p(bool-true, 1, $1); }
	| FALSE { $$ = p(bool-false, 1, $1); }
	;

program:
	declaration_seq_opt { $$ = p(program, 1, $1); yyprogram = $$; }
	;

/*----------------------------------------------------------------------
 * Expressions.
 *----------------------------------------------------------------------*/

primary_expression:
	literal		     { $$ = p(primary-expr1, 1, $1); }
	| '(' expression ')' { $$ = p(primary-expr2, 3, $1, $2, $3); }
	| id_expression	     { $$ = p(primary-expr3, 1, $1); }
	;

id_expression:
	unqualified_id { $$ = p(id-expr1, 1, $1); }
	| qualified_id { $$ = p(id-expr2, 1, $1); }
	;

unqualified_id:
	IDENTIFIER	 { $$ = p(unqualid-1, 1, $1); }
	| '~' CLASS_NAME { $$ = p(unqualid-2, 2, $1, $2); }
	;

qualified_id:
	nested_name_specifier unqualified_id { $$ = p(qual-id, 2, $1, $2); }
	;

nested_name_specifier:
	CLASS_NAME COLONCOLON nested_name_specifier { $$ = p(nested-name1, 3, $1, $2, $3); }
	| CLASS_NAME COLONCOLON			    { $$ = p(nested-name2, 2, $1, $2); }
	;

postfix_expression:
	primary_expression				    { $$ = p(postfix-expr1, 1, $1); }
	| postfix_expression '[' expression ']'		    { $$ = p(postfix-expr2, 4, $1, $2, $3, $4); }
	| postfix_expression '(' expression_list_opt ')'    { $$ = p(postfix-expr3, 4, $1, $2, $3, $4); }
	| simple_type_specifier '(' expression_list_opt ')' { $$ = p(postfix-expr4, 4, $1, $2, $3, $4); }
	| postfix_expression '.' COLONCOLON id_expression   { $$ = p(postfix-expr5, 3, $1, $2, $3); }
	| postfix_expression '.' id_expression		    { $$ = p(postfix-expr6, 3, $1, $2, $3); }
	| postfix_expression ARROW COLONCOLON id_expression { $$ = p(postfix-expr7, 4, $1, $2, $3, $4); }
	| postfix_expression ARROW id_expression	    { $$ = p(postfix-expr8, 3, $1, $2, $3); }
	| postfix_expression PLUSPLUS			    { $$ = p(postfix-expr9, 2, $1, $2); }
	| postfix_expression MINUSMINUS			    { $$ = p(postfix-expr10, 2, $1, $2); }
	;

expression_list:
	assignment_expression			    { $$ = p(expr-list1, 1, $1); }
	| expression_list ',' assignment_expression { $$ = p(expr-list2, 3, $1, $2, $3); }
	;

unary_expression:
	postfix_expression		  { $$ = p(unary-expr1, 1, $1); }
	| PLUSPLUS unary_expression	  { $$ = p(unary-expr2, 2, $1, $2); }
	| MINUSMINUS unary_expression	  { $$ = p(unary-expr3, 2, $1, $2); }
	| '*' unary_expression		  { $$ = p(unary-expr4, 2, $1, $2); }
	| '&' unary_expression		  { $$ = p(unary-expr5, 2, $1, $2); }
	| unary_operator unary_expression { $$ = p(unary-expr6, 2, $1, $2); }
	| SIZEOF unary_expression	  { $$ = p(unary-expr7, 2, $1, $2); }
	| SIZEOF '(' type_id ')'	  { $$ = p(unary-expr8, 4, $1, $2, $3, $4); }
	| new_expression		  { $$ = p(unary-expr9, 1, $1); }
	| delete_expression		  { $$ = p(unary-expr10, 1, $1); }
	;

unary_operator:
	'+'   { $$ = p(unary-op, 1, $1); }
	| '-' { $$ = p(unary-op, 1, $1); }
	| '!' { $$ = p(unary-op, 1, $1); }
	| '~' { $$ = p(unary-op, 1, $1); }
	;

new_expression:
	NEW new_placement_opt new_type_id new_initializer_opt		   { $$ = p(new-expr1, 4, $1, $2, $3, $4); }
	| COLONCOLON NEW new_placement_opt new_type_id new_initializer_opt { $$ = p(new-expr2, 5, $1, $2, $3, $4, $5); }
	;

new_placement:
	'(' expression_list ')' { $$ = p(new-placement, 3, $1, $2, $3); }
	;

new_type_id:
	type_specifier_seq new_declarator_opt { $$ = p(new-type-id, 2, $1, $2); }
	;

new_declarator:
	ptr_operator new_declarator_opt { $$ = p(new-decl1, 2, $1, $2); }
	| direct_new_declarator	        { $$ = p(new-decl2, 1, $1); }
	;

direct_new_declarator:
	'[' expression ']'				    { $$ = p(direct-new-decl1, 3, $1, $2, $3); }
	| direct_new_declarator '[' constant_expression ']' { $$ = p(direct-new-decl1, 4, $1, $2, $3, $4); }
	;

new_initializer:
	'(' expression_list_opt ')' { $$ = p(new-init, 3, $1, $2, $3); }
	;

delete_expression:
	DELETE unary_expression			     { $$ = p(delete-expr1, 2, $1, $2); }
	| COLONCOLON DELETE unary_expression	     { $$ = p(delete-expr2, 3, $1, $2, $3); }
	| DELETE '[' ']' unary_expression	     { $$ = p(delete-expr3, 4, $1, $2, $3, $4); }
	| COLONCOLON DELETE '[' ']' unary_expression { $$ = p(delete-expr4, 5, $1, $2, $3, $4, $5); }
	;

pm_expression:
	unary_expression			   { $$ = p(pm-expr1, 1, $1); }
	| pm_expression DOTSTAR unary_expression   { $$ = p(pm-expr2, 3, $1, $2, $3); }
	| pm_expression ARROWSTAR unary_expression { $$ = p(pm-expr3, 3, $1, $2, $3); }
	;

multiplicative_expression:
	pm_expression				      { $$ = p(mult-expr1, 1, $1); }
	| multiplicative_expression '*' pm_expression { $$ = p(mult-expr2, 3, $1, $2, $3); }
	| multiplicative_expression '/' pm_expression { $$ = p(mult-expr3, 3, $1, $2, $3); }
	| multiplicative_expression '%' pm_expression { $$ = p(mult-expr4, 3, $1, $2, $3); }
	;

additive_expression:
	multiplicative_expression			    { $$ = p(add-expr1, 1, $1); }
	| additive_expression '+' multiplicative_expression { $$ = p(add-expr2, 3, $1, $2, $3); }
	| additive_expression '-' multiplicative_expression { $$ = p(add-expr3, 3, $1, $2, $3); }
	;

shift_expression:
	additive_expression			  { $$ = p(shift-expr1, 1, $1); }
	| shift_expression SL additive_expression { $$ = p(shift-expr2, 3, $1, $2, $3); }
	| shift_expression SR additive_expression { $$ = p(shift-expr3, 3, $1, $2, $3); }
	;

relational_expression:
	shift_expression			      { $$ = p(rel-expr1, 1, $1); }
	| relational_expression '<' shift_expression  { $$ = p(rel-expr2, 3, $1, $2, $3); }
	| relational_expression '>' shift_expression  { $$ = p(rel-expr3, 3, $1, $2, $3); }
	| relational_expression LTEQ shift_expression { $$ = p(rel-expr4, 3, $1, $2, $3); }
	| relational_expression GTEQ shift_expression { $$ = p(rel-expr5, 3, $1, $2, $3); }
	;

equality_expression:
	relational_expression				  { $$ = p(equal-expr1, 1, $1); }
	| equality_expression EQ relational_expression	  { $$ = p(equal-expr2, 3, $1, $2, $3); }
	| equality_expression NOTEQ relational_expression { $$ = p(equal-expr3, 3, $1, $2, $3); }
	;

and_expression:
	equality_expression			 { $$ = p(and-expr1, 1, $1); }
	| and_expression '&' equality_expression { $$ = p(and-expr2, 3, $1, $2, $3); }
	;

exclusive_or_expression:
	and_expression				     { $$ = p(xor-expr1, 1, $1); }
	| exclusive_or_expression '^' and_expression { $$ = p(xor-expr2, 3, $1, $2, $3); }
	;

inclusive_or_expression:
	exclusive_or_expression				      { $$ = p(or-expr1, 1, $1); }
	| inclusive_or_expression '|' exclusive_or_expression { $$ = p(or-expr2, 3, $1, $2, $3); }
	;

logical_and_expression:
	inclusive_or_expression				        { $$ = p(logical-and-expr1, 1, $1); }
	| logical_and_expression ANDAND inclusive_or_expression { $$ = p(logical-and-expr2, 3, $1, $2, $3); }
	;

logical_or_expression:
	logical_and_expression				    { $$ = p(logical-or-expr1, 1, $1); }
	| logical_or_expression OROR logical_and_expression { $$ = p(logical-or-expr2, 3, $1, $2, $3); }
	;

conditional_expression:
	logical_or_expression						 { $$ = p(cond-expr1, 1, $1); }
	| logical_or_expression '?' expression ':' assignment_expression { $$ = p(cond-expr2, 5, $1, $2, $3, $4, $5); }
	;

assignment_expression:
	conditional_expression						  { $$ = p(assign-expr1, 1, $1); }
	| logical_or_expression assignment_operator assignment_expression { $$ = p(assign-expr2, 3, $1, $2, $3); }
	;

assignment_operator:
	'='     { $$ = p(assign-op1, 1, $1); }
	| MULEQ { $$ = p(assign-op2, 1, $1); }
	| DIVEQ { $$ = p(assign-op3, 1, $1); }
	| MODEQ { $$ = p(assign-op4, 1, $1); }
	| ADDEQ { $$ = p(assign-op5, 1, $1); }
	| SUBEQ { $$ = p(assign-op6, 1, $1); }
	| SREQ  { $$ = p(assign-op7, 1, $1); }
	| SLEQ  { $$ = p(assign-op8, 1, $1); }
	| ANDEQ { $$ = p(assign-op9, 1, $1); }
	| XOREQ { $$ = p(assign-op10, 1, $1); }
	| OREQ  { $$ = p(assign-op11, 1, $1); }
	;

expression:
	assignment_expression		       { $$ = p(expr1, 1, $1); }
	| expression ',' assignment_expression { $$ = p(expr2, 3, $1, $2, $3); }
	;

constant_expression:
	conditional_expression { $$ = p(constant-expr, 1, $1); }
	;

/*----------------------------------------------------------------------
 * Statements.
 *----------------------------------------------------------------------*/

statement:
	labeled_statement       { $$ = p(statement1, 1, $1); }
	| expression_statement  { $$ = p(statement2, 1, $1); }
	| compound_statement    { $$ = p(statement3, 1, $1); }
	| selection_statement   { $$ = p(statement4, 1, $1); }
	| iteration_statement   { $$ = p(statement5, 1, $1); }
	| jump_statement        { $$ = p(statement6, 1, $1); }
	| declaration_statement { $$ = p(statement7, 1, $1); }
	;

labeled_statement:
	CASE constant_expression ':' statement { $$ = p(labeled-statement1, 4, $1, $2, $3, $4); }
	| DEFAULT ':' statement		       { $$ = p(labeled-statement2, 3, $1, $2, $3); }
	;

expression_statement:
	expression_opt ';' { $$ = p(expr-statement, 2, $1, $2); }
	;

compound_statement:
	'{' statement_seq_opt '}' { $$ = p(compound-statement, 3, $1, $2, $3); }
	;

statement_seq:
	statement		  { $$ = p(statement-seq1, 1, $1); }
	| statement_seq statement { $$ = p(statement-seq2, 2, $1, $2); }
	;

selection_statement:
	IF '(' condition ')' statement		        { $$ = p(select1, 5, $1, $2, $3, $4, $5); }
	| IF '(' condition ')' statement ELSE statement { $$ = p(select2, 7, $1, $2, $3, $4, $5, $6, $7); }
	| SWITCH '(' condition ')' statement	        { $$ = p(select3, 5, $1, $2, $3, $4, $5); }
	;

condition:
	expression						  { $$ = p(condition1, 1, $1); }
	| type_specifier_seq declarator '=' assignment_expression { $$ = p(condition2, 4, $1, $2, $3, $4); }
	;

iteration_statement:
	WHILE '(' condition ')' statement					    { $$ = p(iter1, 5, $1, $2, $3, $4, $5); }
	| DO statement WHILE '(' expression ')' ';'				    { $$ = p(iter2, 7, $1, $2, $3, $4, $5, $6, $7); }
	| FOR '(' for_init_statement condition_opt ';' expression_opt ')' statement { $$ = p(iter3, 8, $1, $2, $3, $4, $5, $6, $7, $8); }
	;

for_init_statement:
	expression_statement { $$ = p(for-init1, 1, $1); }
	| simple_declaration { $$ = p(for-init2, 1, $1); }
	;

jump_statement:
	BREAK ';'		    { $$ = p(jump1, 2, $1, $2); }
	| CONTINUE ';'		    { $$ = p(jump2, 2, $1, $2); }
	| RETURN expression_opt ';' { $$ = p(jump3, 3, $1, $2, $3); }
	;

declaration_statement:
	block_declaration { $$ = p(decl-statement, 1, $1); }
	;

/*----------------------------------------------------------------------
 * Declarations.
 *----------------------------------------------------------------------*/

declaration_seq:
	declaration		      { $$ = p(decl-seq1, 1, $1); }
	| declaration_seq declaration { $$ = p(decl-seq2, 2, $1, $2); }
	;

declaration:
	block_declaration     { $$ = p(decl1, 1, $1); }
	| function_definition { $$ = p(decl2, 1, $1); }
	;

block_declaration:
	simple_declaration { $$ = p(block-decl, 1, $1); }
	;

simple_declaration:
	decl_specifier_seq init_declarator_list ';' { $$ = p(simple-decl1, 3, $1, $2, $3); }
	| decl_specifier_seq ';'		    { $$ = p(simple-decl2, 2, $1, $2); }
	;

decl_specifier:
	type_specifier { $$ = p(decl-spec, 1, $1); }
	;

decl_specifier_seq:
	decl_specifier			    { $$ = p(decl-spec-seq1, 1, $1); }
	| decl_specifier_seq decl_specifier { $$ = p(decl-spec-seq2, 2, $1, $2); }
	;

type_specifier:
	simple_type_specifier	    { $$ = p(type-spec1, 1, $1); }
	| class_specifier	    { $$ = p(type-spec2, 1, $1); }
	| elaborated_type_specifier { $$ = p(type-spec3, 1, $1); }
	;

simple_type_specifier:
	CLASS_NAME				      { $$ = p(simple-type-spec1, 1, $1); }
	| nested_name_specifier CLASS_NAME	      { $$ = p(simple-type-spec2, 2, $1, $2); }
	| COLONCOLON nested_name_specifier CLASS_NAME { $$ = p(simple-type-spec3, 3, $1, $2, $3); }
	| COLONCOLON CLASS_NAME			      { $$ = p(simple-type-spec4, 2, $1, $2); }
	| CHAR					      { $$ = p(simple-type-spec5, 1, $1); }
	| BOOL					      { $$ = p(simple-type-spec6, 1, $1); }
	| SHORT					      { $$ = p(simple-type-spec7, 1, $1); }
	| INT					      { $$ = p(simple-type-spec8, 1, $1); }
	| LONG					      { $$ = p(simple-type-spec9, 1, $1); }
	| SIGNED				      { $$ = p(simple-type-spec1, 1, $1); }
	| UNSIGNED				      { $$ = p(simple-type-spec1, 1, $1); }
	| FLOAT					      { $$ = p(simple-type-spec1, 1, $1); }
	| DOUBLE				      { $$ = p(simple-type-spec1, 1, $1); }
	| VOID					      { $$ = p(simple-type-spec1, 1, $1); }
	;

elaborated_type_specifier:
	class_key COLONCOLON nested_name_specifier IDENTIFIER { $$ = p(elab-type-spec1, 4, $1, $2, $3, $4); }
	| class_key COLONCOLON IDENTIFIER		      { $$ = p(elab-type-spec2, 3, $1, $2, $3); }
	;

/*----------------------------------------------------------------------
 * Declarators.
 *----------------------------------------------------------------------*/

init_declarator_list:
	init_declarator				   { $$ = p(init-decl-list1, 1, $1); }
	| init_declarator_list ',' init_declarator { $$ = p(init-decl-list2, 3, $1, $2, $3); }
	;

init_declarator:
	declarator initializer_opt { $$ = p(init-decl, 2, $1, $2); }
	;

declarator:
	direct_declarator	  { $$ = p(decl1, 1, $1); }
	| ptr_operator declarator { $$ = p(decl2, 2, $1, $2); }
	;

direct_declarator:
	declarator_id								   { $$ = p(direct-decl1, 1, $1); }
	| direct_declarator '(' parameter_declaration_clause ')'		   { $$ = p(direct-decl2, 4, $1, $2, $3, $4); }
	| CLASS_NAME '(' parameter_declaration_clause ')'			   { $$ = p(direct-decl3, 4, $1, $2, $3, $4); }
	| CLASS_NAME COLONCOLON declarator_id '(' parameter_declaration_clause ')' { $$ = p(direct-decl4, 6, $1, $2, $3, $4, $5, $6); }
	| CLASS_NAME COLONCOLON CLASS_NAME '(' parameter_declaration_clause ')'	   { $$ = p(direct-decl5, 6, $1, $2, $3, $4, $5, $6); }
	| direct_declarator '[' constant_expression_opt ']'			   { $$ = p(direct-decl6, 4, $1, $2, $3, $4); }
	| '(' declarator ')'							   { $$ = p(direct-decl7, 3, $1, $2, $3); }
	;

ptr_operator:
	'*'				       { $$ = p(ptr1, 1, $1); }
	| '&'				       { $$ = p(ptr2, 1, $1); }
	| nested_name_specifier '*'	       { $$ = p(ptr3, 2, $1, $2); }
	| COLONCOLON nested_name_specifier '*' { $$ = p(ptr4, 3, $1, $2, $3); }
	;

declarator_id:
	id_expression				      { $$ = p(decl-d1, 1, $1); }
	| COLONCOLON id_expression		      { $$ = p(decl-d2, 2, $1, $2); }
	| COLONCOLON nested_name_specifier CLASS_NAME { $$ = p(decl-d3, 3, $1, $2, $3); }
	| COLONCOLON CLASS_NAME			      { $$ = p(decl-d4, 2, $1, $2); }
	;

type_id:
	type_specifier_seq abstract_declarator_opt { $$ = p(type-id, 2, $1, $2); }
	;

type_specifier_seq:
	type_specifier type_specifier_seq_opt { $$ = p(type-spec-seq, 2, $1, $2); }
	;

abstract_declarator:
	ptr_operator abstract_declarator_opt { $$ = p(abstract-decl1, 2, $1, $2); }
	| direct_abstract_declarator	     { $$ = p(abstract-decl2, 1, $1); }
	;

direct_abstract_declarator:
	direct_abstract_declarator '(' parameter_declaration_clause ')' { $$ = p(direct-abstract-decl1, 4, $1, $2, $3, $4); }
	| '(' parameter_declaration_clause ')'			        { $$ = p(direct-abstract-decl2, 3, $1, $2, $3); }
	| direct_abstract_declarator '[' constant_expression_opt ']'    { $$ = p(direct-abstract-decl3, 4, $1, $2, $3, $4); }
	| '[' constant_expression_opt ']'			        { $$ = p(direct-abstract-decl4, 3, $1, $2, $3); }
	| '(' abstract_declarator ')'				        { $$ = p(direct-abstract-decl5, 3, $1, $2, $3); }
	;

parameter_declaration_clause:
	parameter_declaration_list { $$ = p(param-decl, 1, $1); }
	| %empty		   { $$ = NULL; }
	;

parameter_declaration_list:
	parameter_declaration				       { $$ = p(param-decl-list1, 1, $1); }
	| parameter_declaration_list ',' parameter_declaration { $$ = p(param-decl-list1, 3, $1, $2, $3); }
	;

parameter_declaration:
	decl_specifier_seq declarator					       { $$ = p(param-decl1, 1, $1); }
	| decl_specifier_seq declarator '=' assignment_expression	       { $$ = p(param-decl2, 4, $1, $2, $3, $4); }
	| decl_specifier_seq abstract_declarator_opt			       { $$ = p(param-decl3, 2, $1, $2); }
	| decl_specifier_seq abstract_declarator_opt '=' assignment_expression { $$ = p(param-decl4, 4, $1, $2, $3, $4); }
	;

function_definition:
	declarator ctor_initializer_opt function_body			   { $$ = p(function-def1, 3, $1, $2, $3); }
	| decl_specifier_seq declarator ctor_initializer_opt function_body { $$ = p(function-def2, 4, $1, $2, $3, $4); }
	;

function_body:
	compound_statement { $$ = p(function-body, 1, $1); }
	;

initializer:
	'=' initializer_clause	  { $$ = p(init1, 2, $1, $2); }
	| '(' expression_list ')' { $$ = p(init2, 3, $1, $2, $3); }
	;

initializer_clause:
	assignment_expression		     { $$ = p(init-clause1, 1, $1); }
	| '{' initializer_list COMMA_opt '}' { $$ = p(init-clause2, 3, $1, $2, $3); }
	| '{' '}'			     { $$ = p(init-clause3, 2, $1, $2); }
	;

initializer_list:
	initializer_clause			  { $$ = p(init-list1, 1, $1); }
	| initializer_list ',' initializer_clause { $$ = p(init-list2, 3, $1, $2, $3); }
	;

/*----------------------------------------------------------------------
 * Classes.
 *----------------------------------------------------------------------*/

class_specifier:
	class_head '{' member_specification_opt '}' { $$ = p(class-spec, 4, $1, $2, $3, $4); }
	;

class_head:
	class_key IDENTIFIER			     { $$ = p(class-head1, 2, $1, $2); typenames_insert_tree($2, CLASS_NAME); }
	| class_key nested_name_specifier IDENTIFIER { $$ = p(class-head2, 3, $1, $2, $3); typenames_insert_tree($3, CLASS_NAME); }
	;

class_key:
	CLASS	 { $$ = p(class-key1, 1, $1); }
	| STRUCT { $$ = p(class-key2, 1, $1); }
	;

member_specification:
	member_declaration member_specification_opt     { $$ = p(member-spec1, 2, $1, $2); }
	| access_specifier ':' member_specification_opt { $$ = p(member-spec2, 3, $1, $2, $3); }
	;

member_declaration:
	decl_specifier_seq member_declarator_list ';' { $$ = p(member-decl1, 3, $1, $2, $3); }
	| decl_specifier_seq ';'		      { $$ = p(member-decl2, 2, $1, $2); }
	| member_declarator_list ';'		      { $$ = p(member-decl3, 2, $1, $2); }
	| ';'					      { $$ = p(member-decl4, 1, $1); }
	| function_definition SEMICOLON_opt	      { $$ = p(member-decl5, 2, $1, $2); }
	| qualified_id ';'			      { $$ = p(member-decl6, 2, $1, $2); }
	;

member_declarator_list:
	member_declarator			       { $$ = p(member-decl-list1, 1, $1); }
	| member_declarator_list ',' member_declarator { $$ = p(member-decl-list2, 3, $1, $2, $3); }
	;

member_declarator:
	declarator			     { $$ = p(member-declarator1, 1, $1); }
	| declarator constant_initializer    { $$ = p(member-declarator2, 2, $1, $2); }
	| IDENTIFIER ':' constant_expression { $$ = p(member-declarator3, 3, $1, $2, $3); }
	;

constant_initializer:
	'=' constant_expression { $$ = p(constant-init, 2, $1, $2); }
	;

access_specifier:
	PRIVATE	    { $$ = p(access-spec1, 1, $1); }
	| PROTECTED { $$ = p(access-spec2, 1, $1); }
	| PUBLIC    { $$ = p(access-spec3, 1, $1); }
	;

/*----------------------------------------------------------------------
 * Special member functions.
 *----------------------------------------------------------------------*/

ctor_initializer:
	':' mem_initializer_list { $$ = p(ctor-init, 2, $1, $2); }
	;

mem_initializer_list:
	mem_initializer				   { $$ = p(mem-init-list1, 1, $1); }
	| mem_initializer ',' mem_initializer_list { $$ = p(mem-init-list2, 3, $1, $2, $3); }
	;

mem_initializer:
	mem_initializer_id '(' expression_list_opt ')' { $$ = p(mem-init, 4, $1, $2, $3, $4); }
	;

mem_initializer_id:
	COLONCOLON nested_name_specifier CLASS_NAME { $$ = p(mem-init-id1, 3, $1, $2, $3); }
	| COLONCOLON CLASS_NAME			    { $$ = p(mem-init-id2, 2, $1, $2); }
	| nested_name_specifier CLASS_NAME	    { $$ = p(mem-init-id3, 2, $1, $2); }
	| CLASS_NAME				    { $$ = p(mem-init-id4, 1, $1); }
	| IDENTIFIER				    { $$ = p(mem-init-id5, 1, $1); }
	;

/*----------------------------------------------------------------------
 * Epsilon (optional) definitions.
 *----------------------------------------------------------------------*/

declaration_seq_opt:
	%empty		  { $$ = NULL; }
	| declaration_seq { $$ = p(decl-seq-opt, 1, $1); }
	;

expression_list_opt:
	%empty		  { $$ = NULL; }
	| expression_list { $$ = p(expr-list-opt, 1, $1); }
	;

new_placement_opt:
	%empty	        { $$ = NULL; }
	| new_placement { $$ = p(new-placement-opt, 1, $1); }
	;

new_initializer_opt:
	%empty		  { $$ = NULL; }
	| new_initializer { $$ = p(new-init-opt, 1, $1); }
	;

new_declarator_opt:
	%empty		 { $$ = NULL; }
	| new_declarator { $$ = p(new-decl-opt, 1, $1); }
	;

expression_opt:
	%empty	     { $$ = NULL; }
	| expression { $$ = p(expr-opt, 1, $1); }
	;

statement_seq_opt:
	%empty	        { $$ = NULL; }
	| statement_seq { $$ = p(statement-opt, 1, $1); }
	;

condition_opt:
	%empty	    { $$ = NULL; }
	| condition { $$ = p(cond-opt, 1, $1); }
	;

initializer_opt:
	%empty	      { $$ = NULL; }
	| initializer { $$ = p(init-opt, 1, $1); }
	;

constant_expression_opt:
	%empty		      { $$ = NULL; }
	| constant_expression { $$ = p(const-expr-opt, 1, $1); }
	;

abstract_declarator_opt:
	%empty		      { $$ = NULL; }
	| abstract_declarator { $$ = p(abstract-decl-opt, 1, $1); }
	;

type_specifier_seq_opt:
	%empty		     { $$ = NULL; }
	| type_specifier_seq { $$ = p(type-spec-seq-opt, 1, $1); }
	;

ctor_initializer_opt:
	%empty		   { $$ = NULL; }
	| ctor_initializer { $$ = p(ctor-init-opt, 1, $1); }
	;

COMMA_opt:
	%empty { $$ = NULL; }
	| ','  { $$ = p(comma-opt, 1, $1); }
	;

member_specification_opt:
	%empty		       { $$ = NULL; }
	| member_specification { $$ = p(member-spec-opt, 1, $1); }
	;

SEMICOLON_opt:
	%empty { $$ = NULL; }
	| ';'  { $$ = p(semicol-opt, 1, $1); }
	;

%%
