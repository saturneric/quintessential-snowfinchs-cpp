// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

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

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.



// First part of user prologue.
#line 22 "Parser.y"

#include <memory>

#include "Driver.hpp"

#line 47 "Parser.cpp"


#include "Parser.hpp"


// Unqualified %code blocks.
#line 18 "Parser.y"

    #define yylex drv.yylex

#line 58 "Parser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 150 "Parser.cpp"

  /// Build a parser object.
  parser::parser (Driver& drv_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      drv (drv_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_program: // program
      case symbol_kind::S_statement: // statement
      case symbol_kind::S_simple_statement: // simple_statement
      case symbol_kind::S_return_statement: // return_statement
      case symbol_kind::S_declarator: // declarator
      case symbol_kind::S_left_value: // left_value
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_primary: // primary
        value.YY_MOVE_OR_COPY< ASTNodePtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_assign_operator: // assign_operator
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_function_body: // function_body
      case symbol_kind::S_statements: // statements
        value.YY_MOVE_OR_COPY< std::vector<ASTNodePtr> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_program: // program
      case symbol_kind::S_statement: // statement
      case symbol_kind::S_simple_statement: // simple_statement
      case symbol_kind::S_return_statement: // return_statement
      case symbol_kind::S_declarator: // declarator
      case symbol_kind::S_left_value: // left_value
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_primary: // primary
        value.move< ASTNodePtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_assign_operator: // assign_operator
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_function_body: // function_body
      case symbol_kind::S_statements: // statements
        value.move< std::vector<ASTNodePtr> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_program: // program
      case symbol_kind::S_statement: // statement
      case symbol_kind::S_simple_statement: // simple_statement
      case symbol_kind::S_return_statement: // return_statement
      case symbol_kind::S_declarator: // declarator
      case symbol_kind::S_left_value: // left_value
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_primary: // primary
        value.copy< ASTNodePtr > (that.value);
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_assign_operator: // assign_operator
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_function_body: // function_body
      case symbol_kind::S_statements: // statements
        value.copy< std::vector<ASTNodePtr> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_program: // program
      case symbol_kind::S_statement: // statement
      case symbol_kind::S_simple_statement: // simple_statement
      case symbol_kind::S_return_statement: // return_statement
      case symbol_kind::S_declarator: // declarator
      case symbol_kind::S_left_value: // left_value
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_primary: // primary
        value.move< ASTNodePtr > (that.value);
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_assign_operator: // assign_operator
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_function_body: // function_body
      case symbol_kind::S_statements: // statements
        value.move< std::vector<ASTNodePtr> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (drv));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_program: // program
      case symbol_kind::S_statement: // statement
      case symbol_kind::S_simple_statement: // simple_statement
      case symbol_kind::S_return_statement: // return_statement
      case symbol_kind::S_declarator: // declarator
      case symbol_kind::S_left_value: // left_value
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_additive: // additive
      case symbol_kind::S_multiplicative: // multiplicative
      case symbol_kind::S_unary: // unary
      case symbol_kind::S_primary: // primary
        yylhs.value.emplace< ASTNodePtr > ();
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_assign_operator: // assign_operator
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_function_body: // function_body
      case symbol_kind::S_statements: // statements
        yylhs.value.emplace< std::vector<ASTNodePtr> > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // program: INT VALUE_ID LEFT_BRACKET RIGHT_BRACKET function_body
#line 54 "Parser.y"
    {
      if (yystack_[3].value.as < std::string > () != "main") {
        YYERROR;
      }
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kPROGRAM, "program", yystack_[3].value.as < std::string > (), drv);
      drv.SetSyntaxTreeRoot(yylhs.value.as < ASTNodePtr > ());
      for (const auto& child : yystack_[0].value.as < std::vector<ASTNodePtr> > ()) {
        yylhs.value.as < ASTNodePtr > ()->AddChildren(child);
      }
    }
#line 678 "Parser.cpp"
    break;

  case 3: // function_body: OPENING_BRACE statements CLOSING_BRACE
