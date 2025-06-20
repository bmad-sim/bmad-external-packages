/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_H5LTYY_HL_SRC_H5LTPARSE_H_INCLUDED
# define YY_H5LTYY_HL_SRC_H5LTPARSE_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int H5LTyydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    H5T_STD_I8BE_TOKEN = 258,      /* H5T_STD_I8BE_TOKEN  */
    H5T_STD_I8LE_TOKEN = 259,      /* H5T_STD_I8LE_TOKEN  */
    H5T_STD_I16BE_TOKEN = 260,     /* H5T_STD_I16BE_TOKEN  */
    H5T_STD_I16LE_TOKEN = 261,     /* H5T_STD_I16LE_TOKEN  */
    H5T_STD_I32BE_TOKEN = 262,     /* H5T_STD_I32BE_TOKEN  */
    H5T_STD_I32LE_TOKEN = 263,     /* H5T_STD_I32LE_TOKEN  */
    H5T_STD_I64BE_TOKEN = 264,     /* H5T_STD_I64BE_TOKEN  */
    H5T_STD_I64LE_TOKEN = 265,     /* H5T_STD_I64LE_TOKEN  */
    H5T_STD_U8BE_TOKEN = 266,      /* H5T_STD_U8BE_TOKEN  */
    H5T_STD_U8LE_TOKEN = 267,      /* H5T_STD_U8LE_TOKEN  */
    H5T_STD_U16BE_TOKEN = 268,     /* H5T_STD_U16BE_TOKEN  */
    H5T_STD_U16LE_TOKEN = 269,     /* H5T_STD_U16LE_TOKEN  */
    H5T_STD_U32BE_TOKEN = 270,     /* H5T_STD_U32BE_TOKEN  */
    H5T_STD_U32LE_TOKEN = 271,     /* H5T_STD_U32LE_TOKEN  */
    H5T_STD_U64BE_TOKEN = 272,     /* H5T_STD_U64BE_TOKEN  */
    H5T_STD_U64LE_TOKEN = 273,     /* H5T_STD_U64LE_TOKEN  */
    H5T_NATIVE_CHAR_TOKEN = 274,   /* H5T_NATIVE_CHAR_TOKEN  */
    H5T_NATIVE_SCHAR_TOKEN = 275,  /* H5T_NATIVE_SCHAR_TOKEN  */
    H5T_NATIVE_UCHAR_TOKEN = 276,  /* H5T_NATIVE_UCHAR_TOKEN  */
    H5T_NATIVE_SHORT_TOKEN = 277,  /* H5T_NATIVE_SHORT_TOKEN  */
    H5T_NATIVE_USHORT_TOKEN = 278, /* H5T_NATIVE_USHORT_TOKEN  */
    H5T_NATIVE_INT_TOKEN = 279,    /* H5T_NATIVE_INT_TOKEN  */
    H5T_NATIVE_UINT_TOKEN = 280,   /* H5T_NATIVE_UINT_TOKEN  */
    H5T_NATIVE_LONG_TOKEN = 281,   /* H5T_NATIVE_LONG_TOKEN  */
    H5T_NATIVE_ULONG_TOKEN = 282,  /* H5T_NATIVE_ULONG_TOKEN  */
    H5T_NATIVE_LLONG_TOKEN = 283,  /* H5T_NATIVE_LLONG_TOKEN  */
    H5T_NATIVE_ULLONG_TOKEN = 284, /* H5T_NATIVE_ULLONG_TOKEN  */
    H5T_IEEE_F16BE_TOKEN = 285,    /* H5T_IEEE_F16BE_TOKEN  */
    H5T_IEEE_F16LE_TOKEN = 286,    /* H5T_IEEE_F16LE_TOKEN  */
    H5T_IEEE_F32BE_TOKEN = 287,    /* H5T_IEEE_F32BE_TOKEN  */
    H5T_IEEE_F32LE_TOKEN = 288,    /* H5T_IEEE_F32LE_TOKEN  */
    H5T_IEEE_F64BE_TOKEN = 289,    /* H5T_IEEE_F64BE_TOKEN  */
    H5T_IEEE_F64LE_TOKEN = 290,    /* H5T_IEEE_F64LE_TOKEN  */
    H5T_NATIVE_FLOAT16_TOKEN = 291, /* H5T_NATIVE_FLOAT16_TOKEN  */
    H5T_NATIVE_FLOAT_TOKEN = 292,  /* H5T_NATIVE_FLOAT_TOKEN  */
    H5T_NATIVE_DOUBLE_TOKEN = 293, /* H5T_NATIVE_DOUBLE_TOKEN  */
    H5T_NATIVE_LDOUBLE_TOKEN = 294, /* H5T_NATIVE_LDOUBLE_TOKEN  */
    H5T_STRING_TOKEN = 295,        /* H5T_STRING_TOKEN  */
    STRSIZE_TOKEN = 296,           /* STRSIZE_TOKEN  */
    STRPAD_TOKEN = 297,            /* STRPAD_TOKEN  */
    CSET_TOKEN = 298,              /* CSET_TOKEN  */
    CTYPE_TOKEN = 299,             /* CTYPE_TOKEN  */
    H5T_VARIABLE_TOKEN = 300,      /* H5T_VARIABLE_TOKEN  */
    H5T_STR_NULLTERM_TOKEN = 301,  /* H5T_STR_NULLTERM_TOKEN  */
    H5T_STR_NULLPAD_TOKEN = 302,   /* H5T_STR_NULLPAD_TOKEN  */
    H5T_STR_SPACEPAD_TOKEN = 303,  /* H5T_STR_SPACEPAD_TOKEN  */
    H5T_CSET_ASCII_TOKEN = 304,    /* H5T_CSET_ASCII_TOKEN  */
    H5T_CSET_UTF8_TOKEN = 305,     /* H5T_CSET_UTF8_TOKEN  */
    H5T_C_S1_TOKEN = 306,          /* H5T_C_S1_TOKEN  */
    H5T_FORTRAN_S1_TOKEN = 307,    /* H5T_FORTRAN_S1_TOKEN  */
    H5T_OPAQUE_TOKEN = 308,        /* H5T_OPAQUE_TOKEN  */
    OPQ_SIZE_TOKEN = 309,          /* OPQ_SIZE_TOKEN  */
    OPQ_TAG_TOKEN = 310,           /* OPQ_TAG_TOKEN  */
    H5T_COMPOUND_TOKEN = 311,      /* H5T_COMPOUND_TOKEN  */
    H5T_ENUM_TOKEN = 312,          /* H5T_ENUM_TOKEN  */
    H5T_ARRAY_TOKEN = 313,         /* H5T_ARRAY_TOKEN  */
    H5T_VLEN_TOKEN = 314,          /* H5T_VLEN_TOKEN  */
    STRING = 315,                  /* STRING  */
    NUMBER = 316                   /* NUMBER  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 68 "hl/src//H5LTparse.y"

    int     ival;         /*for integer token*/
    char    *sval;        /*for name string*/
    hid_t   hid;          /*for hid_t token*/

#line 131 "hl/src//H5LTparse.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE H5LTyylval;


hid_t H5LTyyparse (void);


#endif /* !YY_H5LTYY_HL_SRC_H5LTPARSE_H_INCLUDED  */
