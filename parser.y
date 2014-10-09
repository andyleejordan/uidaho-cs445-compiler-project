/*
 * parser.y - Merged and modified adaptation of Dr. Jeffery's
 * adaptation of Sigala's grammar.
 */

/*
 * Grammar for 120++, a subset of C++ used in CS 120 at University of Idaho
 *
 * Adaptation by Clinton Jeffery, with help from Matthew Brown, Ranger
 * Adams, and Shea Newton.
 *
 * Based on Sandro Sigala's transcription of the ISO C++ 1996 draft standard.
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

#include "list.h"
#include "tree.h"
#include "lexer.h"

extern struct tree *yyprogram;
extern struct list *filenames;

void yyerror(const char *s);
void typenames_insert_tree(struct tree *t, int category);

#define P(name, ...) tree_initv(NULL, #name, __VA_ARGS__)
#define E() NULL

%}

%require "3.0"
%defines
%expect 21

%union {
        struct tree *t;
}

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
        INTEGER     { $$ = $1; }
        | CHARACTER { $$ = $1; }
        | FLOATING  { $$ = $1; }
        | STRING    { $$ = $1; }
        | boolean   { $$ = $1; }
        ;

boolean:
        TRUE    { $$ = $1; }
        | FALSE { $$ = $1; }
        ;

program:
        declaration_seq_opt { $$ = P(program, 1, $1); yyprogram = $$; }
        ;

/*----------------------------------------------------------------------
 * Expressions.
 *----------------------------------------------------------------------*/

primary_expression:
        literal              { $$ = $1; }
        | '(' expression ')' { $$ = $2; }
        | id_expression      { $$ = $1; }
        ;

id_expression:
        unqualified_id { $$ = $1; }
        | qualified_id { $$ = $1; }
        ;

unqualified_id:
        IDENTIFIER       { $$ = $1; }
        | '~' CLASS_NAME { $$ = P(unqualid-2, 2, $1, $2); }
        ;

qualified_id:
        nested_name_specifier unqualified_id { $$ = P(qual-id, 2, $1, $2); }
        ;

nested_name_specifier:
        CLASS_NAME COLONCOLON nested_name_specifier { $$ = P(nested-name1, 2, $1, $3); }
        | CLASS_NAME COLONCOLON                     { $$ = $1; }
        ;

postfix_expression:
        primary_expression                                  { $$ = $1; }
        | postfix_expression '[' expression ']'             { $$ = P(postfix-expr2, 4, $1, $2, $3, $4); }
        | postfix_expression '(' expression_list_opt ')'    { $$ = P(postfix-expr3, 2, $1, $3); }
        | simple_type_specifier '(' expression_list_opt ')' { $$ = P(postfix-expr4, 2, $1, $3); }
        | postfix_expression '.' COLONCOLON id_expression   { $$ = P(postfix-expr5, 3, $1, $2, $4); }
        | postfix_expression '.' id_expression              { $$ = P(postfix-expr6, 3, $1, $2, $3); }
        | postfix_expression ARROW COLONCOLON id_expression { $$ = P(postfix-expr7, 3, $1, $2, $4); }
        | postfix_expression ARROW id_expression            { $$ = P(postfix-expr8, 3, $1, $2, $3); }
        | postfix_expression PLUSPLUS                       { $$ = P(postfix-expr9, 2, $1, $2); }
        | postfix_expression MINUSMINUS                     { $$ = P(postfix-expr10, 2, $1, $2); }
        ;

expression_list:
        assignment_expression                       { $$ = $1; }
        | expression_list ',' assignment_expression { $$ = P(expr-list2, 2, $1, $3); }
        ;

unary_expression:
        postfix_expression                { $$ = $1; }
        | PLUSPLUS unary_expression       { $$ = P(unary-expr2, 2, $1, $2); }
        | MINUSMINUS unary_expression     { $$ = P(unary-expr3, 2, $1, $2); }
        | '*' unary_expression            { $$ = P(unary-expr4, 2, $1, $2); }
        | '&' unary_expression            { $$ = P(unary-expr5, 2, $1, $2); }
        | unary_operator unary_expression { $$ = P(unary-expr6, 2, $1, $2); }
        | SIZEOF unary_expression         { $$ = P(unary-expr7, 2, $1, $2); }
        | SIZEOF '(' type_id ')'          { $$ = P(unary-expr8, 2, $1, $3); }
        | new_expression                  { $$ = $1; }
        | delete_expression               { $$ = $1; }
        ;