#line 68 "Parser.y"
    {
      EnterScope(drv);
      yylhs.value.as < std::vector<ASTNodePtr> > () = yystack_[1].value.as < std::vector<ASTNodePtr> > ();
      LeaveScope(drv);
    }
#line 688 "Parser.cpp"
    break;

  case 4: // statements: %empty
#line 77 "Parser.y"
    {
      yylhs.value.as < std::vector<ASTNodePtr> > () = {};
    }
#line 696 "Parser.cpp"
    break;

  case 5: // statements: statement statements
#line 81 "Parser.y"
    {
      yystack_[0].value.as < std::vector<ASTNodePtr> > ().insert(yystack_[0].value.as < std::vector<ASTNodePtr> > ().begin(), yystack_[1].value.as < ASTNodePtr > ());
      yylhs.value.as < std::vector<ASTNodePtr> > () = yystack_[0].value.as < std::vector<ASTNodePtr> > ();
    }
#line 705 "Parser.cpp"
    break;

  case 6: // statement: declarator SEMICOLON
#line 89 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[1].value.as < ASTNodePtr > ();
    }
#line 713 "Parser.cpp"
    break;

  case 7: // statement: simple_statement SEMICOLON
#line 93 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[1].value.as < ASTNodePtr > ();
    }
#line 721 "Parser.cpp"
    break;

  case 8: // statement: return_statement SEMICOLON
#line 97 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[1].value.as < ASTNodePtr > ();
    }
#line 729 "Parser.cpp"
    break;

  case 9: // simple_statement: left_value assign_operator expression
#line 104 "Parser.y"
    {
      if (yystack_[1].value.as < std::string > () == "=") {
        yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kASSIGN, "simple_statement", yystack_[1].value.as < std::string > (), drv);
        yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[2].value.as < ASTNodePtr > ());
        yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[0].value.as < ASTNodePtr > ());
      } else {

        auto op = yystack_[1].value.as < std::string > ();
        if (op.size() >= 2 && op.back() == '=') {
            op = op.substr(0, 1);
        }

        auto bin_op = MakeASTTreeNode(ASTNodeType::kBIN_OP, "simple_statement", op, drv);
        bin_op->AddChildren(yystack_[2].value.as < ASTNodePtr > ());
        bin_op->AddChildren(yystack_[0].value.as < ASTNodePtr > ());

        yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kASSIGN, "simple_statement", "=", drv);
        yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[2].value.as < ASTNodePtr > ());
        yylhs.value.as < ASTNodePtr > ()->AddChildren(bin_op);
      }
    }
#line 755 "Parser.cpp"
    break;

  case 10: // return_statement: RETURN expression
#line 129 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kRETURN, "return_statement", "", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 764 "Parser.cpp"
    break;

  case 11: // declarator: INT VALUE_ID
#line 137 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kDECLARE, "declarator", yystack_[0].value.as < std::string > (), drv);
    }
#line 772 "Parser.cpp"
    break;

  case 12: // declarator: INT VALUE_ID EQUAL expression
#line 141 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kDECLARE, "declarator", yystack_[2].value.as < std::string > (), drv);
      auto assign_node = MakeASTTreeNode(ASTNodeType::kASSIGN, "declarator", "=", drv);
      
      assign_node->AddChildren(
        MakeASTTreeNode(ASTNodeType::kIDENT, "declarator", yystack_[2].value.as < std::string > (), drv));
      assign_node->AddChildren(yystack_[0].value.as < ASTNodePtr > ());  

      yylhs.value.as < ASTNodePtr > ()->AddChildren(assign_node);
    }
#line 787 "Parser.cpp"
    break;

  case 13: // left_value: VALUE_ID
#line 155 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kIDENT, "left_value", yystack_[0].value.as < std::string > (), drv);
    }
#line 795 "Parser.cpp"
    break;

  case 14: // left_value: LEFT_BRACKET left_value RIGHT_BRACKET
#line 159 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[1].value.as < ASTNodePtr > ();
    }
#line 803 "Parser.cpp"
    break;

  case 15: // expression: additive
