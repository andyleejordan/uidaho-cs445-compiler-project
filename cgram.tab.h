
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     BAD_TOKEN = 258,
     ICON = 259,
     CCON = 260,
     FCON = 261,
     ENUMERATION_CONSTANT = 262,
     IDENTIFIER = 263,
     STRING = 264,
     SIZEOF = 265,
     INCOP = 266,
     DECOP = 267,
     SHL = 268,
     SHR = 269,
     LE = 270,
     GE = 271,
     EQ = 272,
     NE = 273,
     ANDAND = 274,
     OROR = 275,
     MUASN = 276,
     DIASN = 277,
     MOASN = 278,
     PLASN = 279,
     ASN = 280,
     MIASN = 281,
     SLASN = 282,
     SRASN = 283,
     ANASN = 284,
     ERASN = 285,
     ORASN = 286,
     TYPEDEF_NAME = 287,
     CM = 288,
     SM = 289,
     LT = 290,
     GT = 291,
     PLUS = 292,
     MINUS = 293,
     MUL = 294,
     DIV = 295,
     MOD = 296,
     LP = 297,
     RP = 298,
     LB = 299,
     RB = 300,
     LC = 301,
     RC = 302,
     COLON = 303,
     QUEST = 304,
     AND = 305,
     OR = 306,
     ER = 307,
     NOT = 308,
     FOLLOW = 309,
     BANG = 310,
     DOT = 311,
     SP = 312,
     DTAB = 313,
     ITAB = 314,
     EOL = 315,
     TAB = 316,
     ECNT = 317,
     TYPEDEF = 318,
     EXTERN = 319,
     STATIC = 320,
     AUTO = 321,
     REGISTER = 322,
     CHAR = 323,
     SHORT = 324,
     INT = 325,
     LONG = 326,
     SIGNED = 327,
     UNSIGNED = 328,
     FLOAT = 329,
     DOUBLE = 330,
     CONST = 331,
     VOLATILE = 332,
     VOID = 333,
     STRUCT = 334,
     UNION = 335,
     ENUM = 336,
     ELIPSIS = 337,
     CASE = 338,
     DEFAULT = 339,
     IF = 340,
     SWITCH = 341,
     WHILE = 342,
     DO = 343,
     FOR = 344,
     GOTO = 345,
     CONTINUE = 346,
     BREAK = 347,
     RETURN = 348,
     THEN = 349,
     ELSE = 350
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 19 "cgram.y"

struct token *tokptr;
struct tree *treeptr;



/* Line 1676 of yacc.c  */
#line 154 "cgram.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