unary_operator:
        '+'   { $$ = $1; }
        | '-' { $$ = $1; }
        | '!' { $$ = $1; }
        | '~' { $$ = $1; }
        ;

new_expression:
        NEW new_placement_opt new_type_id new_initializer_opt              { $$ = P(new-expr1, 4, $1, $2, $3, $4); }
        | COLONCOLON NEW new_placement_opt new_type_id new_initializer_opt { $$ = P(new-expr2, 4, $2, $3, $4, $5); }
        ;

new_placement:
        '(' expression_list ')' { $$ = $2; }
        ;

new_type_id:
        type_specifier_seq new_declarator_opt { $$ = P(new-type-id, 2, $1, $2); }
        ;

new_declarator:
        ptr_operator new_declarator_opt { $$ = P(new-decl1, 2, $1, $2); }
        | direct_new_declarator         { $$ = $1; }
        ;

direct_new_declarator:
        '[' expression ']'                                  { $$ = P(direct-new-decl1, 3, $1, $2, $3); }
        | direct_new_declarator '[' constant_expression ']' { $$ = P(direct-new-decl2, 4, $1, $2, $3, $4); }
        ;

new_initializer:
        '(' expression_list_opt ')' { $$ = $2; }
        ;

delete_expression:
        DELETE unary_expression                      { $$ = P(delete-expr1, 2, $1, $2); }
        | COLONCOLON DELETE unary_expression         { $$ = P(delete-expr2, 2, $2, $3); }
        | DELETE '[' ']' unary_expression            { $$ = P(delete-expr3, 4, $1, $2, $3, $4); }
        | COLONCOLON DELETE '[' ']' unary_expression { $$ = P(delete-expr4, 4, $2, $3, $4, $5); }
        ;

pm_expression:
        unary_expression                           { $$ = $1; }
        | pm_expression DOTSTAR unary_expression   { $$ = P(pm-expr2, 3, $1, $2, $3); }
        | pm_expression ARROWSTAR unary_expression { $$ = P(pm-expr3, 3, $1, $2, $3); }
        ;

multiplicative_expression:
        pm_expression                                 { $$ = $1; }
        | multiplicative_expression '*' pm_expression { $$ = P(mult-expr2, 3, $1, $2, $3); }
        | multiplicative_expression '/' pm_expression { $$ = P(mult-expr3, 3, $1, $2, $3); }
        | multiplicative_expression '%' pm_expression { $$ = P(mult-expr4, 3, $1, $2, $3); }
        ;

additive_expression:
        multiplicative_expression                           { $$ = $1; }
        | additive_expression '+' multiplicative_expression { $$ = P(add-expr2, 3, $1, $2, $3); }
        | additive_expression '-' multiplicative_expression { $$ = P(add-expr3, 3, $1, $2, $3); }
        ;

shift_expression:
        additive_expression                       { $$ = $1; }
        | shift_expression SL additive_expression { $$ = P(shift-expr2, 3, $1, $2, $3); }
        | shift_expression SR additive_expression { $$ = P(shift-expr3, 3, $1, $2, $3); }
        ;

relational_expression:
        shift_expression                              { $$ = $1; }
        | relational_expression '<' shift_expression  { $$ = P(rel-expr2, 3, $1, $2, $3); }
        | relational_expression '>' shift_expression  { $$ = P(rel-expr3, 3, $1, $2, $3); }
        | relational_expression LTEQ shift_expression { $$ = P(rel-expr4, 3, $1, $2, $3); }
        | relational_expression GTEQ shift_expression { $$ = P(rel-expr5, 3, $1, $2, $3); }
        ;

equality_expression:
        relational_expression                             { $$ = $1; }
        | equality_expression EQ relational_expression    { $$ = P(equal-expr2, 3, $1, $2, $3); }
        | equality_expression NOTEQ relational_expression { $$ = P(equal-expr3, 3, $1, $2, $3); }
        ;

and_expression:
        equality_expression                      { $$ = $1; }
        | and_expression '&' equality_expression { $$ = P(and-expr2, 3, $1, $2, $3); }
        ;

