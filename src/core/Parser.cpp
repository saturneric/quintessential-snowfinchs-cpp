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

  #include "Driver.hpp"

#line 45 "Parser.cpp"


#include "Parser.hpp"


// Unqualified %code blocks.
#line 18 "Parser.y"

    #define yylex drv.yylex

#line 56 "Parser.cpp"


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
#line 148 "Parser.cpp"

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
      case symbol_kind::S_function: // function
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
        value.YY_MOVE_OR_COPY< ASTNodePtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_functions: // functions
      case symbol_kind::S_param_list_follow: // param_list_follow
      case symbol_kind::S_statements: // statements
      case symbol_kind::S_arg_list_follow: // arg_list_follow
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
      case symbol_kind::S_function: // function
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
        value.move< ASTNodePtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_functions: // functions
      case symbol_kind::S_param_list_follow: // param_list_follow
      case symbol_kind::S_statements: // statements
      case symbol_kind::S_arg_list_follow: // arg_list_follow
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
      case symbol_kind::S_function: // function
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
        value.copy< ASTNodePtr > (that.value);
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_functions: // functions
      case symbol_kind::S_param_list_follow: // param_list_follow
      case symbol_kind::S_statements: // statements
      case symbol_kind::S_arg_list_follow: // arg_list_follow
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
      case symbol_kind::S_function: // function
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
        value.move< ASTNodePtr > (that.value);
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_functions: // functions
      case symbol_kind::S_param_list_follow: // param_list_follow
      case symbol_kind::S_statements: // statements
      case symbol_kind::S_arg_list_follow: // arg_list_follow
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
      case symbol_kind::S_function: // function
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
        yylhs.value.emplace< ASTNodePtr > ();
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_functions: // functions
      case symbol_kind::S_param_list_follow: // param_list_follow
      case symbol_kind::S_statements: // statements
      case symbol_kind::S_arg_list_follow: // arg_list_follow
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
  case 2: // program: functions
#line 93 "Parser.y"
  { 
    yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kPROGRAM, "program", {}, drv);
    drv.SetSyntaxTreeRoot(yylhs.value.as < ASTNodePtr > ());
    for (auto p : yystack_[0].value.as < std::vector<ASTNodePtr> > ()) yylhs.value.as < ASTNodePtr > ()->AddChild(p);
  }
#line 701 "Parser.cpp"
    break;

  case 3: // functions: %empty
#line 102 "Parser.y"
    { yylhs.value.as < std::vector<ASTNodePtr> > () = std::vector<ASTNodePtr>(); }
#line 707 "Parser.cpp"
    break;

  case 4: // functions: function functions
#line 104 "Parser.y"
    {
      yystack_[0].value.as < std::vector<ASTNodePtr> > ().insert(yystack_[0].value.as < std::vector<ASTNodePtr> > ().begin(), yystack_[1].value.as < ASTNodePtr > ());
      yylhs.value.as < std::vector<ASTNodePtr> > () = yystack_[0].value.as < std::vector<ASTNodePtr> > ();
    }
#line 716 "Parser.cpp"
    break;

  case 5: // $@1: %empty
#line 111 "Parser.y"
                  { EnterScope(drv); }
#line 722 "Parser.cpp"
    break;

  case 6: // function: type VALUE_ID $@1 LEFT_BRACKET param_list RIGHT_BRACKET block
#line 112 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kFUNCTION, yystack_[6].value.as < std::string > (), std::string("__func_") + yystack_[5].value.as < std::string > (), drv);
      if (yystack_[2].value.as < ASTNodePtr > ()) yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > (), ASTNodeTag::kPARAMS);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > (), ASTNodeTag::kBODY);
      LeaveScope(drv);
    }
#line 733 "Parser.cpp"
    break;

  case 7: // param_list: %empty
#line 122 "Parser.y"
    { yylhs.value.as < ASTNodePtr > () = nullptr; }
#line 739 "Parser.cpp"
    break;

  case 8: // param_list: param param_list_follow
#line 124 "Parser.y"
    {
      auto list = MakeASTTreeNode(ASTNodeType::kPARAM_LIST, "param_list", {}, drv);
      list->AddChild(yystack_[1].value.as < ASTNodePtr > ());
      for (auto p : yystack_[0].value.as < std::vector<ASTNodePtr> > ()) list->AddChild(p);
      yylhs.value.as < ASTNodePtr > () = list;
    }