#line 164 "Parser.y"
            { yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > (); }
#line 809 "Parser.cpp"
    break;

  case 16: // additive: additive PLUS multiplicative
#line 168 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "additive", "+", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[2].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 819 "Parser.cpp"
    break;

  case 17: // additive: additive SUB multiplicative
#line 174 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "additive", "-", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[2].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 829 "Parser.cpp"
    break;

  case 18: // additive: multiplicative
#line 180 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > ();
    }
#line 837 "Parser.cpp"
    break;

  case 19: // multiplicative: multiplicative MULT unary
#line 187 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "multiplicative", "*", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[2].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 847 "Parser.cpp"
    break;

  case 20: // multiplicative: multiplicative SLASH unary
#line 193 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "multiplicative", "/", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[2].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 857 "Parser.cpp"
    break;

  case 21: // multiplicative: multiplicative PERCENT unary
#line 199 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "multiplicative", "%", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[2].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 867 "Parser.cpp"
    break;

  case 22: // multiplicative: unary
#line 205 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > ();
    }
#line 875 "Parser.cpp"
    break;

  case 23: // unary: SUB unary
#line 212 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kUN_OP, "unary", "-", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChildren(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 884 "Parser.cpp"
    break;

  case 24: // unary: primary
#line 217 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > ();
    }
#line 892 "Parser.cpp"
    break;

  case 25: // primary: LEFT_BRACKET expression RIGHT_BRACKET
#line 224 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[1].value.as < ASTNodePtr > ();
    }
#line 900 "Parser.cpp"
    break;

  case 26: // primary: VALUE_INTEGER
#line 228 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kVALUE, "primary", yystack_[0].value.as < std::string > (), drv);
    }
#line 908 "Parser.cpp"
    break;

  case 27: // primary: VALUE_ID
#line 232 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kIDENT, "primary", yystack_[0].value.as < std::string > (), drv);
    }
#line 916 "Parser.cpp"
    break;

  case 28: // assign_operator: EQUAL
#line 238 "Parser.y"
                      { yylhs.value.as < std::string > () = "="; }
#line 922 "Parser.cpp"
    break;

  case 29: // assign_operator: PLUS_EQUAL
#line 239 "Parser.y"
                      { yylhs.value.as < std::string > () = "+="; }
#line 928 "Parser.cpp"
    break;

  case 30: // assign_operator: SUB_EQUAL
#line 240 "Parser.y"
                      { yylhs.value.as < std::string > () = "-="; }
#line 934 "Parser.cpp"
    break;

  case 31: // assign_operator: MULT_EQUAL
#line 241 "Parser.y"
                      { yylhs.value.as < std::string > () = "*="; }
#line 940 "Parser.cpp"
    break;

  case 32: // assign_operator: SLASH_EQUAL
#line 242 "Parser.y"
                      { yylhs.value.as < std::string > () = "/="; }
#line 946 "Parser.cpp"
    break;

  case 33: // assign_operator: PERCENT_EQUAL
#line 243 "Parser.y"
                      { yylhs.value.as < std::string > () = "%="; }
#line 952 "Parser.cpp"
    break;