exclusive_or_expression:
        and_expression                               { $$ = $1; }
        | exclusive_or_expression '^' and_expression { $$ = P(xor-expr2, 3, $1, $2, $3); }
        ;

inclusive_or_expression:
        exclusive_or_expression                               { $$ = $1; }
        | inclusive_or_expression '|' exclusive_or_expression { $$ = P(or-expr2, 3, $1, $2, $3); }
        ;

logical_and_expression:
        inclusive_or_expression                                 { $$ = $1; }
        | logical_and_expression ANDAND inclusive_or_expression { $$ = P(logical-and-expr2, 3, $1, $2, $3); }
        ;

logical_or_expression:
        logical_and_expression                              { $$ = $1; }
        | logical_or_expression OROR logical_and_expression { $$ = P(logical-or-expr2, 3, $1, $2, $3); }
        ;

conditional_expression:
        logical_or_expression                                            { $$ = $1; }
        | logical_or_expression '?' expression ':' assignment_expression { $$ = P(cond-expr2, 5, $1, $2, $3, $4, $5); }
        ;

assignment_expression:
        conditional_expression                                            { $$ = $1; }
        | logical_or_expression assignment_operator assignment_expression { $$ = P(assign-expr2, 3, $1, $2, $3); }
        ;

assignment_operator:
        '='     { $$ = $1; }
        | MULEQ { $$ = $1; }
        | DIVEQ { $$ = $1; }
        | MODEQ { $$ = $1; }
        | ADDEQ { $$ = $1; }
        | SUBEQ { $$ = $1; }
        | SREQ  { $$ = $1; }
        | SLEQ  { $$ = $1; }
        | ANDEQ { $$ = $1; }
        | XOREQ { $$ = $1; }
        | OREQ  { $$ = $1; }
        ;

expression:
        assignment_expression                  { $$ = $1; }
        | expression ',' assignment_expression { $$ = P(expr2, 2, $1, $3); }
        ;

constant_expression:
        conditional_expression { $$ = $1; }
        ;

/*----------------------------------------------------------------------
 * Statements.
 *----------------------------------------------------------------------*/

statement:
        labeled_statement       { $$ = $1; }
        | expression_statement  { $$ = $1; }
        | compound_statement    { $$ = $1; }
        | selection_statement   { $$ = $1; }
        | iteration_statement   { $$ = $1; }
        | jump_statement        { $$ = $1; }
        | declaration_statement { $$ = $1; }
        ;

labeled_statement:
        CASE constant_expression ':' statement { $$ = P(labeled-statement1, 4, $1, $2, $3, $4); }
        | DEFAULT ':' statement                { $$ = P(labeled-statement2, 3, $1, $2, $3); }
        ;

expression_statement:
        expression_opt ';' { $$ = $1; }
        ;

compound_statement:
        '{' statement_seq_opt '}' { $$ = $2; }
        ;

statement_seq:
        statement                 { $$ = $1; }
        | statement_seq statement { $$ = P(statement-seq2, 2, $1, $2); }
        ;

selection_statement:
        IF '(' condition ')' statement                  { $$ = P(select1, 3, $1, $3, $5); }
        | IF '(' condition ')' statement ELSE statement { $$ = P(select2, 5, $1, $3, $5, $6, $7); }
        | SWITCH '(' condition ')' statement            { $$ = P(select3, 3, $1, $3, $5); }
        ;

condition:
        expression                                                { $$ = $1; }
        | type_specifier_seq declarator '=' assignment_expression { $$ = P(condition2, 4, $1, $2, $3, $4); }
        ;

iteration_statement:
        WHILE '(' condition ')' statement                                           { $$ = P(iter1, 3, $1, $3, $5); }
        | DO statement WHILE '(' expression ')' ';'                                 { $$ = P(iter2, 4, $1, $2, $3, $5); }
        | FOR '(' for_init_statement condition_opt ';' expression_opt ')' statement { $$ = P(iter3, 5, $1, $3, $4, $6, $8); }
        ;

for_init_statement:
        expression_statement { $$ = $1; }
        | simple_declaration { $$ = $1; }
        ;