#line 750 "Parser.cpp"
    break;

  case 9: // param_list_follow: %empty
#line 134 "Parser.y"
    { yylhs.value.as < std::vector<ASTNodePtr> > () = std::vector<ASTNodePtr>(); }
#line 756 "Parser.cpp"
    break;

  case 10: // param_list_follow: COMMA param param_list_follow
#line 136 "Parser.y"
    {
      auto v = yystack_[0].value.as < std::vector<ASTNodePtr> > ();
      v.insert(v.begin(), yystack_[1].value.as < ASTNodePtr > ());
      yylhs.value.as < std::vector<ASTNodePtr> > () = std::vector<ASTNodePtr>(v);
    }
#line 766 "Parser.cpp"
    break;

  case 11: // param: type VALUE_ID
#line 145 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kPARAM, yystack_[1].value.as < std::string > (), yystack_[0].value.as < std::string > (), drv);
    }
#line 774 "Parser.cpp"
    break;

  case 12: // $@2: %empty
#line 151 "Parser.y"
                  { EnterScope(drv); }
#line 780 "Parser.cpp"
    break;

  case 13: // block: OPENING_BRACE $@2 statements CLOSING_BRACE
#line 152 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBLOCK, "block", {}, drv);
      for (const auto& child : yystack_[1].value.as < std::vector<ASTNodePtr> > ()) {
        yylhs.value.as < ASTNodePtr > ()->AddChild(child);
      }
      LeaveScope(drv);
    }
#line 792 "Parser.cpp"
    break;

  case 14: // type: INT
#line 161 "Parser.y"
              { yylhs.value.as < std::string > () = "int"; }
#line 798 "Parser.cpp"
    break;

  case 15: // type: BOOLEAN
#line 162 "Parser.y"
              { yylhs.value.as < std::string > () = "bool"; }
#line 804 "Parser.cpp"
    break;

  case 16: // statements: %empty
#line 167 "Parser.y"
    {
      yylhs.value.as < std::vector<ASTNodePtr> > () = {};
    }
#line 812 "Parser.cpp"
    break;

  case 17: // statements: statement statements
#line 171 "Parser.y"
    {
      yystack_[0].value.as < std::vector<ASTNodePtr> > ().insert(yystack_[0].value.as < std::vector<ASTNodePtr> > ().begin(), yystack_[1].value.as < ASTNodePtr > ());
      yylhs.value.as < std::vector<ASTNodePtr> > () = yystack_[0].value.as < std::vector<ASTNodePtr> > ();
    }
#line 821 "Parser.cpp"
    break;

  case 18: // statement: simple_statement SEMICOLON
#line 179 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[1].value.as < ASTNodePtr > ();
    }
#line 829 "Parser.cpp"
    break;

  case 19: // statement: control
#line 183 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > ();
    }
#line 837 "Parser.cpp"
    break;

  case 20: // statement: block
#line 187 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > ();
    }
#line 845 "Parser.cpp"
    break;

  case 21: // simple_statement: left_value assign_operator expression
#line 194 "Parser.y"
    {
      if (yystack_[1].value.as < std::string > () == "=") {
        yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kASSIGN, "simple_statement", yystack_[1].value.as < std::string > (), drv);
        yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ());
        yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ());
      } else {

        auto op = yystack_[1].value.as < std::string > ();
        if (!op.empty() && op.back() == '=') {
          op.pop_back();
        }

        auto bin_op = MakeASTTreeNode(ASTNodeType::kBIN_OP, "simple_statement", op, drv);
        bin_op->AddChild(yystack_[2].value.as < ASTNodePtr > ());
        bin_op->AddChild(yystack_[0].value.as < ASTNodePtr > ());

        yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kASSIGN, "simple_statement", "=", drv);
        yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ());
        yylhs.value.as < ASTNodePtr > ()->AddChild(bin_op);
      }
    }
#line 871 "Parser.cpp"
    break;

  case 22: // simple_statement: declarator
#line 216 "Parser.y"
    { yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > (); }
#line 877 "Parser.cpp"
    break;

  case 23: // simple_statement: call
#line 218 "Parser.y"
    { yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > (); }
#line 883 "Parser.cpp"
    break;

  case 24: // simple_optional: %empty
#line 223 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = {};
    }
#line 891 "Parser.cpp"
    break;

  case 25: // simple_optional: simple_statement
#line 227 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > ();
    }
