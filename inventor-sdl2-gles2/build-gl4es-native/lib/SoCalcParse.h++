/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
     LEXERR = 258,
     CONST = 259,
     FUNC = 260,
     INPUT = 261,
     OUTPUT = 262,
     VAR = 263,
     OROR = 264,
     ANDAND = 265,
     GEQ = 266,
     LEQ = 267,
     EQEQ = 268,
     NEQ = 269
   };
#endif
/* Tokens.  */
#define LEXERR 258
#define CONST 259
#define FUNC 260
#define INPUT 261
#define OUTPUT 262
#define VAR 263
#define OROR 264
#define ANDAND 265
#define GEQ 266
#define LEQ 267
#define EQEQ 268
#define NEQ 269




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 114 "/Users/larsens/Github/sgi-inventor/inventor-sdl2-gles2/lib/database/src/so/engines/SoCalcParse.y"
{
    Expr	*expr;
    ExprList	*list;
}
/* Line 1529 of yacc.c.  */
#line 82 "/Users/larsens/Github/sgi-inventor/inventor-sdl2-gles2/build-gl4es-native/lib/SoCalcParse.h++"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