jump_statement:
        BREAK ';'                   { $$ = $1; }
        | CONTINUE ';'              { $$ = $1; }
        | RETURN expression_opt ';' { $$ = P(jump3, 2, $1, $2); }
        ;

declaration_statement:
        block_declaration { $$ = $1; }
        ;

/*----------------------------------------------------------------------
 * Declarations.
 *----------------------------------------------------------------------*/

declaration_seq:
        declaration                   { $$ = $1; }
        | declaration_seq declaration { $$ = P(decl-seq2, 2, $1, $2); }
        ;

declaration:
        block_declaration     { $$ = $1; }
        | function_definition { $$ = $1; }
        ;

block_declaration:
        simple_declaration { $$ = $1; }
        ;

simple_declaration:
        decl_specifier_seq init_declarator_list ';' { $$ = P(simple-decl1, 2, $1, $2); }
        | decl_specifier_seq ';'                    { $$ = $1; }
        ;

decl_specifier:
        type_specifier { $$ = $1; }
        ;

decl_specifier_seq:
        decl_specifier                      { $$ = $1; }
        | decl_specifier_seq decl_specifier { $$ = P(decl-spec-seq2, 2, $1, $2); }
        ;

type_specifier:
        simple_type_specifier       { $$ = $1; }
        | class_specifier           { $$ = $1; }
        | elaborated_type_specifier { $$ = $1; }
        ;

simple_type_specifier:
        CLASS_NAME                                    { $$ = $1; }
        | nested_name_specifier CLASS_NAME            { $$ = P(simple-type-spec2, 2, $1, $2); }
        | CHAR                                        { $$ = $1; }
        | BOOL                                        { $$ = $1; }
        | SHORT                                       { $$ = $1; }
        | INT                                         { $$ = $1; }
        | LONG                                        { $$ = $1; }
        | SIGNED                                      { $$ = $1; }
        | UNSIGNED                                    { $$ = $1; }
        | FLOAT                                       { $$ = $1; }
        | DOUBLE                                      { $$ = $1; }
        | VOID                                        { $$ = $1; }
        ;

elaborated_type_specifier:
        class_key COLONCOLON nested_name_specifier IDENTIFIER { $$ = P(elab-type-spec1, 3, $1, $3, $4); }
        | class_key COLONCOLON IDENTIFIER                     { $$ = P(elab-type-spec2, 2, $1, $3); }
        ;

/*----------------------------------------------------------------------
 * Declarators.
 *----------------------------------------------------------------------*/

init_declarator_list:
        init_declarator                            { $$ = $1; }
        | init_declarator_list ',' init_declarator { $$ = P(init-decl-list2, 2, $1, $3); }
        ;

init_declarator:
        declarator initializer_opt { $$ = P(init-decl, 2, $1, $2); }
        ;

declarator:
        direct_declarator         { $$ = $1; }
        | ptr_operator declarator { $$ = P(decl2, 2, $1, $2); }
        ;

direct_declarator:
        declarator_id                                                              { $$ = $1; }
        | direct_declarator '(' parameter_declaration_clause ')'                   { $$ = P(direct-decl2, 2, $1, $3); }
        | CLASS_NAME '(' parameter_declaration_clause ')'                          { $$ = P(direct-decl3, 2, $1, $3); }
        | CLASS_NAME COLONCOLON declarator_id '(' parameter_declaration_clause ')' { $$ = P(direct-decl4, 3, $1, $3, $5); }
        | CLASS_NAME COLONCOLON CLASS_NAME '(' parameter_declaration_clause ')'    { $$ = P(direct-decl5, 3, $1, $3, $5); }
        | direct_declarator '[' constant_expression_opt ']'                        { $$ = P(direct-decl6, 4, $1, $2, $3, $4); }
        | '(' declarator ')'                                                       { $$ = $2; }
        ;

ptr_operator:
        '*'                                    { $$ = $1; }
        | '&'                                  { $$ = $1; }
        | nested_name_specifier '*'            { $$ = P(ptr3, 2, $1, $2); }
        | COLONCOLON nested_name_specifier '*' { $$ = P(ptr4, 2, $2, $3); }
        ;