#line 899 "Parser.cpp"
    break;

  case 26: // declarator: type VALUE_ID
#line 234 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kDECLARE, yystack_[1].value.as < std::string > (), yystack_[0].value.as < std::string > (), drv);
    }
#line 907 "Parser.cpp"
    break;

  case 27: // declarator: type VALUE_ID ASSIGN expression
#line 238 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kDECLARE, yystack_[3].value.as < std::string > (), yystack_[2].value.as < std::string > (), drv);
      auto assign_node = MakeASTTreeNode(ASTNodeType::kASSIGN, yystack_[3].value.as < std::string > (), "=", drv);
      
      assign_node->AddChild(
        MakeASTTreeNode(ASTNodeType::kIDENT, "declarator", yystack_[2].value.as < std::string > (), drv));
      assign_node->AddChild(yystack_[0].value.as < ASTNodePtr > ());  

      yylhs.value.as < ASTNodePtr > ()->AddChild(assign_node);
    }
#line 922 "Parser.cpp"
    break;

  case 28: // left_value: VALUE_ID
#line 252 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kIDENT, "left_value", yystack_[0].value.as < std::string > (), drv);
    }
#line 930 "Parser.cpp"
    break;

  case 29: // left_value: LEFT_BRACKET left_value RIGHT_BRACKET
#line 256 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[1].value.as < ASTNodePtr > ();
    }
#line 938 "Parser.cpp"
    break;

  case 30: // control: IF LEFT_BRACKET expression RIGHT_BRACKET statement ELSE statement
#line 263 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kIF, "control", {}, drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[4].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 949 "Parser.cpp"
    break;

  case 31: // control: IF LEFT_BRACKET expression RIGHT_BRACKET statement
#line 270 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kIF, "control", {}, drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 959 "Parser.cpp"
    break;

  case 32: // control: WHILE LEFT_BRACKET expression RIGHT_BRACKET statement
#line 276 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kWHILE, "while", {}, drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > (), ASTNodeTag::kCOND);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > (), ASTNodeTag::kBODY);
    }
#line 969 "Parser.cpp"
    break;

  case 33: // $@3: %empty
#line 281 "Parser.y"
                       { EnterScope(drv); }
#line 975 "Parser.cpp"
    break;

  case 34: // control: FOR LEFT_BRACKET $@3 simple_optional SEMICOLON expression SEMICOLON simple_optional RIGHT_BRACKET statement
#line 282 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kWHILE, "for", {}, drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[6].value.as < ASTNodePtr > (), ASTNodeTag::kINIT);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[4].value.as < ASTNodePtr > (), ASTNodeTag::kCOND);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > (), ASTNodeTag::kSTEP);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > (), ASTNodeTag::kBODY);

      LeaveScope(drv);
    }
#line 989 "Parser.cpp"
    break;

  case 35: // control: CONTINUE SEMICOLON
#line 292 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCONTINUE, "continue", {}, drv);
    }
#line 997 "Parser.cpp"
    break;

  case 36: // control: BREAK SEMICOLON
#line 296 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBREAK, "break", {}, drv);
    }
#line 1005 "Parser.cpp"
    break;

  case 37: // control: RETURN expression SEMICOLON
#line 300 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kRETURN, "return_statement", {}, drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[1].value.as < ASTNodePtr > ());
    }
#line 1014 "Parser.cpp"
    break;

  case 38: // expression: LEFT_BRACKET expression RIGHT_BRACKET
#line 307 "Parser.y"
                                            { yylhs.value.as < ASTNodePtr > () = yystack_[1].value.as < ASTNodePtr > (); }
#line 1020 "Parser.cpp"
    break;

  case 39: // expression: call
#line 308 "Parser.y"
           { yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > (); }
#line 1026 "Parser.cpp"
    break;

  case 40: // expression: expression PLUS expression
#line 310 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "PLUS", "+", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1032 "Parser.cpp"
    break;

  case 41: // expression: expression SUB expression
#line 312 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "SUB", "-", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1038 "Parser.cpp"
    break;

  case 42: // expression: expression MULT expression
#line 314 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "MULT", "*", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1044 "Parser.cpp"
    break;

  case 43: // expression: expression SLASH expression
#line 316 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "SLASH", "/", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1050 "Parser.cpp"
    break;

  case 44: // expression: expression PERCENT expression