#line 956 "Parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // parser::context.
  parser::context::context (const parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char parser::yypact_ninf_ = -43;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
     -42,    12,    17,    14,   -43,     7,    10,    -3,   -43,   -43,
      -1,     1,    32,    16,    -3,    21,    22,    23,     2,    24,
     -43,   -43,     1,     1,   -43,     8,     9,   -43,   -43,    35,
     -43,   -43,   -43,   -43,   -43,   -43,   -43,   -43,   -43,   -43,
     -43,     1,   -43,   -43,    25,     1,     1,     1,     1,     1,
       1,   -43,   -43,     9,     9,   -43,   -43,   -43,   -43
  };

  const signed char
  parser::yydefact_[] =
  {
       0,     0,     0,     0,     1,     0,     0,     4,     2,    13,
       0,     0,     0,     0,     4,     0,     0,     0,     0,     0,
      27,    26,     0,     0,    10,    15,    18,    22,    24,    11,
       3,     5,     7,     8,     6,    28,    29,    30,    31,    32,
      33,     0,    14,    23,     0,     0,     0,     0,     0,     0,
       0,     9,    25,    16,    17,    19,    20,    21,    12
  };

  const signed char
  parser::yypgoto_[] =
  {
     -43,   -43,   -43,    27,   -43,   -43,   -43,   -43,    34,   -20,
     -43,   -12,   -21,   -43,   -43
  };

  const signed char
  parser::yydefgoto_[] =
  {
       0,     2,     8,    13,    14,    15,    16,    17,    18,    24,
      25,    26,    27,    28,    41
  };

  const signed char
  parser::yytable_[] =
  {
       9,    43,     9,    44,    20,    21,     1,    35,    36,    37,
      38,    39,    40,    22,    10,     3,    10,     4,    23,    45,
      46,    51,    47,    48,    49,     6,    55,    56,    57,     7,
      58,     5,    11,    53,    54,    29,    30,    32,    33,    34,
      50,    31,    42,    52,    19,    12
  };

  const signed char
  parser::yycheck_[] =
  {
       3,    22,     3,    23,     3,     4,    48,     5,     6,     7,
       8,     9,    10,    12,    17,     3,    17,     0,    17,    11,
      12,    41,    13,    14,    15,    18,    47,    48,    49,    19,
      50,    17,    35,    45,    46,     3,    20,    16,    16,    16,
       5,    14,    18,    18,    10,    48
  };

  const signed char
  parser::yystos_[] =
  {
       0,    48,    52,     3,     0,    17,    18,    19,    53,     3,
      17,    35,    48,    54,    55,    56,    57,    58,    59,    59,
       3,     4,    12,    17,    60,    61,    62,    63,    64,     3,
      20,    54,    16,    16,    16,     5,     6,     7,     8,     9,
      10,    65,    18,    63,    60,    11,    12,    13,    14,    15,
       5,    60,    18,    62,    62,    63,    63,    63,    60
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    51,    52,    53,    54,    54,    55,    55,    55,    56,
      57,    58,    58,    59,    59,    60,    61,    61,    61,    62,
      62,    62,    62,    63,    63,    64,    64,    64,    65,    65,
      65,    65,    65,    65
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     5,     3,     0,     2,     2,     2,     2,     3,
       2,     2,     4,     1,     3,     1,     3,     3,     1,     3,
       3,     3,     1,     2,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "VALUE_ID",
  "VALUE_INTEGER", "EQUAL", "PLUS_EQUAL", "SUB_EQUAL", "MULT_EQUAL",
  "SLASH_EQUAL", "PERCENT_EQUAL", "PLUS", "SUB", "MULT", "SLASH",
  "PERCENT", "SEMICOLON", "LEFT_BRACKET", "RIGHT_BRACKET", "OPENING_BRACE",
  "CLOSING_BRACE", "LEFT_SHIFT", "RIGHT_SHIFT", "AND", "INSERT",
  "DELIMITER", "COMMA", "TILDE", "STRUCT", "IF", "ELSE", "WHILE", "FOR",
  "CONTINUE", "BREAK", "RETURN", "ASSERT", "PRINT", "READ", "ALLOC",
  "ALLOC_ARRAY", "TRUE", "FALSE", "KW_NULL", "STRING", "BOOLEAN", "VOID",
  "CHAR", "INT", "NONE", "UMINUS", "$accept", "program", "function_body",
  "statements", "statement", "simple_statement", "return_statement",
  "declarator", "left_value", "expression", "additive", "multiplicative",
  "unary", "primary", "assign_operator", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const unsigned char
  parser::yyrline_[] =
  {
       0,    53,    53,    67,    77,    80,    88,    92,    96,   103,
     128,   136,   140,   154,   158,   164,   167,   173,   179,   186,
     192,   198,   204,   211,   216,   223,   227,   231,   238,   239,
     240,   241,   242,   243
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


} // yy
#line 1459 "Parser.cpp"

#line 246 "Parser.y"