declarator_id:
        id_expression                                 { $$ = $1; }
        | COLONCOLON id_expression                    { $$ = $2; }
        | COLONCOLON nested_name_specifier CLASS_NAME { $$ = P(decl-d3, 2, $2, $3); }
        | COLONCOLON CLASS_NAME                       { $$ = $2; }
        ;

type_id:
        type_specifier_seq abstract_declarator_opt { $$ = P(type-id, 2, $1, $2); }
        ;

type_specifier_seq:
        type_specifier type_specifier_seq_opt { $$ = P(type-spec-seq, 2, $1, $2); }
        ;

abstract_declarator:
        ptr_operator abstract_declarator_opt { $$ = P(abstract-decl1, 2, $1, $2); }
        | direct_abstract_declarator         { $$ = $1; }
        ;

direct_abstract_declarator:
        direct_abstract_declarator '(' parameter_declaration_clause ')' { $$ = P(direct-abstract-decl1, 2, $1, $3); }
        | '(' parameter_declaration_clause ')'                          { $$ = $2; }
        | direct_abstract_declarator '[' constant_expression_opt ']'    { $$ = P(direct-abstract-decl3, 4, $1, $2, $3, $4); }
        | '[' constant_expression_opt ']'                               { $$ = P(direct-abstract-decl4, 3, $1, $2, $3); }
        | '(' abstract_declarator ')'                                   { $$ = $2; }
        ;

parameter_declaration_clause:
        parameter_declaration_list { $$ = $1; }
        | %empty                   { $$ = E(); }
        ;

parameter_declaration_list:
        parameter_declaration                                  { $$ = $1; }
        | parameter_declaration_list ',' parameter_declaration { $$ = P(param-decl-list1, 2, $1, $3); }
        ;

parameter_declaration:
        decl_specifier_seq declarator                                          { $$ = P(param-decl2, 2, $1, $2); }
        | decl_specifier_seq declarator '=' assignment_expression              { $$ = P(param-decl2, 4, $1, $2, $3, $4); }
        | decl_specifier_seq abstract_declarator_opt                           { $$ = P(param-decl3, 2, $1, $2); }
        | decl_specifier_seq abstract_declarator_opt '=' assignment_expression { $$ = P(param-decl4, 4, $1, $2, $3, $4); }
        ;

function_definition:
        declarator ctor_initializer_opt function_body                      { $$ = P(function-def1, 3, $1, $2, $3); }
        | decl_specifier_seq declarator ctor_initializer_opt function_body { $$ = P(function-def2, 4, $1, $2, $3, $4); }
        ;

function_body:
        compound_statement { $$ = $1; }
        ;

initializer:
        '=' initializer_clause    { $$ = P(init1, 2, $1, $2); }
        | '(' expression_list ')' { $$ = $2; }
        ;

initializer_clause:
        assignment_expression                { $$ = $1; }
        | '{' initializer_list COMMA_opt '}' { $$ = P(init-clause2, 2, $2, $3); }
        | '{' '}'                            { $$ = E(); }
        ;

initializer_list:
        initializer_clause                        { $$ = $1; }
        | initializer_list ',' initializer_clause { $$ = P(init-list2, 2, $1, $3); }
        ;

/*----------------------------------------------------------------------
 * Classes.
 *----------------------------------------------------------------------*/

class_specifier:
        class_head '{' member_specification_opt '}' { $$ = P(class-spec, 2, $1, $3); }
        ;

class_head:
        class_key IDENTIFIER                         { $$ = P(class-head1, 2, $1, $2); typenames_insert_tree($2, CLASS_NAME); }
        | class_key nested_name_specifier IDENTIFIER { $$ = P(class-head2, 3, $1, $2, $3); typenames_insert_tree($3, CLASS_NAME); }
        ;

class_key:
        CLASS    { $$ = $1; }
        | STRUCT { $$ = $1; }
        ;

member_specification:
        member_declaration member_specification_opt     { $$ = P(member-spec1, 2, $1, $2); }
        | access_specifier ':' member_specification_opt { $$ = P(member-spec2, 3, $1, $2, $3); }
        ;

member_declaration:
        decl_specifier_seq member_declarator_list ';' { $$ = P(member-decl1, 2, $1, $2); }
        | decl_specifier_seq ';'                      { $$ = $1; }
        | member_declarator_list ';'                  { $$ = $1; }
        | ';'                                         { $$ = E(); }
        | function_definition SEMICOLON_opt           { $$ = P(member-decl5, 2, $1, $2); }
        | qualified_id ';'                            { $$ = $1; }
        ;