#line 318 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "PERCENT", "%", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1056 "Parser.cpp"
    break;

  case 45: // expression: expression LT expression
#line 320 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LT", "<", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1062 "Parser.cpp"
    break;

  case 46: // expression: expression LT_EQ expression
#line 322 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LT_EQ", "<=", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1068 "Parser.cpp"
    break;

  case 47: // expression: expression GT expression
#line 324 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "GT", ">", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1074 "Parser.cpp"
    break;

  case 48: // expression: expression GT_EQ expression
#line 326 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "GT_EQ", ">=", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1080 "Parser.cpp"
    break;

  case 49: // expression: expression EQ expression
#line 328 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "EQ", "==", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1086 "Parser.cpp"
    break;

  case 50: // expression: expression NOT_EQ expression
#line 330 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "NOT_EQ", "!=", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1092 "Parser.cpp"
    break;

  case 51: // expression: expression LOGIC_AND expression
#line 332 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LOGIC_AND", "&&", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1098 "Parser.cpp"
    break;

  case 52: // expression: expression LOGIC_OR expression
#line 334 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LOGIC_OR", "||", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1104 "Parser.cpp"
    break;

  case 53: // expression: expression BIT_AND expression
#line 336 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "BIT_AND", "&", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1110 "Parser.cpp"
    break;

  case 54: // expression: expression BIT_EX_OR expression
#line 338 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "BIT_EX_OR", "^", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1116 "Parser.cpp"
    break;

  case 55: // expression: expression BIT_OR expression
#line 340 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "BIT_OR", "|", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1122 "Parser.cpp"
    break;

  case 56: // expression: expression LEFT_SHIFT expression
#line 342 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LEFT_SHIFT", "<<", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1128 "Parser.cpp"
    break;

  case 57: // expression: expression RIGHT_SHIFT expression
#line 344 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "RIGHT_SHIFT", ">>", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1134 "Parser.cpp"
    break;

  case 58: // expression: expression QUESTION expression COLON expression
#line 348 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCOND_EXP, "conditional_expression", {}, drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[4].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1140 "Parser.cpp"
    break;

  case 59: // expression: LOGIC_NOT expression
#line 352 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kUN_OP, "LOGIC_NOT", "!", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1146 "Parser.cpp"
    break;

  case 60: // expression: BIT_NOT expression
#line 354 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kUN_OP, "BIT_NOT", "~", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1152 "Parser.cpp"
    break;

  case 61: // expression: SUB expression
#line 356 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kUN_OP, "SUB", "-", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1158 "Parser.cpp"
    break;

  case 62: // expression: VALUE_INTEGER
#line 360 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kVALUE, "int", yystack_[0].value.as < std::string > (), drv); }
#line 1164 "Parser.cpp"
    break;

  case 63: // expression: VALUE_ID
#line 362 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kIDENT, "reference", yystack_[0].value.as < std::string > (), drv); }
#line 1170 "Parser.cpp"
    break;

  case 64: // expression: TRUE
#line 364 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kVALUE, "bool", "true", drv); }
#line 1176 "Parser.cpp"
    break;

  case 65: // expression: FALSE
#line 366 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kVALUE, "bool", "false", drv); }
#line 1182 "Parser.cpp"
    break;

  case 66: // assign_operator: ASSIGN
#line 370 "Parser.y"
                          { yylhs.value.as < std::string > () = "="; }
#line 1188 "Parser.cpp"
    break;

  case 67: // assign_operator: PLUS_ASSIGN
#line 371 "Parser.y"
                          { yylhs.value.as < std::string > () = "+="; }
#line 1194 "Parser.cpp"
    break;

  case 68: // assign_operator: SUB_ASSIGN
#line 372 "Parser.y"
                          { yylhs.value.as < std::string > () = "-="; }
#line 1200 "Parser.cpp"
    break;

  case 69: // assign_operator: MULT_ASSIGN
#line 373 "Parser.y"
                          { yylhs.value.as < std::string > () = "*="; }
#line 1206 "Parser.cpp"
    break;

  case 70: // assign_operator: SLASH_ASSIGN
#line 374 "Parser.y"
                          { yylhs.value.as < std::string > () = "/="; }
#line 1212 "Parser.cpp"
    break;

  case 71: // assign_operator: PERCENT_ASSIGN
#line 375 "Parser.y"
                          { yylhs.value.as < std::string > () = "%="; }
