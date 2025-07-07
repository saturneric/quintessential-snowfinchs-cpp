// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


/**
 ** \file Parser.hpp
 ** Define the yy::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_PARSER_HPP_INCLUDED
# define YY_YY_PARSER_HPP_INCLUDED
// "%code requires" blocks.
#line 13 "Parser.y"

  #include "core/AST.h"
  class Driver;

#line 54 "Parser.hpp"


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif
# include "Location.hpp"


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

namespace yy {
#line 189 "Parser.hpp"




  /// A Bison parser.
  class parser
  {
  public:
#ifdef YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define YYSTYPE in C++, use %define api.value.type"
# endif
    typedef YYSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
    {
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {}

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // program
      // root_declaration
      // function
      // struct_def
      // field
      // param_list
      // param
      // block
      // statement
      // simple_statement
      // simple_optional
      // declarator
      // left_value
      // control
      // expression
      // call
      // arg_list
      char dummy1[sizeof (ASTNodePtr)];

      // VALUE_ID
      // VALUE_INTEGER
      // type
      // assign_operator
      char dummy2[sizeof (std::string)];

      // root_declarations
      // field_list
      // param_list_follow
      // statements
      // arg_list_follow
      char dummy3[sizeof (std::vector<ASTNodePtr>)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        TOKEN_YYEMPTY = -2,
    TOKEN_YYEOF = 0,               // "end of file"
    TOKEN_YYerror = 256,           // error
    TOKEN_YYUNDEF = 257,           // "invalid token"
    TOKEN_VALUE_ID = 258,          // VALUE_ID
    TOKEN_VALUE_INTEGER = 259,     // VALUE_INTEGER
    TOKEN_ASSIGN = 260,            // ASSIGN
    TOKEN_PLUS_ASSIGN = 261,       // PLUS_ASSIGN
    TOKEN_SUB_ASSIGN = 262,        // SUB_ASSIGN
    TOKEN_MULT_ASSIGN = 263,       // MULT_ASSIGN
    TOKEN_SLASH_ASSIGN = 264,      // SLASH_ASSIGN
    TOKEN_PERCENT_ASSIGN = 265,    // PERCENT_ASSIGN
    TOKEN_BIT_AND_ASSIGN = 266,    // BIT_AND_ASSIGN
    TOKEN_BIT_EX_OR_ASSIGN = 267,  // BIT_EX_OR_ASSIGN
    TOKEN_BIT_OR_ASSIGN = 268,     // BIT_OR_ASSIGN
    TOKEN_LEFT_SHIFT_ASSIGN = 269, // LEFT_SHIFT_ASSIGN
    TOKEN_RIGHT_SHIFT_ASSIGN = 270, // RIGHT_SHIFT_ASSIGN
    TOKEN_PLUS = 271,              // PLUS
    TOKEN_SUB = 272,               // SUB
    TOKEN_MULT = 273,              // MULT
    TOKEN_SLASH = 274,             // SLASH
    TOKEN_PERCENT = 275,           // PERCENT
    TOKEN_LOGIC_NOT = 276,         // LOGIC_NOT
    TOKEN_BIT_NOT = 277,           // BIT_NOT
    TOKEN_SEMICOLON = 278,         // SEMICOLON
    TOKEN_LEFT_PAREN = 279,        // LEFT_PAREN
    TOKEN_RIGHT_PAREN = 280,       // RIGHT_PAREN
    TOKEN_LEFT_BRACKET = 281,      // LEFT_BRACKET
    TOKEN_RIGHT_BRACKET = 282,     // RIGHT_BRACKET
    TOKEN_OPENING_BRACE = 283,     // OPENING_BRACE
    TOKEN_CLOSING_BRACE = 284,     // CLOSING_BRACE
    TOKEN_COMMA = 285,             // COMMA
    TOKEN_LEFT_SHIFT = 286,        // LEFT_SHIFT
    TOKEN_RIGHT_SHIFT = 287,       // RIGHT_SHIFT
    TOKEN_BIT_AND = 288,           // BIT_AND
    TOKEN_BIT_EX_OR = 289,         // BIT_EX_OR
    TOKEN_BIT_OR = 290,            // BIT_OR
    TOKEN_LOGIC_AND = 291,         // LOGIC_AND
    TOKEN_LOGIC_OR = 292,          // LOGIC_OR
    TOKEN_QUESTION = 293,          // QUESTION
    TOKEN_COLON = 294,             // COLON
    TOKEN_LT = 295,                // LT
    TOKEN_LT_EQ = 296,             // LT_EQ
    TOKEN_GT = 297,                // GT
    TOKEN_GT_EQ = 298,             // GT_EQ
    TOKEN_EQ = 299,                // EQ
    TOKEN_NOT_EQ = 300,            // NOT_EQ
    TOKEN_PRINT = 301,             // PRINT
    TOKEN_READ = 302,              // READ
    TOKEN_FLUSH = 303,             // FLUSH
    TOKEN_ALLOC = 304,             // ALLOC
    TOKEN_ALLOC_ARRAY = 305,       // ALLOC_ARRAY
    TOKEN_STRUCT = 306,            // STRUCT
    TOKEN_ARROW = 307,             // ARROW
    TOKEN_DOT = 308,               // DOT
    TOKEN_IF = 309,                // IF
    TOKEN_ELSE = 310,              // ELSE
    TOKEN_WHILE = 311,             // WHILE
    TOKEN_FOR = 312,               // FOR
    TOKEN_CONTINUE = 313,          // CONTINUE
    TOKEN_BREAK = 314,             // BREAK
    TOKEN_RETURN = 315,            // RETURN
    TOKEN_ASSERT = 316,            // ASSERT
    TOKEN_TRUE = 317,              // TRUE
    TOKEN_FALSE = 318,             // FALSE
    TOKEN_KW_NULL = 319,           // KW_NULL
    TOKEN_STRING = 320,            // STRING
    TOKEN_BOOLEAN = 321,           // BOOLEAN
    TOKEN_VOID = 322,              // VOID
    TOKEN_CHAR = 323,              // CHAR
    TOKEN_INT = 324,               // INT
    TOKEN_NONE = 325,              // NONE
    TOKEN_IFX = 326,               // IFX
    TOKEN_CON_EXP = 327,           // CON_EXP
    TOKEN_UMINUS = 328,            // UMINUS
    TOKEN_DEREF = 329              // DEREF
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 75, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_VALUE_ID = 3,                          // VALUE_ID
        S_VALUE_INTEGER = 4,                     // VALUE_INTEGER
        S_ASSIGN = 5,                            // ASSIGN
        S_PLUS_ASSIGN = 6,                       // PLUS_ASSIGN
        S_SUB_ASSIGN = 7,                        // SUB_ASSIGN
        S_MULT_ASSIGN = 8,                       // MULT_ASSIGN
        S_SLASH_ASSIGN = 9,                      // SLASH_ASSIGN
        S_PERCENT_ASSIGN = 10,                   // PERCENT_ASSIGN
        S_BIT_AND_ASSIGN = 11,                   // BIT_AND_ASSIGN
        S_BIT_EX_OR_ASSIGN = 12,                 // BIT_EX_OR_ASSIGN
        S_BIT_OR_ASSIGN = 13,                    // BIT_OR_ASSIGN
        S_LEFT_SHIFT_ASSIGN = 14,                // LEFT_SHIFT_ASSIGN
        S_RIGHT_SHIFT_ASSIGN = 15,               // RIGHT_SHIFT_ASSIGN
        S_PLUS = 16,                             // PLUS
        S_SUB = 17,                              // SUB
        S_MULT = 18,                             // MULT
        S_SLASH = 19,                            // SLASH
        S_PERCENT = 20,                          // PERCENT
        S_LOGIC_NOT = 21,                        // LOGIC_NOT
        S_BIT_NOT = 22,                          // BIT_NOT
        S_SEMICOLON = 23,                        // SEMICOLON
        S_LEFT_PAREN = 24,                       // LEFT_PAREN
        S_RIGHT_PAREN = 25,                      // RIGHT_PAREN
        S_LEFT_BRACKET = 26,                     // LEFT_BRACKET
        S_RIGHT_BRACKET = 27,                    // RIGHT_BRACKET
        S_OPENING_BRACE = 28,                    // OPENING_BRACE
        S_CLOSING_BRACE = 29,                    // CLOSING_BRACE
        S_COMMA = 30,                            // COMMA
        S_LEFT_SHIFT = 31,                       // LEFT_SHIFT
        S_RIGHT_SHIFT = 32,                      // RIGHT_SHIFT
        S_BIT_AND = 33,                          // BIT_AND
        S_BIT_EX_OR = 34,                        // BIT_EX_OR
        S_BIT_OR = 35,                           // BIT_OR
        S_LOGIC_AND = 36,                        // LOGIC_AND
        S_LOGIC_OR = 37,                         // LOGIC_OR
        S_QUESTION = 38,                         // QUESTION
        S_COLON = 39,                            // COLON
        S_LT = 40,                               // LT
        S_LT_EQ = 41,                            // LT_EQ
        S_GT = 42,                               // GT
        S_GT_EQ = 43,                            // GT_EQ
        S_EQ = 44,                               // EQ
        S_NOT_EQ = 45,                           // NOT_EQ
        S_PRINT = 46,                            // PRINT
        S_READ = 47,                             // READ
        S_FLUSH = 48,                            // FLUSH
        S_ALLOC = 49,                            // ALLOC
        S_ALLOC_ARRAY = 50,                      // ALLOC_ARRAY
        S_STRUCT = 51,                           // STRUCT
        S_ARROW = 52,                            // ARROW
        S_DOT = 53,                              // DOT
        S_IF = 54,                               // IF
        S_ELSE = 55,                             // ELSE
        S_WHILE = 56,                            // WHILE
        S_FOR = 57,                              // FOR
        S_CONTINUE = 58,                         // CONTINUE
        S_BREAK = 59,                            // BREAK
        S_RETURN = 60,                           // RETURN
        S_ASSERT = 61,                           // ASSERT
        S_TRUE = 62,                             // TRUE
        S_FALSE = 63,                            // FALSE
        S_KW_NULL = 64,                          // KW_NULL
        S_STRING = 65,                           // STRING
        S_BOOLEAN = 66,                          // BOOLEAN
        S_VOID = 67,                             // VOID
        S_CHAR = 68,                             // CHAR
        S_INT = 69,                              // INT
        S_NONE = 70,                             // NONE
        S_IFX = 71,                              // IFX
        S_CON_EXP = 72,                          // CON_EXP
        S_UMINUS = 73,                           // UMINUS
        S_DEREF = 74,                            // DEREF
        S_YYACCEPT = 75,                         // $accept
        S_program = 76,                          // program
        S_root_declarations = 77,                // root_declarations
        S_root_declaration = 78,                 // root_declaration
        S_function = 79,                         // function
        S_80_1 = 80,                             // $@1
        S_struct_def = 81,                       // struct_def
        S_field_list = 82,                       // field_list
        S_field = 83,                            // field
        S_param_list = 84,                       // param_list
        S_param_list_follow = 85,                // param_list_follow
        S_param = 86,                            // param
        S_block = 87,                            // block
        S_88_2 = 88,                             // $@2
        S_type = 89,                             // type
        S_statements = 90,                       // statements
        S_statement = 91,                        // statement
        S_simple_statement = 92,                 // simple_statement
        S_simple_optional = 93,                  // simple_optional
        S_declarator = 94,                       // declarator
        S_left_value = 95,                       // left_value
        S_control = 96,                          // control
        S_97_3 = 97,                             // $@3
        S_expression = 98,                       // expression
        S_assign_operator = 99,                  // assign_operator
        S_call = 100,                            // call
        S_arg_list = 101,                        // arg_list
        S_arg_list_follow = 102                  // arg_list_follow
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value ()
        , location (std::move (that.location))
      {
        switch (this->kind ())
    {
      case symbol_kind::S_program: // program
      case symbol_kind::S_root_declaration: // root_declaration
      case symbol_kind::S_function: // function
      case symbol_kind::S_struct_def: // struct_def
      case symbol_kind::S_field: // field
      case symbol_kind::S_param_list: // param_list
      case symbol_kind::S_param: // param
      case symbol_kind::S_block: // block
      case symbol_kind::S_statement: // statement
      case symbol_kind::S_simple_statement: // simple_statement
      case symbol_kind::S_simple_optional: // simple_optional
      case symbol_kind::S_declarator: // declarator
      case symbol_kind::S_left_value: // left_value
      case symbol_kind::S_control: // control
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_call: // call
      case symbol_kind::S_arg_list: // arg_list
        value.move< ASTNodePtr > (std::move (that.value));
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        value.move< std::string > (std::move (that.value));
        break;

      case symbol_kind::S_root_declarations: // root_declarations
      case symbol_kind::S_field_list: // field_list
      case symbol_kind::S_param_list_follow: // param_list_follow
      case symbol_kind::S_statements: // statements
      case symbol_kind::S_arg_list_follow: // arg_list_follow
        value.move< std::vector<ASTNodePtr> > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ASTNodePtr&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ASTNodePtr& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::string&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::string& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::vector<ASTNodePtr>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::vector<ASTNodePtr>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
    {
      case symbol_kind::S_program: // program
      case symbol_kind::S_root_declaration: // root_declaration
      case symbol_kind::S_function: // function
      case symbol_kind::S_struct_def: // struct_def
      case symbol_kind::S_field: // field
      case symbol_kind::S_param_list: // param_list
      case symbol_kind::S_param: // param
      case symbol_kind::S_block: // block
      case symbol_kind::S_statement: // statement
      case symbol_kind::S_simple_statement: // simple_statement
      case symbol_kind::S_simple_optional: // simple_optional
      case symbol_kind::S_declarator: // declarator
      case symbol_kind::S_left_value: // left_value
      case symbol_kind::S_control: // control
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_call: // call
      case symbol_kind::S_arg_list: // arg_list
        value.template destroy< ASTNodePtr > ();
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        value.template destroy< std::string > ();
        break;

      case symbol_kind::S_root_declarations: // root_declarations
      case symbol_kind::S_field_list: // field_list
      case symbol_kind::S_param_list_follow: // param_list_follow
      case symbol_kind::S_statements: // statements
      case symbol_kind::S_arg_list_follow: // arg_list_follow
        value.template destroy< std::vector<ASTNodePtr> > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

      /// The user-facing name of this symbol.
      std::string name () const YY_NOEXCEPT
      {
        return parser::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, std::string v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const std::string& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
    };

    /// Build a parser object.
    parser (Driver& drv_yyarg);
    virtual ~parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    parser (const parser&) = delete;
    /// Non copyable.
    parser& operator= (const parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static std::string symbol_name (symbol_kind_type yysymbol);

    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYEOF (location_type l)
      {
        return symbol_type (token::TOKEN_YYEOF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYEOF (const location_type& l)
      {
        return symbol_type (token::TOKEN_YYEOF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYerror (location_type l)
      {
        return symbol_type (token::TOKEN_YYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_YYerror (const location_type& l)
      {
        return symbol_type (token::TOKEN_YYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYUNDEF (location_type l)
      {
        return symbol_type (token::TOKEN_YYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYUNDEF (const location_type& l)
      {
        return symbol_type (token::TOKEN_YYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VALUE_ID (std::string v, location_type l)
      {
        return symbol_type (token::TOKEN_VALUE_ID, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_VALUE_ID (const std::string& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_VALUE_ID, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VALUE_INTEGER (std::string v, location_type l)
      {
        return symbol_type (token::TOKEN_VALUE_INTEGER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_VALUE_INTEGER (const std::string& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_VALUE_INTEGER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ASSIGN (location_type l)
      {
        return symbol_type (token::TOKEN_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOKEN_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PLUS_ASSIGN (location_type l)
      {
        return symbol_type (token::TOKEN_PLUS_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_PLUS_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOKEN_PLUS_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SUB_ASSIGN (location_type l)
      {
        return symbol_type (token::TOKEN_SUB_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_SUB_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOKEN_SUB_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MULT_ASSIGN (location_type l)
      {
        return symbol_type (token::TOKEN_MULT_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_MULT_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOKEN_MULT_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SLASH_ASSIGN (location_type l)
      {
        return symbol_type (token::TOKEN_SLASH_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_SLASH_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOKEN_SLASH_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PERCENT_ASSIGN (location_type l)
      {
        return symbol_type (token::TOKEN_PERCENT_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_PERCENT_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOKEN_PERCENT_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BIT_AND_ASSIGN (location_type l)
      {
        return symbol_type (token::TOKEN_BIT_AND_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_BIT_AND_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOKEN_BIT_AND_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BIT_EX_OR_ASSIGN (location_type l)
      {
        return symbol_type (token::TOKEN_BIT_EX_OR_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_BIT_EX_OR_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOKEN_BIT_EX_OR_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BIT_OR_ASSIGN (location_type l)
      {
        return symbol_type (token::TOKEN_BIT_OR_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_BIT_OR_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOKEN_BIT_OR_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LEFT_SHIFT_ASSIGN (location_type l)
      {
        return symbol_type (token::TOKEN_LEFT_SHIFT_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_LEFT_SHIFT_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOKEN_LEFT_SHIFT_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RIGHT_SHIFT_ASSIGN (location_type l)
      {
        return symbol_type (token::TOKEN_RIGHT_SHIFT_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_RIGHT_SHIFT_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOKEN_RIGHT_SHIFT_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PLUS (location_type l)
      {
        return symbol_type (token::TOKEN_PLUS, std::move (l));
      }
#else
      static
      symbol_type
      make_PLUS (const location_type& l)
      {
        return symbol_type (token::TOKEN_PLUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SUB (location_type l)
      {
        return symbol_type (token::TOKEN_SUB, std::move (l));
      }
#else
      static
      symbol_type
      make_SUB (const location_type& l)
      {
        return symbol_type (token::TOKEN_SUB, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MULT (location_type l)
      {
        return symbol_type (token::TOKEN_MULT, std::move (l));
      }
#else
      static
      symbol_type
      make_MULT (const location_type& l)
      {
        return symbol_type (token::TOKEN_MULT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SLASH (location_type l)
      {
        return symbol_type (token::TOKEN_SLASH, std::move (l));
      }
#else
      static
      symbol_type
      make_SLASH (const location_type& l)
      {
        return symbol_type (token::TOKEN_SLASH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PERCENT (location_type l)
      {
        return symbol_type (token::TOKEN_PERCENT, std::move (l));
      }
#else
      static
      symbol_type
      make_PERCENT (const location_type& l)
      {
        return symbol_type (token::TOKEN_PERCENT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LOGIC_NOT (location_type l)
      {
        return symbol_type (token::TOKEN_LOGIC_NOT, std::move (l));
      }
#else
      static
      symbol_type
      make_LOGIC_NOT (const location_type& l)
      {
        return symbol_type (token::TOKEN_LOGIC_NOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BIT_NOT (location_type l)
      {
        return symbol_type (token::TOKEN_BIT_NOT, std::move (l));
      }
#else
      static
      symbol_type
      make_BIT_NOT (const location_type& l)
      {
        return symbol_type (token::TOKEN_BIT_NOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SEMICOLON (location_type l)
      {
        return symbol_type (token::TOKEN_SEMICOLON, std::move (l));
      }
#else
      static
      symbol_type
      make_SEMICOLON (const location_type& l)
      {
        return symbol_type (token::TOKEN_SEMICOLON, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LEFT_PAREN (location_type l)
      {
        return symbol_type (token::TOKEN_LEFT_PAREN, std::move (l));
      }
#else
      static
      symbol_type
      make_LEFT_PAREN (const location_type& l)
      {
        return symbol_type (token::TOKEN_LEFT_PAREN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RIGHT_PAREN (location_type l)
      {
        return symbol_type (token::TOKEN_RIGHT_PAREN, std::move (l));
      }
#else
      static
      symbol_type
      make_RIGHT_PAREN (const location_type& l)
      {
        return symbol_type (token::TOKEN_RIGHT_PAREN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LEFT_BRACKET (location_type l)
      {
        return symbol_type (token::TOKEN_LEFT_BRACKET, std::move (l));
      }
#else
      static
      symbol_type
      make_LEFT_BRACKET (const location_type& l)
      {
        return symbol_type (token::TOKEN_LEFT_BRACKET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RIGHT_BRACKET (location_type l)
      {
        return symbol_type (token::TOKEN_RIGHT_BRACKET, std::move (l));
      }
#else
      static
      symbol_type
      make_RIGHT_BRACKET (const location_type& l)
      {
        return symbol_type (token::TOKEN_RIGHT_BRACKET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OPENING_BRACE (location_type l)
      {
        return symbol_type (token::TOKEN_OPENING_BRACE, std::move (l));
      }
#else
      static
      symbol_type
      make_OPENING_BRACE (const location_type& l)
      {
        return symbol_type (token::TOKEN_OPENING_BRACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLOSING_BRACE (location_type l)
      {
        return symbol_type (token::TOKEN_CLOSING_BRACE, std::move (l));
      }
#else
      static
      symbol_type
      make_CLOSING_BRACE (const location_type& l)
      {
        return symbol_type (token::TOKEN_CLOSING_BRACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COMMA (location_type l)
      {
        return symbol_type (token::TOKEN_COMMA, std::move (l));
      }
#else
      static
      symbol_type
      make_COMMA (const location_type& l)
      {
        return symbol_type (token::TOKEN_COMMA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LEFT_SHIFT (location_type l)
      {
        return symbol_type (token::TOKEN_LEFT_SHIFT, std::move (l));
      }
#else
      static
      symbol_type
      make_LEFT_SHIFT (const location_type& l)
      {
        return symbol_type (token::TOKEN_LEFT_SHIFT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RIGHT_SHIFT (location_type l)
      {
        return symbol_type (token::TOKEN_RIGHT_SHIFT, std::move (l));
      }
#else
      static
      symbol_type
      make_RIGHT_SHIFT (const location_type& l)
      {
        return symbol_type (token::TOKEN_RIGHT_SHIFT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BIT_AND (location_type l)
      {
        return symbol_type (token::TOKEN_BIT_AND, std::move (l));
      }
#else
      static
      symbol_type
      make_BIT_AND (const location_type& l)
      {
        return symbol_type (token::TOKEN_BIT_AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BIT_EX_OR (location_type l)
      {
        return symbol_type (token::TOKEN_BIT_EX_OR, std::move (l));
      }
#else
      static
      symbol_type
      make_BIT_EX_OR (const location_type& l)
      {
        return symbol_type (token::TOKEN_BIT_EX_OR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BIT_OR (location_type l)
      {
        return symbol_type (token::TOKEN_BIT_OR, std::move (l));
      }
#else
      static
      symbol_type
      make_BIT_OR (const location_type& l)
      {
        return symbol_type (token::TOKEN_BIT_OR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LOGIC_AND (location_type l)
      {
        return symbol_type (token::TOKEN_LOGIC_AND, std::move (l));
      }
#else
      static
      symbol_type
      make_LOGIC_AND (const location_type& l)
      {
        return symbol_type (token::TOKEN_LOGIC_AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LOGIC_OR (location_type l)
      {
        return symbol_type (token::TOKEN_LOGIC_OR, std::move (l));
      }
#else
      static
      symbol_type
      make_LOGIC_OR (const location_type& l)
      {
        return symbol_type (token::TOKEN_LOGIC_OR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_QUESTION (location_type l)
      {
        return symbol_type (token::TOKEN_QUESTION, std::move (l));
      }
#else
      static
      symbol_type
      make_QUESTION (const location_type& l)
      {
        return symbol_type (token::TOKEN_QUESTION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COLON (location_type l)
      {
        return symbol_type (token::TOKEN_COLON, std::move (l));
      }
#else
      static
      symbol_type
      make_COLON (const location_type& l)
      {
        return symbol_type (token::TOKEN_COLON, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LT (location_type l)
      {
        return symbol_type (token::TOKEN_LT, std::move (l));
      }
#else
      static
      symbol_type
      make_LT (const location_type& l)
      {
        return symbol_type (token::TOKEN_LT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LT_EQ (location_type l)
      {
        return symbol_type (token::TOKEN_LT_EQ, std::move (l));
      }
#else
      static
      symbol_type
      make_LT_EQ (const location_type& l)
      {
        return symbol_type (token::TOKEN_LT_EQ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GT (location_type l)
      {
        return symbol_type (token::TOKEN_GT, std::move (l));
      }
#else
      static
      symbol_type
      make_GT (const location_type& l)
      {
        return symbol_type (token::TOKEN_GT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GT_EQ (location_type l)
      {
        return symbol_type (token::TOKEN_GT_EQ, std::move (l));
      }
#else
      static
      symbol_type
      make_GT_EQ (const location_type& l)
      {
        return symbol_type (token::TOKEN_GT_EQ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EQ (location_type l)
      {
        return symbol_type (token::TOKEN_EQ, std::move (l));
      }
#else
      static
      symbol_type
      make_EQ (const location_type& l)
      {
        return symbol_type (token::TOKEN_EQ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NOT_EQ (location_type l)
      {
        return symbol_type (token::TOKEN_NOT_EQ, std::move (l));
      }
#else
      static
      symbol_type
      make_NOT_EQ (const location_type& l)
      {
        return symbol_type (token::TOKEN_NOT_EQ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PRINT (location_type l)
      {
        return symbol_type (token::TOKEN_PRINT, std::move (l));
      }
#else
      static
      symbol_type
      make_PRINT (const location_type& l)
      {
        return symbol_type (token::TOKEN_PRINT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_READ (location_type l)
      {
        return symbol_type (token::TOKEN_READ, std::move (l));
      }
#else
      static
      symbol_type
      make_READ (const location_type& l)
      {
        return symbol_type (token::TOKEN_READ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FLUSH (location_type l)
      {
        return symbol_type (token::TOKEN_FLUSH, std::move (l));
      }
#else
      static
      symbol_type
      make_FLUSH (const location_type& l)
      {
        return symbol_type (token::TOKEN_FLUSH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ALLOC (location_type l)
      {
        return symbol_type (token::TOKEN_ALLOC, std::move (l));
      }
#else
      static
      symbol_type
      make_ALLOC (const location_type& l)
      {
        return symbol_type (token::TOKEN_ALLOC, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ALLOC_ARRAY (location_type l)
      {
        return symbol_type (token::TOKEN_ALLOC_ARRAY, std::move (l));
      }
#else
      static
      symbol_type
      make_ALLOC_ARRAY (const location_type& l)
      {
        return symbol_type (token::TOKEN_ALLOC_ARRAY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRUCT (location_type l)
      {
        return symbol_type (token::TOKEN_STRUCT, std::move (l));
      }
#else
      static
      symbol_type
      make_STRUCT (const location_type& l)
      {
        return symbol_type (token::TOKEN_STRUCT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ARROW (location_type l)
      {
        return symbol_type (token::TOKEN_ARROW, std::move (l));
      }
#else
      static
      symbol_type
      make_ARROW (const location_type& l)
      {
        return symbol_type (token::TOKEN_ARROW, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DOT (location_type l)
      {
        return symbol_type (token::TOKEN_DOT, std::move (l));
      }
#else
      static
      symbol_type
      make_DOT (const location_type& l)
      {
        return symbol_type (token::TOKEN_DOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IF (location_type l)
      {
        return symbol_type (token::TOKEN_IF, std::move (l));
      }
#else
      static
      symbol_type
      make_IF (const location_type& l)
      {
        return symbol_type (token::TOKEN_IF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ELSE (location_type l)
      {
        return symbol_type (token::TOKEN_ELSE, std::move (l));
      }
#else
      static
      symbol_type
      make_ELSE (const location_type& l)
      {
        return symbol_type (token::TOKEN_ELSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_WHILE (location_type l)
      {
        return symbol_type (token::TOKEN_WHILE, std::move (l));
      }
#else
      static
      symbol_type
      make_WHILE (const location_type& l)
      {
        return symbol_type (token::TOKEN_WHILE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FOR (location_type l)
      {
        return symbol_type (token::TOKEN_FOR, std::move (l));
      }
#else
      static
      symbol_type
      make_FOR (const location_type& l)
      {
        return symbol_type (token::TOKEN_FOR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONTINUE (location_type l)
      {
        return symbol_type (token::TOKEN_CONTINUE, std::move (l));
      }
#else
      static
      symbol_type
      make_CONTINUE (const location_type& l)
      {
        return symbol_type (token::TOKEN_CONTINUE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BREAK (location_type l)
      {
        return symbol_type (token::TOKEN_BREAK, std::move (l));
      }
#else
      static
      symbol_type
      make_BREAK (const location_type& l)
      {
        return symbol_type (token::TOKEN_BREAK, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RETURN (location_type l)
      {
        return symbol_type (token::TOKEN_RETURN, std::move (l));
      }
#else
      static
      symbol_type
      make_RETURN (const location_type& l)
      {
        return symbol_type (token::TOKEN_RETURN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ASSERT (location_type l)
      {
        return symbol_type (token::TOKEN_ASSERT, std::move (l));
      }
#else
      static
      symbol_type
      make_ASSERT (const location_type& l)
      {
        return symbol_type (token::TOKEN_ASSERT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TRUE (location_type l)
      {
        return symbol_type (token::TOKEN_TRUE, std::move (l));
      }
#else
      static
      symbol_type
      make_TRUE (const location_type& l)
      {
        return symbol_type (token::TOKEN_TRUE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FALSE (location_type l)
      {
        return symbol_type (token::TOKEN_FALSE, std::move (l));
      }
#else
      static
      symbol_type
      make_FALSE (const location_type& l)
      {
        return symbol_type (token::TOKEN_FALSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_KW_NULL (location_type l)
      {
        return symbol_type (token::TOKEN_KW_NULL, std::move (l));
      }
#else
      static
      symbol_type
      make_KW_NULL (const location_type& l)
      {
        return symbol_type (token::TOKEN_KW_NULL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRING (location_type l)
      {
        return symbol_type (token::TOKEN_STRING, std::move (l));
      }
#else
      static
      symbol_type
      make_STRING (const location_type& l)
      {
        return symbol_type (token::TOKEN_STRING, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BOOLEAN (location_type l)
      {
        return symbol_type (token::TOKEN_BOOLEAN, std::move (l));
      }
#else
      static
      symbol_type
      make_BOOLEAN (const location_type& l)
      {
        return symbol_type (token::TOKEN_BOOLEAN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VOID (location_type l)
      {
        return symbol_type (token::TOKEN_VOID, std::move (l));
      }
#else
      static
      symbol_type
      make_VOID (const location_type& l)
      {
        return symbol_type (token::TOKEN_VOID, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CHAR (location_type l)
      {
        return symbol_type (token::TOKEN_CHAR, std::move (l));
      }
#else
      static
      symbol_type
      make_CHAR (const location_type& l)
      {
        return symbol_type (token::TOKEN_CHAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INT (location_type l)
      {
        return symbol_type (token::TOKEN_INT, std::move (l));
      }
#else
      static
      symbol_type
      make_INT (const location_type& l)
      {
        return symbol_type (token::TOKEN_INT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NONE (location_type l)
      {
        return symbol_type (token::TOKEN_NONE, std::move (l));
      }
#else
      static
      symbol_type
      make_NONE (const location_type& l)
      {
        return symbol_type (token::TOKEN_NONE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IFX (location_type l)
      {
        return symbol_type (token::TOKEN_IFX, std::move (l));
      }
#else
      static
      symbol_type
      make_IFX (const location_type& l)
      {
        return symbol_type (token::TOKEN_IFX, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CON_EXP (location_type l)
      {
        return symbol_type (token::TOKEN_CON_EXP, std::move (l));
      }
#else
      static
      symbol_type
      make_CON_EXP (const location_type& l)
      {
        return symbol_type (token::TOKEN_CON_EXP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UMINUS (location_type l)
      {
        return symbol_type (token::TOKEN_UMINUS, std::move (l));
      }
#else
      static
      symbol_type
      make_UMINUS (const location_type& l)
      {
        return symbol_type (token::TOKEN_UMINUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DEREF (location_type l)
      {
        return symbol_type (token::TOKEN_DEREF, std::move (l));
      }
#else
      static
      symbol_type
      make_DEREF (const location_type& l)
      {
        return symbol_type (token::TOKEN_DEREF, l);
      }
#endif


    class context
    {
    public:
      context (const parser& yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const YY_NOEXCEPT { return yyla_; }
      symbol_kind_type token () const YY_NOEXCEPT { return yyla_.kind (); }
      const location_type& location () const YY_NOEXCEPT { return yyla_.location; }

      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const parser& yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    parser (const parser&);
    /// Non copyable.
    parser& operator= (const parser&);
#endif


    /// Stored state numbers (used for stacks).
    typedef unsigned char state_type;

    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;

    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const short yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;

    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *yystr);

    /// For a symbol, its name in clear.
    static const char* const yytname_[];


    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const signed char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const unsigned char yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const unsigned char yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const signed char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const signed char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 658,     ///< Last index in yytable_.
      yynnts_ = 28,  ///< Number of nonterminal symbols.
      yyfinal_ = 11 ///< Termination state number.
    };


    // User arguments.
    Driver& drv;

  };

  inline
  parser::symbol_kind_type
  parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74
    };
    // Last valid token kind.
    const int code_max = 329;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

  // basic_symbol.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
    {
      case symbol_kind::S_program: // program
      case symbol_kind::S_root_declaration: // root_declaration
      case symbol_kind::S_function: // function
      case symbol_kind::S_struct_def: // struct_def
      case symbol_kind::S_field: // field
      case symbol_kind::S_param_list: // param_list
      case symbol_kind::S_param: // param
      case symbol_kind::S_block: // block
      case symbol_kind::S_statement: // statement
      case symbol_kind::S_simple_statement: // simple_statement
      case symbol_kind::S_simple_optional: // simple_optional
      case symbol_kind::S_declarator: // declarator
      case symbol_kind::S_left_value: // left_value
      case symbol_kind::S_control: // control
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_call: // call
      case symbol_kind::S_arg_list: // arg_list
        value.copy< ASTNodePtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        value.copy< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_root_declarations: // root_declarations
      case symbol_kind::S_field_list: // field_list
      case symbol_kind::S_param_list_follow: // param_list_follow
      case symbol_kind::S_statements: // statements
      case symbol_kind::S_arg_list_follow: // arg_list_follow
        value.copy< std::vector<ASTNodePtr> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  parser::symbol_kind_type
  parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_program: // program
      case symbol_kind::S_root_declaration: // root_declaration
      case symbol_kind::S_function: // function
      case symbol_kind::S_struct_def: // struct_def
      case symbol_kind::S_field: // field
      case symbol_kind::S_param_list: // param_list
      case symbol_kind::S_param: // param
      case symbol_kind::S_block: // block
      case symbol_kind::S_statement: // statement
      case symbol_kind::S_simple_statement: // simple_statement
      case symbol_kind::S_simple_optional: // simple_optional
      case symbol_kind::S_declarator: // declarator
      case symbol_kind::S_left_value: // left_value
      case symbol_kind::S_control: // control
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_call: // call
      case symbol_kind::S_arg_list: // arg_list
        value.move< ASTNodePtr > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        value.move< std::string > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_root_declarations: // root_declarations
      case symbol_kind::S_field_list: // field_list
      case symbol_kind::S_param_list_follow: // param_list_follow
      case symbol_kind::S_statements: // statements
      case symbol_kind::S_arg_list_follow: // arg_list_follow
        value.move< std::vector<ASTNodePtr> > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  inline
  parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  inline
  parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  inline
  parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  inline
  parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  inline
  void
  parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  inline
  void
  parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  inline
  parser::symbol_kind_type
  parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  inline
  parser::symbol_kind_type
  parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


} // yy
#line 2702 "Parser.hpp"




#endif // !YY_YY_PARSER_HPP_INCLUDED