member_declarator_list:
        member_declarator                              { $$ = $1; }
        | member_declarator_list ',' member_declarator { $$ = P(member-decl-list2, 2, $1, $3); }
        ;

member_declarator:
        declarator                           { $$ = $1; }
        | declarator constant_initializer    { $$ = P(member-declarator2, 2, $1, $2); }
        | IDENTIFIER ':' constant_expression { $$ = P(member-declarator3, 3, $1, $2, $3); }
        ;

constant_initializer:
        '=' constant_expression { $$ = P(constant-init, 2, $1, $2); }
        ;

access_specifier:
        PRIVATE     { $$ = $1; }
        | PROTECTED { $$ = $1; }
        | PUBLIC    { $$ = $1; }
        ;

/*----------------------------------------------------------------------
 * Special member functions.
 *----------------------------------------------------------------------*/

ctor_initializer:
        ':' mem_initializer_list { $$ = P(ctor-init, 2, $1, $2); }
        ;

mem_initializer_list:
        mem_initializer                            { $$ = $1; }
        | mem_initializer ',' mem_initializer_list { $$ = P(mem-init-list2, 2, $1, $3); }
        ;

mem_initializer:
        mem_initializer_id '(' expression_list_opt ')' { $$ = P(mem-init, 2, $1, $3); }
        ;

mem_initializer_id:
        COLONCOLON nested_name_specifier CLASS_NAME { $$ = P(mem-init-id1, 2, $2, $3); }
        | COLONCOLON CLASS_NAME                     { $$ = $2; }
        | nested_name_specifier CLASS_NAME          { $$ = P(mem-init-id3, 2, $1, $2); }
        | CLASS_NAME                                { $$ = $1; }
        | IDENTIFIER                                { $$ = $1; }
        ;

/*----------------------------------------------------------------------
 * Epsilon (optional) definitions.
 *----------------------------------------------------------------------*/

declaration_seq_opt:
        %empty            { $$ = E(); }
        | declaration_seq { $$ = $1; }
        ;

expression_list_opt:
        %empty            { $$ = E(); }
        | expression_list { $$ = $1; }
        ;

new_placement_opt:
        %empty          { $$ = E(); }
        | new_placement { $$ = $1; }
        ;

new_initializer_opt:
        %empty            { $$ = E(); }
        | new_initializer { $$ = $1; }
        ;

new_declarator_opt:
        %empty           { $$ = E(); }
        | new_declarator { $$ = $1; }
        ;

expression_opt:
        %empty       { $$ = E(); }
        | expression { $$ = $1; }
        ;

statement_seq_opt:
        %empty          { $$ = E(); }
        | statement_seq { $$ = $1; }
        ;

condition_opt:
        %empty      { $$ = E(); }
        | condition { $$ = $1; }
        ;

initializer_opt:
        %empty        { $$ = E(); }
        | initializer { $$ = $1; }
        ;

constant_expression_opt:
        %empty                { $$ = E(); }
        | constant_expression { $$ = $1; }
        ;

abstract_declarator_opt:
        %empty                { $$ = E(); }
        | abstract_declarator { $$ = $1; }
        ;

type_specifier_seq_opt:
        %empty               { $$ = E(); }
        | type_specifier_seq { $$ = $1; }
        ;

ctor_initializer_opt:
        %empty             { $$ = E(); }
        | ctor_initializer { $$ = $1; }
        ;

COMMA_opt:
        %empty { $$ = E(); }
        | ','  { $$ = E(); }
        ;

member_specification_opt:
        %empty                 { $$ = E(); }
        | member_specification { $$ = $1; }
        ;

SEMICOLON_opt:
        %empty { $$ = E(); }
        | ';'  { $$ = E(); }
        ;

%%

#undef P
#undef E

/*
 * Prints relevant information for syntax errors and exits returning 2
 * per assignment requirements.
 */
void yyerror(const char *s)
{
        fprintf(stderr, "Syntax error: file %s, line %d, token %s: %s\n",
                (const char *)list_peek(filenames), yylineno, yytext, s);
        exit(2);
}