#line 1218 "Parser.cpp"
    break;

  case 72: // assign_operator: BIT_AND_ASSIGN
#line 376 "Parser.y"
                          { yylhs.value.as < std::string > () = "&="; }
#line 1224 "Parser.cpp"
    break;

  case 73: // assign_operator: BIT_EX_OR_ASSIGN
#line 377 "Parser.y"
                          { yylhs.value.as < std::string > () = "^="; }
#line 1230 "Parser.cpp"
    break;

  case 74: // assign_operator: BIT_OR_ASSIGN
#line 378 "Parser.y"
                          { yylhs.value.as < std::string > () = "|="; }
#line 1236 "Parser.cpp"
    break;

  case 75: // assign_operator: LEFT_SHIFT_ASSIGN
#line 379 "Parser.y"
                          { yylhs.value.as < std::string > () = "<<="; }
#line 1242 "Parser.cpp"
    break;

  case 76: // assign_operator: RIGHT_SHIFT_ASSIGN
#line 380 "Parser.y"
                          { yylhs.value.as < std::string > () = ">>="; }
#line 1248 "Parser.cpp"
    break;

  case 77: // call: VALUE_ID LEFT_BRACKET arg_list RIGHT_BRACKET
#line 385 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + yystack_[3].value.as < std::string > (), drv);
      if (yystack_[1].value.as < ASTNodePtr > ()) {
        for (auto child : yystack_[1].value.as < ASTNodePtr > ()->Children())
          yylhs.value.as < ASTNodePtr > ()->AddChild(child);
      }
    }
#line 1260 "Parser.cpp"
    break;

  case 78: // call: PRINT LEFT_BRACKET expression RIGHT_BRACKET
#line 393 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCALL, "call", "__func_print", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[1].value.as < ASTNodePtr > ());
    }
#line 1269 "Parser.cpp"
    break;

  case 79: // call: READ LEFT_BRACKET RIGHT_BRACKET
#line 398 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCALL, "call", "__func_read", drv);
    }
#line 1277 "Parser.cpp"
    break;

  case 80: // call: FLUSH LEFT_BRACKET RIGHT_BRACKET
#line 402 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCALL, "call", "__func_flush", drv);
    }
#line 1285 "Parser.cpp"
    break;

  case 81: // arg_list: %empty
#line 408 "Parser.y"
                             { yylhs.value.as < ASTNodePtr > () = nullptr; }
#line 1291 "Parser.cpp"
    break;

  case 82: // arg_list: expression arg_list_follow
#line 410 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kARG_LIST, "arg_list", {}, drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[1].value.as < ASTNodePtr > ());
      for (auto& a : yystack_[0].value.as < std::vector<ASTNodePtr> > ()) yylhs.value.as < ASTNodePtr > ()->AddChild(a);
    }
#line 1301 "Parser.cpp"
    break;

  case 83: // arg_list_follow: %empty
#line 418 "Parser.y"
                             { yylhs.value.as < std::vector<ASTNodePtr> > () = {}; }
#line 1307 "Parser.cpp"
    break;

  case 84: // arg_list_follow: COMMA expression arg_list_follow
#line 420 "Parser.y"
    {
      yystack_[0].value.as < std::vector<ASTNodePtr> > ().insert(yystack_[0].value.as < std::vector<ASTNodePtr> > ().begin(), yystack_[1].value.as < ASTNodePtr > ());
      yylhs.value.as < std::vector<ASTNodePtr> > () = std::move(yystack_[0].value.as < std::vector<ASTNodePtr> > ());
    }
#line 1316 "Parser.cpp"
    break;


#line 1320 "Parser.cpp"

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


  const signed char parser::yypact_ninf_ = -120;

  const signed char parser::yytable_ninf_ = -1;

  const short
  parser::yypact_[] =
  {
     -52,  -120,  -120,     9,  -120,   -52,     8,  -120,  -120,  -120,
     -10,   -52,     6,     2,    57,    35,   -52,  -120,  -120,  -120,
    -120,     2,    86,  -120,    62,     5,    63,    64,    66,    68,
      69,    70,    72,    74,    97,  -120,   100,    77,    86,    85,
    -120,    44,  -120,  -120,    97,  -120,    80,    97,    84,    88,
      97,    97,  -120,  -120,  -120,    62,  -120,    97,    97,    97,
      97,  -120,  -120,   141,  -120,   106,  -120,  -120,  -120,  -120,
    -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,
      97,   169,    90,  -120,   197,  -120,  -120,   225,   253,    82,
    -120,  -120,  -120,   281,    97,    97,    97,    97,    97,  -120,
      97,    97,    97,    97,    97,    97,    97,    97,    97,    97,
      97,    97,    97,    97,    97,   365,    97,  -120,  -120,  -120,
      86,    86,  -120,    99,  -120,    21,    21,  -120,  -120,  -120,
     -13,   -13,   455,   427,   421,     4,   393,   309,   399,   399,
     399,   399,   461,   461,   365,   169,    71,  -120,    97,    97,
    -120,    86,   337,   365,  -120,    82,    98,    86,  -120
  };

  const signed char
  parser::yydefact_[] =
  {
       3,    15,    14,     0,     2,     3,     0,     1,     4,     5,
       0,     7,     0,     9,     0,     0,     0,     8,    11,    12,
       6,     9,    16,    10,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,     0,     0,    16,     0,
      22,     0,    19,    23,    81,    28,     0,     0,     0,     0,
       0,     0,    33,    35,    36,    63,    62,     0,     0,     0,
       0,    64,    65,     0,    39,    26,    13,    17,    18,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
       0,    83,     0,    29,     0,    79,    80,     0,     0,    24,
      61,    59,    60,     0,     0,     0,     0,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,     0,    82,    77,    78,
       0,     0,    25,     0,    38,    40,    41,    42,    43,    44,
      56,    57,    53,    54,    55,    51,    52,     0,    45,    46,
      47,    48,    49,    50,    27,    83,    31,    32,     0,     0,
      84,     0,     0,    58,    30,    24,     0,     0,    34
  };

  const short
  parser::yypgoto_[] =
  {
    -120,  -120,   119,  -120,  -120,  -120,   104,   129,   131,  -120,
      91,   111,  -119,   -72,    -5,  -120,   127,  -120,  -120,   -32,
    -120,   -22,  -120,    17
  };

  const signed char
  parser::yydefgoto_[] =
  {
       0,     3,     4,     5,    10,    12,    17,    13,    35,    22,
      36,    37,    38,    39,   123,    40,    41,    42,    89,    63,
      80,    64,    82,   117
  };

  const unsigned char
  parser::yytable_[] =
  {
      43,   146,   147,    94,    95,    96,    97,    98,    45,     7,
       1,     9,    81,     2,    11,    84,    43,   122,    87,    88,
      94,    95,    96,    97,    98,    90,    91,    92,    93,    25,
      16,    15,   154,   100,   101,   102,   103,   104,   158,    96,
      97,    98,   108,   109,   110,   111,   112,   113,   115,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      18,    19,   125,   126,   127,   128,   129,    43,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   122,   145,    24,    44,    47,    48,    24,
      49,     6,    50,    51,    52,    53,     6,    54,    43,    43,
      55,    56,    14,    65,    66,    83,    25,    14,    68,    85,
      25,   114,    19,    86,    57,   118,   152,   153,    58,    59,
     151,    60,   148,   157,     8,    23,    26,    27,    28,    43,
      26,    27,    28,    43,    29,    43,    30,    31,    32,    33,
      34,    26,    27,    28,     1,    21,    20,     2,     1,    67,
     156,     2,    46,     0,     0,    61,    62,    94,    95,    96,
      97,    98,   150,     0,    99,     0,     0,     0,     0,     0,
     100,   101,   102,   103,   104,   105,   106,   107,     0,   108,
     109,   110,   111,   112,   113,    94,    95,    96,    97,    98,
       0,     0,     0,     0,     0,     0,     0,   116,   100,   101,
     102,   103,   104,   105,   106,   107,     0,   108,   109,   110,
     111,   112,   113,    94,    95,    96,    97,    98,     0,     0,
       0,     0,   119,     0,     0,     0,   100,   101,   102,   103,
     104,   105,   106,   107,     0,   108,   109,   110,   111,   112,
     113,    94,    95,    96,    97,    98,     0,     0,     0,     0,
     120,     0,     0,     0,   100,   101,   102,   103,   104,   105,
     106,   107,     0,   108,   109,   110,   111,   112,   113,    94,
      95,    96,    97,    98,     0,     0,     0,     0,   121,     0,
       0,     0,   100,   101,   102,   103,   104,   105,   106,   107,
       0,   108,   109,   110,   111,   112,   113,    94,    95,    96,
      97,    98,     0,     0,     0,     0,   124,     0,     0,     0,
     100,   101,   102,   103,   104,   105,   106,   107,     0,   108,
     109,   110,   111,   112,   113,    94,    95,    96,    97,    98,
       0,     0,     0,     0,     0,     0,     0,     0,   100,   101,
     102,   103,   104,   105,   106,   107,   149,   108,   109,   110,
     111,   112,   113,    94,    95,    96,    97,    98,     0,     0,
     155,     0,     0,     0,     0,     0,   100,   101,   102,   103,
     104,   105,   106,   107,     0,   108,   109,   110,   111,   112,
     113,    94,    95,    96,    97,    98,     0,     0,     0,     0,
       0,     0,     0,     0,   100,   101,   102,   103,   104,   105,
     106,   107,     0,   108,   109,   110,   111,   112,   113,    94,
      95,    96,    97,    98,     0,    94,    95,    96,    97,    98,
       0,     0,   100,   101,   102,   103,   104,   105,   100,   101,
       0,   108,   109,   110,   111,   112,   113,    94,    95,    96,
      97,    98,     0,    94,    95,    96,    97,    98,     0,     0,
     100,   101,   102,   103,     0,     0,   100,   101,   102,   108,
     109,   110,   111,   112,   113,   108,   109,   110,   111,   112,
     113,    94,    95,    96,    97,    98,     0,    94,    95,    96,
      97,    98,     0,     0,   100,   101,     0,     0,     0,     0,
     100,   101,     0,   108,   109,   110,   111,   112,   113,   108,
     109,   110,   111
  };

  const short
  parser::yycheck_[] =
  {
      22,   120,   121,    16,    17,    18,    19,    20,     3,     0,
      62,     3,    44,    65,    24,    47,    38,    89,    50,    51,
      16,    17,    18,    19,    20,    57,    58,    59,    60,    24,
      28,    25,   151,    29,    30,    31,    32,    33,   157,    18,
      19,    20,    38,    39,    40,    41,    42,    43,    80,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
       3,    26,    94,    95,    96,    97,    98,    89,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   155,   116,     3,    24,    24,    24,     3,
      24,     0,    24,    24,    24,    23,     5,    23,   120,   121,
       3,     4,    11,     3,    27,    25,    24,    16,    23,    25,
      24,     5,    26,    25,    17,    25,   148,   149,    21,    22,
      49,    24,    23,    25,     5,    21,    44,    45,    46,   151,
      44,    45,    46,   155,    48,   157,    50,    51,    52,    53,
      54,    44,    45,    46,    62,    16,    15,    65,    62,    38,
     155,    65,    25,    -1,    -1,    58,    59,    16,    17,    18,
      19,    20,   145,    -1,    23,    -1,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    36,    -1,    38,
      39,    40,    41,    42,    43,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    -1,    38,    39,    40,
      41,    42,    43,    16,    17,    18,    19,    20,    -1,    -1,
      -1,    -1,    25,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    36,    -1,    38,    39,    40,    41,    42,
      43,    16,    17,    18,    19,    20,    -1,    -1,    -1,    -1,
      25,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    36,    -1,    38,    39,    40,    41,    42,    43,    16,
      17,    18,    19,    20,    -1,    -1,    -1,    -1,    25,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    36,
      -1,    38,    39,    40,    41,    42,    43,    16,    17,    18,
      19,    20,    -1,    -1,    -1,    -1,    25,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    36,    -1,    38,
      39,    40,    41,    42,    43,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    16,    17,    18,    19,    20,    -1,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    36,    -1,    38,    39,    40,    41,    42,
      43,    16,    17,    18,    19,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    36,    -1,    38,    39,    40,    41,    42,    43,    16,
      17,    18,    19,    20,    -1,    16,    17,    18,    19,    20,
      -1,    -1,    29,    30,    31,    32,    33,    34,    29,    30,
      -1,    38,    39,    40,    41,    42,    43,    16,    17,    18,
      19,    20,    -1,    16,    17,    18,    19,    20,    -1,    -1,
      29,    30,    31,    32,    -1,    -1,    29,    30,    31,    38,
      39,    40,    41,    42,    43,    38,    39,    40,    41,    42,
      43,    16,    17,    18,    19,    20,    -1,    16,    17,    18,
      19,    20,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,
      29,    30,    -1,    38,    39,    40,    41,    42,    43,    38,
      39,    40,    41
  };

  const signed char
  parser::yystos_[] =
  {
       0,    62,    65,    71,    72,    73,    80,     0,    72,     3,
      74,    24,    75,    77,    80,    25,    28,    76,     3,    26,
      78,    77,    79,    76,     3,    24,    44,    45,    46,    48,
      50,    51,    52,    53,    54,    78,    80,    81,    82,    83,
      85,    86,    87,    91,    24,     3,    86,    24,    24,    24,
      24,    24,    24,    23,    23,     3,     4,    17,    21,    22,
      24,    58,    59,    89,    91,     3,    27,    81,    23,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      90,    89,    92,    25,    89,    25,    25,    89,    89,    88,
      89,    89,    89,    89,    16,    17,    18,    19,    20,    23,
      29,    30,    31,    32,    33,    34,    35,    36,    38,    39,
      40,    41,    42,    43,     5,    89,    28,    93,    25,    25,
      25,    25,    83,    84,    25,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    82,    82,    23,    37,
      93,    49,    89,    89,    82,    23,    84,    25,    82
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    70,    71,    72,    72,    74,    73,    75,    75,    76,
      76,    77,    79,    78,    80,    80,    81,    81,    82,    82,
      82,    83,    83,    83,    84,    84,    85,    85,    86,    86,
      87,    87,    87,    88,    87,    87,    87,    87,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    91,    91,    91,
      91,    92,    92,    93,    93
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     0,     2,     0,     7,     0,     2,     0,
       3,     2,     0,     4,     1,     1,     0,     2,     2,     1,
       1,     3,     1,     1,     0,     1,     2,     4,     1,     3,
       7,     5,     5,     0,    10,     2,     2,     3,     3,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     5,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     4,     3,
       3,     0,     2,     0,     3
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "VALUE_ID",
  "VALUE_INTEGER", "ASSIGN", "PLUS_ASSIGN", "SUB_ASSIGN", "MULT_ASSIGN",
  "SLASH_ASSIGN", "PERCENT_ASSIGN", "BIT_AND_ASSIGN", "BIT_EX_OR_ASSIGN",
  "BIT_OR_ASSIGN", "LEFT_SHIFT_ASSIGN", "RIGHT_SHIFT_ASSIGN", "PLUS",
  "SUB", "MULT", "SLASH", "PERCENT", "LOGIC_NOT", "BIT_NOT", "SEMICOLON",
  "LEFT_BRACKET", "RIGHT_BRACKET", "OPENING_BRACE", "CLOSING_BRACE",
  "COMMA", "LEFT_SHIFT", "RIGHT_SHIFT", "BIT_AND", "BIT_EX_OR", "BIT_OR",
  "LOGIC_AND", "LOGIC_OR", "QUESTION", "COLON", "LT", "LT_EQ", "GT",
  "GT_EQ", "EQ", "NOT_EQ", "PRINT", "READ", "FLUSH", "STRUCT", "IF",
  "ELSE", "WHILE", "FOR", "CONTINUE", "BREAK", "RETURN", "ASSERT", "ALLOC",
  "ALLOC_ARRAY", "TRUE", "FALSE", "KW_NULL", "STRING", "BOOLEAN", "VOID",
  "CHAR", "INT", "NONE", "IFX", "CON_EXP", "UMINUS", "$accept", "program",
  "functions", "function", "$@1", "param_list", "param_list_follow",
  "param", "block", "$@2", "type", "statements", "statement",
  "simple_statement", "simple_optional", "declarator", "left_value",
  "control", "$@3", "expression", "assign_operator", "call", "arg_list",
  "arg_list_follow", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,    92,    92,   102,   103,   111,   111,   122,   123,   134,
     135,   144,   151,   151,   161,   162,   167,   170,   178,   182,
     186,   193,   215,   217,   223,   226,   233,   237,   251,   255,
     262,   269,   275,   281,   281,   291,   295,   299,   307,   308,
     309,   311,   313,   315,   317,   319,   321,   323,   325,   327,
     329,   331,   333,   335,   337,   339,   341,   343,   347,   351,
     353,   355,   359,   361,   363,   365,   370,   371,   372,   373,
     374,   375,   376,   377,   378,   379,   380,   384,   392,   397,
     401,   408,   409,   418,   419
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
#line 1967 "Parser.cpp"

#line 426 "Parser.y"

