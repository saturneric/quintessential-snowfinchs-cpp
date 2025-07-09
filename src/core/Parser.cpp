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
        value.YY_MOVE_OR_COPY< ASTNodePtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_root_declarations: // root_declarations
      case symbol_kind::S_field_list: // field_list
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
        value.move< ASTNodePtr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_root_declarations: // root_declarations
      case symbol_kind::S_field_list: // field_list
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
        value.copy< ASTNodePtr > (that.value);
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_root_declarations: // root_declarations
      case symbol_kind::S_field_list: // field_list
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
        value.move< ASTNodePtr > (that.value);
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_root_declarations: // root_declarations
      case symbol_kind::S_field_list: // field_list
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
        yylhs.value.emplace< ASTNodePtr > ();
        break;

      case symbol_kind::S_VALUE_ID: // VALUE_ID
      case symbol_kind::S_VALUE_INTEGER: // VALUE_INTEGER
      case symbol_kind::S_type: // type
      case symbol_kind::S_assign_operator: // assign_operator
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_root_declarations: // root_declarations
      case symbol_kind::S_field_list: // field_list
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
  case 2: // program: root_declarations
#line 101 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kPROGRAM, "program", {}, drv);
      drv.SetSyntaxTreeRoot(yylhs.value.as < ASTNodePtr > ());
      for (auto decl : yystack_[0].value.as < std::vector<ASTNodePtr> > ())
        yylhs.value.as < ASTNodePtr > ()->AddChild(decl);
    }
#line 722 "Parser.cpp"
    break;

  case 3: // root_declarations: %empty
#line 111 "Parser.y"
    {
      yylhs.value.as < std::vector<ASTNodePtr> > () = std::vector<ASTNodePtr>();
    }
#line 730 "Parser.cpp"
    break;

  case 4: // root_declarations: root_declaration root_declarations
#line 115 "Parser.y"
    {
      yystack_[0].value.as < std::vector<ASTNodePtr> > ().insert(yystack_[0].value.as < std::vector<ASTNodePtr> > ().begin(), yystack_[1].value.as < ASTNodePtr > ());
      yylhs.value.as < std::vector<ASTNodePtr> > () = yystack_[0].value.as < std::vector<ASTNodePtr> > ();
    }
#line 739 "Parser.cpp"
    break;

  case 5: // root_declaration: function
#line 123 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > ();
    }
#line 747 "Parser.cpp"
    break;

  case 6: // root_declaration: struct_def
#line 127 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > ();
    }
#line 755 "Parser.cpp"
    break;

  case 7: // $@1: %empty
#line 133 "Parser.y"
                  { EnterScope(drv); }
#line 761 "Parser.cpp"
    break;

  case 8: // function: type VALUE_ID $@1 LEFT_PAREN param_list RIGHT_PAREN block
#line 134 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kFUNCTION, yystack_[6].value.as < std::string > (), std::string("__func_") + yystack_[5].value.as < std::string > (), drv);
      if (yystack_[2].value.as < ASTNodePtr > ()) yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > (), ASTNodeTag::kPARAMS);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > (), ASTNodeTag::kBODY);
      LeaveScope(drv);
    }
#line 772 "Parser.cpp"
    break;

  case 9: // struct_def: STRUCT VALUE_ID OPENING_BRACE field_list CLOSING_BRACE SEMICOLON
#line 144 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kSTRUCT, "struct", std::string("__struct_") +yystack_[4].value.as < std::string > (), drv);
      for (auto &f : yystack_[2].value.as < std::vector<ASTNodePtr> > ()) yylhs.value.as < ASTNodePtr > ()->AddChild(f);
    }
#line 781 "Parser.cpp"
    break;

  case 10: // field_list: %empty
#line 152 "Parser.y"
    { 
      yylhs.value.as < std::vector<ASTNodePtr> > () = std::vector<ASTNodePtr>();
    }
#line 789 "Parser.cpp"
    break;

  case 11: // field_list: field field_list
#line 156 "Parser.y"
    {
      yystack_[0].value.as < std::vector<ASTNodePtr> > ().insert(yystack_[0].value.as < std::vector<ASTNodePtr> > ().begin(), yystack_[1].value.as < ASTNodePtr > ());
      yylhs.value.as < std::vector<ASTNodePtr> > () = yystack_[0].value.as < std::vector<ASTNodePtr> > ();
    }
#line 798 "Parser.cpp"
    break;

  case 12: // field: type VALUE_ID SEMICOLON
#line 164 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kFIELD, yystack_[1].value.as < std::string > (), yystack_[2].value.as < std::string > (), drv);
    }
#line 806 "Parser.cpp"
    break;

  case 13: // param_list: %empty
#line 171 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = nullptr;
    }
#line 814 "Parser.cpp"
    break;

  case 14: // param_list: param param_list_follow
#line 175 "Parser.y"
    {
      auto list = MakeASTTreeNode(ASTNodeType::kPARAM_LIST, "param_list", {}, drv);
      list->AddChild(yystack_[1].value.as < ASTNodePtr > ());
      for (auto p : yystack_[0].value.as < std::vector<ASTNodePtr> > ()) list->AddChild(p);
      yylhs.value.as < ASTNodePtr > () = list;
    }
#line 825 "Parser.cpp"
    break;

  case 15: // param_list_follow: %empty
#line 185 "Parser.y"
    { yylhs.value.as < std::vector<ASTNodePtr> > () = std::vector<ASTNodePtr>(); }
#line 831 "Parser.cpp"
    break;

  case 16: // param_list_follow: COMMA param param_list_follow
#line 187 "Parser.y"
    {
      auto v = yystack_[0].value.as < std::vector<ASTNodePtr> > ();
      v.insert(v.begin(), yystack_[1].value.as < ASTNodePtr > ());
      yylhs.value.as < std::vector<ASTNodePtr> > () = std::vector<ASTNodePtr>(v);
    }
#line 841 "Parser.cpp"
    break;

  case 17: // param: type VALUE_ID
#line 196 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kPARAM, yystack_[1].value.as < std::string > (), yystack_[0].value.as < std::string > (), drv);
    }
#line 849 "Parser.cpp"
    break;

  case 18: // $@2: %empty
#line 202 "Parser.y"
                  { EnterScope(drv); }
#line 855 "Parser.cpp"
    break;

  case 19: // block: OPENING_BRACE $@2 statements CLOSING_BRACE
#line 203 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBLOCK, "block", {}, drv);
      for (const auto& child : yystack_[1].value.as < std::vector<ASTNodePtr> > ()) {
        yylhs.value.as < ASTNodePtr > ()->AddChild(child);
      }
      LeaveScope(drv);
    }
#line 867 "Parser.cpp"
    break;

  case 20: // type: INT
#line 212 "Parser.y"
                              { yylhs.value.as < std::string > () = "int"; }
#line 873 "Parser.cpp"
    break;

  case 21: // type: BOOLEAN
#line 213 "Parser.y"
                              { yylhs.value.as < std::string > () = "bool"; }
#line 879 "Parser.cpp"
    break;

  case 22: // type: STRUCT VALUE_ID
#line 214 "Parser.y"
                              { yylhs.value.as < std::string > () = std::string("struct ") + yystack_[0].value.as < std::string > (); }
#line 885 "Parser.cpp"
    break;

  case 23: // type: type MULT
#line 216 "Parser.y"
      {
        yylhs.value.as < std::string > () = yystack_[1].value.as < std::string > () + "*";
      }
#line 893 "Parser.cpp"
    break;

  case 24: // type: type LEFT_BRACKET RIGHT_BRACKET
#line 220 "Parser.y"
      {
        yylhs.value.as < std::string > () = yystack_[2].value.as < std::string > () + "[]";
      }
#line 901 "Parser.cpp"
    break;

  case 25: // statements: %empty
#line 227 "Parser.y"
    {
      yylhs.value.as < std::vector<ASTNodePtr> > () = {};
    }
#line 909 "Parser.cpp"
    break;

  case 26: // statements: statement statements
#line 231 "Parser.y"
    {
      yystack_[0].value.as < std::vector<ASTNodePtr> > ().insert(yystack_[0].value.as < std::vector<ASTNodePtr> > ().begin(), yystack_[1].value.as < ASTNodePtr > ());
      yylhs.value.as < std::vector<ASTNodePtr> > () = yystack_[0].value.as < std::vector<ASTNodePtr> > ();
    }
#line 918 "Parser.cpp"
    break;

  case 27: // statement: simple_statement SEMICOLON
#line 239 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[1].value.as < ASTNodePtr > ();
    }
#line 926 "Parser.cpp"
    break;

  case 28: // statement: control
#line 243 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > ();
    }
#line 934 "Parser.cpp"
    break;

  case 29: // statement: block
#line 247 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > ();
    }
#line 942 "Parser.cpp"
    break;

  case 30: // simple_statement: left_value assign_operator expression
#line 254 "Parser.y"
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
        
        if(yystack_[2].value.as < ASTNodePtr > ()->Type() == ASTNodeType::kIDENT) {
          bin_op->AddChild(MakeASTTreeNode(ASTNodeType::kIDENT, "reference", yystack_[2].value.as < ASTNodePtr > ()->Symbol()->Name(), drv));
        } else {
          bin_op->AddChild(yystack_[2].value.as < ASTNodePtr > ());
        }

        bin_op->AddChild(yystack_[0].value.as < ASTNodePtr > ());

        yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kASSIGN, "simple_statement", "=", drv);
        yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ());
        yylhs.value.as < ASTNodePtr > ()->AddChild(bin_op);
      }
    }
#line 974 "Parser.cpp"
    break;

  case 31: // simple_statement: declarator
#line 282 "Parser.y"
    { yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > (); }
#line 980 "Parser.cpp"
    break;

  case 32: // simple_statement: call
#line 284 "Parser.y"
    { yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > (); }
#line 986 "Parser.cpp"
    break;

  case 33: // simple_optional: %empty
#line 289 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = {};
    }
#line 994 "Parser.cpp"
    break;

  case 34: // simple_optional: simple_statement
#line 293 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > ();
    }
#line 1002 "Parser.cpp"
    break;

  case 35: // declarator: type VALUE_ID
#line 300 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kDECLARE, yystack_[1].value.as < std::string > (), yystack_[0].value.as < std::string > (), drv);
    }
#line 1010 "Parser.cpp"
    break;

  case 36: // declarator: type VALUE_ID ASSIGN expression
#line 304 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kDECLARE, yystack_[3].value.as < std::string > (), yystack_[2].value.as < std::string > (), drv);
      auto assign_node = MakeASTTreeNode(ASTNodeType::kASSIGN, yystack_[3].value.as < std::string > (), "=", drv);
      
      assign_node->AddChild(
        MakeASTTreeNode(ASTNodeType::kIDENT, "declarator", yystack_[2].value.as < std::string > (), drv));
      assign_node->AddChild(yystack_[0].value.as < ASTNodePtr > ());  

      yylhs.value.as < ASTNodePtr > ()->AddChild(assign_node);
    }
#line 1025 "Parser.cpp"
    break;

  case 37: // left_value: VALUE_ID
#line 318 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kIDENT, "lvalue", yystack_[0].value.as < std::string > (), drv);
    }
#line 1033 "Parser.cpp"
    break;

  case 38: // left_value: LEFT_PAREN left_value RIGHT_PAREN
#line 322 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = yystack_[1].value.as < ASTNodePtr > ();
    }
#line 1041 "Parser.cpp"
    break;

  case 39: // left_value: MULT left_value
#line 326 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kUN_OP, "lvalue", "*", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 1050 "Parser.cpp"
    break;

  case 40: // left_value: left_value LEFT_BRACKET expression RIGHT_BRACKET
#line 331 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kARRAY_ACCESS, "subscript", "lvalue", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[3].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[1].value.as < ASTNodePtr > ());
    }
#line 1060 "Parser.cpp"
    break;

  case 41: // left_value: left_value DOT VALUE_ID
#line 337 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kFIELD_ACCESS, "dot", yystack_[0].value.as < std::string > (), drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ());
    }
#line 1069 "Parser.cpp"
    break;

  case 42: // left_value: left_value ARROW VALUE_ID
#line 342 "Parser.y"
    {
      /* equals to (*lvalue).field */
      auto deref = MakeASTTreeNode(ASTNodeType::kUN_OP, "lvalue", "*", drv);
      deref->AddChild(yystack_[2].value.as < ASTNodePtr > ());

      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kFIELD_ACCESS, "arrow", yystack_[0].value.as < std::string > (), drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(deref);
    }
#line 1082 "Parser.cpp"
    break;

  case 43: // control: IF LEFT_PAREN expression RIGHT_PAREN statement ELSE statement
#line 354 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kIF, "control", {}, drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[4].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 1093 "Parser.cpp"
    break;

  case 44: // control: IF LEFT_PAREN expression RIGHT_PAREN statement
#line 361 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kIF, "control", {}, drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 1103 "Parser.cpp"
    break;

  case 45: // control: WHILE LEFT_PAREN expression RIGHT_PAREN statement
#line 367 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kWHILE, "while", {}, drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > (), ASTNodeTag::kCOND);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > (), ASTNodeTag::kBODY);
    }
#line 1113 "Parser.cpp"
    break;

  case 46: // $@3: %empty
#line 372 "Parser.y"
                     { EnterScope(drv); }
#line 1119 "Parser.cpp"
    break;

  case 47: // control: FOR LEFT_PAREN $@3 simple_optional SEMICOLON expression SEMICOLON simple_optional RIGHT_PAREN statement
#line 373 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kWHILE, "for", {}, drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[6].value.as < ASTNodePtr > (), ASTNodeTag::kINIT);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[4].value.as < ASTNodePtr > (), ASTNodeTag::kCOND);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > (), ASTNodeTag::kSTEP);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > (), ASTNodeTag::kBODY);

      LeaveScope(drv);
    }
#line 1133 "Parser.cpp"
    break;

  case 48: // control: CONTINUE SEMICOLON
#line 383 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCONTINUE, "continue", {}, drv);
    }
#line 1141 "Parser.cpp"
    break;

  case 49: // control: BREAK SEMICOLON
#line 387 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBREAK, "break", {}, drv);
    }
#line 1149 "Parser.cpp"
    break;

  case 50: // control: RETURN expression SEMICOLON
#line 391 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kRETURN, "return_statement", {}, drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[1].value.as < ASTNodePtr > ());
    }
#line 1158 "Parser.cpp"
    break;

  case 51: // expression: LEFT_PAREN expression RIGHT_PAREN
#line 398 "Parser.y"
                                        { yylhs.value.as < ASTNodePtr > () = yystack_[1].value.as < ASTNodePtr > (); }
#line 1164 "Parser.cpp"
    break;

  case 52: // expression: call
#line 399 "Parser.y"
           { yylhs.value.as < ASTNodePtr > () = yystack_[0].value.as < ASTNodePtr > (); }
#line 1170 "Parser.cpp"
    break;

  case 53: // expression: expression PLUS expression
#line 401 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "PLUS", "+", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1176 "Parser.cpp"
    break;

  case 54: // expression: expression SUB expression
#line 403 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "SUB", "-", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1182 "Parser.cpp"
    break;

  case 55: // expression: expression MULT expression
#line 405 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "MULT", "*", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1188 "Parser.cpp"
    break;

  case 56: // expression: expression SLASH expression
#line 407 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "SLASH", "/", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1194 "Parser.cpp"
    break;

  case 57: // expression: expression PERCENT expression
#line 409 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "PERCENT", "%", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1200 "Parser.cpp"
    break;

  case 58: // expression: expression LT expression
#line 411 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LT", "<", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1206 "Parser.cpp"
    break;

  case 59: // expression: expression LT_EQ expression
#line 413 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LT_EQ", "<=", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1212 "Parser.cpp"
    break;

  case 60: // expression: expression GT expression
#line 415 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "GT", ">", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1218 "Parser.cpp"
    break;

  case 61: // expression: expression GT_EQ expression
#line 417 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "GT_EQ", ">=", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1224 "Parser.cpp"
    break;

  case 62: // expression: expression EQ expression
#line 419 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "EQ", "==", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1230 "Parser.cpp"
    break;

  case 63: // expression: expression NOT_EQ expression
#line 421 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "NOT_EQ", "!=", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1236 "Parser.cpp"
    break;

  case 64: // expression: expression LOGIC_AND expression
#line 423 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LOGIC_AND", "&&", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1242 "Parser.cpp"
    break;

  case 65: // expression: expression LOGIC_OR expression
#line 425 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LOGIC_OR", "||", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1248 "Parser.cpp"
    break;

  case 66: // expression: expression BIT_AND expression
#line 427 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "BIT_AND", "&", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1254 "Parser.cpp"
    break;

  case 67: // expression: expression BIT_EX_OR expression
#line 429 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "BIT_EX_OR", "^", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1260 "Parser.cpp"
    break;

  case 68: // expression: expression BIT_OR expression
#line 431 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "BIT_OR", "|", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1266 "Parser.cpp"
    break;

  case 69: // expression: expression LEFT_SHIFT expression
#line 433 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LEFT_SHIFT", "<<", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1272 "Parser.cpp"
    break;

  case 70: // expression: expression RIGHT_SHIFT expression
#line 435 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kBIN_OP, "RIGHT_SHIFT", ">>", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1278 "Parser.cpp"
    break;

  case 71: // expression: expression QUESTION expression COLON expression
#line 439 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCOND_EXP, "conditional_expression", {}, drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[4].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ()); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1284 "Parser.cpp"
    break;

  case 72: // expression: LOGIC_NOT expression
#line 443 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kUN_OP, "LOGIC_NOT", "!", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1290 "Parser.cpp"
    break;

  case 73: // expression: BIT_NOT expression
#line 445 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kUN_OP, "BIT_NOT", "~", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1296 "Parser.cpp"
    break;

  case 74: // expression: SUB expression
#line 447 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kUN_OP, "SUB", "-", drv); yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ()); }
#line 1302 "Parser.cpp"
    break;

  case 75: // expression: VALUE_INTEGER
#line 451 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kVALUE, "int", yystack_[0].value.as < std::string > (), drv); }
#line 1308 "Parser.cpp"
    break;

  case 76: // expression: VALUE_ID
#line 453 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kIDENT, "reference", yystack_[0].value.as < std::string > (), drv); }
#line 1314 "Parser.cpp"
    break;

  case 77: // expression: TRUE
#line 455 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kVALUE, "bool", "true", drv); }
#line 1320 "Parser.cpp"
    break;

  case 78: // expression: FALSE
#line 457 "Parser.y"
        { yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kVALUE, "bool", "false", drv); }
#line 1326 "Parser.cpp"
    break;

  case 79: // expression: expression DOT VALUE_ID
#line 460 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kFIELD_ACCESS, "dot", yystack_[0].value.as < std::string > (), drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[2].value.as < ASTNodePtr > ());
    }
#line 1335 "Parser.cpp"
    break;

  case 80: // expression: expression ARROW VALUE_ID
#line 465 "Parser.y"
    {
      /* equals to (*exp).field */
      auto deref = MakeASTTreeNode(ASTNodeType::kUN_OP, "deref", "*", drv);
      deref->AddChild(yystack_[2].value.as < ASTNodePtr > ());

      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kFIELD_ACCESS, "arrow", yystack_[0].value.as < std::string > (), drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(deref);
    }
#line 1348 "Parser.cpp"
    break;

  case 81: // expression: MULT expression
#line 474 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kUN_OP, "deref", "*", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[0].value.as < ASTNodePtr > ());
    }
#line 1357 "Parser.cpp"
    break;

  case 82: // expression: expression LEFT_BRACKET expression RIGHT_BRACKET
#line 479 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kARRAY_ACCESS, "subscript", "exp", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[3].value.as < ASTNodePtr > ());
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[1].value.as < ASTNodePtr > ());
    }
#line 1367 "Parser.cpp"
    break;

  case 83: // expression: KW_NULL
#line 485 "Parser.y"
    { 
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kVALUE, "nullptr", "0", drv); 
    }
#line 1375 "Parser.cpp"
    break;

  case 84: // assign_operator: ASSIGN
#line 491 "Parser.y"
                          { yylhs.value.as < std::string > () = "="; }
#line 1381 "Parser.cpp"
    break;

  case 85: // assign_operator: PLUS_ASSIGN
#line 492 "Parser.y"
                          { yylhs.value.as < std::string > () = "+="; }
#line 1387 "Parser.cpp"
    break;

  case 86: // assign_operator: SUB_ASSIGN
#line 493 "Parser.y"
                          { yylhs.value.as < std::string > () = "-="; }
#line 1393 "Parser.cpp"
    break;

  case 87: // assign_operator: MULT_ASSIGN
#line 494 "Parser.y"
                          { yylhs.value.as < std::string > () = "*="; }
#line 1399 "Parser.cpp"
    break;

  case 88: // assign_operator: SLASH_ASSIGN
#line 495 "Parser.y"
                          { yylhs.value.as < std::string > () = "/="; }
#line 1405 "Parser.cpp"
    break;

  case 89: // assign_operator: PERCENT_ASSIGN
#line 496 "Parser.y"
                          { yylhs.value.as < std::string > () = "%="; }
#line 1411 "Parser.cpp"
    break;

  case 90: // assign_operator: BIT_AND_ASSIGN
#line 497 "Parser.y"
                          { yylhs.value.as < std::string > () = "&="; }
#line 1417 "Parser.cpp"
    break;

  case 91: // assign_operator: BIT_EX_OR_ASSIGN
#line 498 "Parser.y"
                          { yylhs.value.as < std::string > () = "^="; }
#line 1423 "Parser.cpp"
    break;

  case 92: // assign_operator: BIT_OR_ASSIGN
#line 499 "Parser.y"
                          { yylhs.value.as < std::string > () = "|="; }
#line 1429 "Parser.cpp"
    break;

  case 93: // assign_operator: LEFT_SHIFT_ASSIGN
#line 500 "Parser.y"
                          { yylhs.value.as < std::string > () = "<<="; }
#line 1435 "Parser.cpp"
    break;

  case 94: // assign_operator: RIGHT_SHIFT_ASSIGN
#line 501 "Parser.y"
                          { yylhs.value.as < std::string > () = ">>="; }
#line 1441 "Parser.cpp"
    break;

  case 95: // call: VALUE_ID LEFT_PAREN arg_list RIGHT_PAREN
#line 506 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + yystack_[3].value.as < std::string > (), drv);
      if (yystack_[1].value.as < ASTNodePtr > ()) {
        for (auto child : yystack_[1].value.as < ASTNodePtr > ()->Children())
          yylhs.value.as < ASTNodePtr > ()->AddChild(child);
      }
    }
#line 1453 "Parser.cpp"
    break;

  case 96: // call: PRINT LEFT_PAREN arg_list RIGHT_PAREN
#line 514 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + "print", drv);
      if (yystack_[1].value.as < ASTNodePtr > ()) {
        for (auto child : yystack_[1].value.as < ASTNodePtr > ()->Children())
          yylhs.value.as < ASTNodePtr > ()->AddChild(child);
      }
    }
#line 1465 "Parser.cpp"
    break;

  case 97: // call: READ LEFT_PAREN arg_list RIGHT_PAREN
#line 522 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + "read", drv);
      if (yystack_[1].value.as < ASTNodePtr > ()) {
        for (auto child : yystack_[1].value.as < ASTNodePtr > ()->Children())
          yylhs.value.as < ASTNodePtr > ()->AddChild(child);
      }
    }
#line 1477 "Parser.cpp"
    break;

  case 98: // call: FLUSH LEFT_PAREN arg_list RIGHT_PAREN
#line 530 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + "flush", drv);
      if (yystack_[1].value.as < ASTNodePtr > ()) {
        for (auto child : yystack_[1].value.as < ASTNodePtr > ()->Children())
          yylhs.value.as < ASTNodePtr > ()->AddChild(child);
      }
    }
#line 1489 "Parser.cpp"
    break;

  case 99: // call: ALLOC LEFT_PAREN type RIGHT_PAREN
#line 538 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") +"alloc", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(MakeASTTreeNode(ASTNodeType::kTYPE, "type", yystack_[1].value.as < std::string > (), drv));
    }
#line 1498 "Parser.cpp"
    break;

  case 100: // call: ALLOC_ARRAY LEFT_PAREN type COMMA expression RIGHT_PAREN
#line 543 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") +"alloc_array", drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(MakeASTTreeNode(ASTNodeType::kTYPE, "type", yystack_[3].value.as < std::string > (), drv));
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[1].value.as < ASTNodePtr > ());
    }
#line 1508 "Parser.cpp"
    break;

  case 101: // arg_list: %empty
#line 552 "Parser.y"
                             { yylhs.value.as < ASTNodePtr > () = nullptr; }
#line 1514 "Parser.cpp"
    break;

  case 102: // arg_list: expression arg_list_follow
#line 554 "Parser.y"
    {
      yylhs.value.as < ASTNodePtr > () = MakeASTTreeNode(ASTNodeType::kARG_LIST, "arg_list", {}, drv);
      yylhs.value.as < ASTNodePtr > ()->AddChild(yystack_[1].value.as < ASTNodePtr > ());
      for (auto& a : yystack_[0].value.as < std::vector<ASTNodePtr> > ()) yylhs.value.as < ASTNodePtr > ()->AddChild(a);
    }
#line 1524 "Parser.cpp"
    break;

  case 103: // arg_list_follow: %empty
#line 562 "Parser.y"
                             { yylhs.value.as < std::vector<ASTNodePtr> > () = {}; }
#line 1530 "Parser.cpp"
    break;

  case 104: // arg_list_follow: COMMA expression arg_list_follow
#line 564 "Parser.y"
    {
      yystack_[0].value.as < std::vector<ASTNodePtr> > ().insert(yystack_[0].value.as < std::vector<ASTNodePtr> > ().begin(), yystack_[1].value.as < ASTNodePtr > ());
      yylhs.value.as < std::vector<ASTNodePtr> > () = std::move(yystack_[0].value.as < std::vector<ASTNodePtr> > ());
    }
#line 1539 "Parser.cpp"
    break;


#line 1543 "Parser.cpp"

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


  const short parser::yypact_ninf_ = -161;

  const signed char parser::yytable_ninf_ = -1;

  const short
  parser::yypact_[] =
  {
     123,    -2,  -161,  -161,    11,  -161,   123,  -161,  -161,    25,
     -12,  -161,  -161,  -161,  -161,    12,   133,    32,  -161,    22,
      35,   133,   122,   133,  -161,    49,  -161,    74,    75,    69,
     195,  -161,  -161,    76,   133,  -161,  -161,  -161,  -161,    69,
     121,  -161,    81,    44,    44,    82,    83,    84,    85,   107,
     109,   111,   113,   115,   124,   193,  -161,   200,   105,   121,
     134,  -161,   106,  -161,  -161,   193,  -161,   -39,     1,   193,
     193,   193,   133,   133,   193,   193,  -161,  -161,  -161,    81,
    -161,   193,   193,   193,   193,   193,  -161,  -161,  -161,   228,
    -161,   151,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,   193,   162,   170,   193,
     258,   160,  -161,   161,   163,   166,    -8,    39,   288,   318,
     158,    -4,    14,    -4,    -4,   348,   193,   193,   193,   193,
     193,  -161,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   180,   190,   193,
     378,  -161,  -161,   529,   193,  -161,  -161,  -161,  -161,  -161,
    -161,   193,   121,   121,  -161,   171,  -161,    18,    18,    -4,
      -4,    -4,   408,   697,   697,   649,   110,   619,   589,   559,
     438,   -11,   -11,   -11,   -11,   679,   679,  -161,  -161,   529,
    -161,   258,   468,   140,  -161,   193,  -161,   193,  -161,  -161,
     121,   499,   529,  -161,   158,   175,   121,  -161
  };

  const signed char
  parser::yydefact_[] =
  {
       3,     0,    21,    20,     0,     2,     3,     5,     6,     0,
      22,     1,     4,     7,    23,     0,    10,     0,    24,     0,
       0,    10,     0,    13,    22,     0,    11,     0,     0,    15,
       0,     9,    12,     0,     0,    14,    17,    18,     8,    15,
      25,    16,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    25,
       0,    31,     0,    28,    32,   101,    37,    39,     0,   101,
     101,   101,     0,     0,     0,     0,    46,    48,    49,    76,
      75,     0,     0,     0,     0,     0,    77,    78,    83,     0,
      52,    35,    19,    26,    27,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,     0,     0,     0,     0,
     103,     0,    38,     0,     0,     0,     0,     0,     0,     0,
      33,    74,    81,    72,    73,     0,     0,     0,     0,     0,
       0,    50,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    42,    41,    30,     0,   102,    95,    96,    97,    98,
      99,     0,     0,     0,    34,     0,    51,    53,    54,    55,
      56,    57,     0,    69,    70,    66,    67,    68,    64,    65,
       0,    58,    59,    60,    61,    62,    63,    80,    79,    36,
      40,   103,     0,    44,    45,     0,    82,     0,   104,   100,
       0,     0,    71,    43,    33,     0,     0,    47
  };

  const short
  parser::yypgoto_[] =
  {
    -161,  -161,   206,  -161,  -161,  -161,  -161,   198,  -161,  -161,
     177,   167,   187,  -161,    29,   164,  -160,  -108,    21,  -161,
      30,  -161,  -161,   -51,  -161,   -40,   -10,    31
  };

  const unsigned char
  parser::yydefgoto_[] =
  {
       0,     4,     5,     6,     7,    17,     8,    20,    21,    28,
      35,    29,    56,    40,    57,    58,    59,    60,   165,    61,
      62,    63,   120,   110,   109,    90,   111,   155
  };

  const unsigned char
  parser::yytable_[] =
  {
      64,    10,   193,   194,    89,   126,   127,   128,   129,   130,
      14,    11,   164,   107,   108,   132,    16,   160,    15,    64,
     133,   134,   132,   118,   119,    24,   112,   106,    13,     9,
     121,   122,   123,   124,   125,     9,   128,   129,   130,    18,
     203,   147,   148,    14,   132,    22,   207,    66,   147,   148,
      22,    15,    30,   107,   108,   150,    23,    14,   153,   113,
     114,   115,    43,    30,    25,    15,   147,   148,    44,   161,
     147,   148,    31,    67,    68,   167,   168,   169,   170,   171,
      64,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   164,    32,   189,    34,
      33,   116,   117,   191,    37,    65,    69,    70,    71,    72,
     192,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,    64,    64,    42,    27,   126,   127,   128,   129,
     130,    73,   106,    74,    92,    75,   132,    76,    77,    43,
      14,   133,   134,   135,   201,    44,   202,    78,    15,    37,
     141,   142,   143,   144,   145,   146,   149,    94,   107,   108,
      64,    42,   147,   148,    64,   151,    64,    45,    46,    47,
      48,    49,    19,   152,     1,    50,    43,    51,    52,    53,
      54,    55,    44,   187,    19,   156,   157,     2,   158,     2,
       3,   159,     3,   188,   195,   200,    79,    80,    36,     2,
     206,    39,     3,    91,    45,    46,    47,    48,    49,    19,
      81,    82,    12,    14,    83,    84,    41,    85,    14,    26,
      38,    15,   198,    93,     2,   205,    15,     3,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    45,
      46,    47,    48,    49,   126,   127,   128,   129,   130,     0,
       0,   131,     0,     0,   132,    86,    87,    88,     0,   133,
     134,   135,   136,   137,   138,   139,   140,     0,   141,   142,
     143,   144,   145,   146,   126,   127,   128,   129,   130,     0,
     147,   148,     0,     0,   132,     0,     0,     0,   154,   133,
     134,   135,   136,   137,   138,   139,   140,     0,   141,   142,
     143,   144,   145,   146,   126,   127,   128,   129,   130,     0,
     147,   148,     0,   162,   132,     0,     0,     0,     0,   133,
     134,   135,   136,   137,   138,   139,   140,     0,   141,   142,
     143,   144,   145,   146,   126,   127,   128,   129,   130,     0,
     147,   148,     0,   163,   132,     0,     0,     0,     0,   133,
     134,   135,   136,   137,   138,   139,   140,     0,   141,   142,
     143,   144,   145,   146,   126,   127,   128,   129,   130,     0,
     147,   148,     0,   166,   132,     0,     0,     0,     0,   133,
     134,   135,   136,   137,   138,   139,   140,     0,   141,   142,
     143,   144,   145,   146,   126,   127,   128,   129,   130,     0,
     147,   148,     0,     0,   132,   190,     0,     0,     0,   133,
     134,   135,   136,   137,   138,   139,   140,     0,   141,   142,
     143,   144,   145,   146,   126,   127,   128,   129,   130,     0,
     147,   148,     0,     0,   132,   196,     0,     0,     0,   133,
     134,   135,   136,   137,   138,   139,   140,     0,   141,   142,
     143,   144,   145,   146,   126,   127,   128,   129,   130,     0,
     147,   148,     0,     0,   132,     0,     0,     0,     0,   133,
     134,   135,   136,   137,   138,   139,   140,   197,   141,   142,
     143,   144,   145,   146,   126,   127,   128,   129,   130,     0,
     147,   148,     0,   199,   132,     0,     0,     0,     0,   133,
     134,   135,   136,   137,   138,   139,   140,     0,   141,   142,
     143,   144,   145,   146,     0,   126,   127,   128,   129,   130,
     147,   148,   204,     0,     0,   132,     0,     0,     0,     0,
     133,   134,   135,   136,   137,   138,   139,   140,     0,   141,
     142,   143,   144,   145,   146,   126,   127,   128,   129,   130,
       0,   147,   148,     0,     0,   132,     0,     0,     0,     0,
     133,   134,   135,   136,   137,   138,   139,   140,     0,   141,
     142,   143,   144,   145,   146,   126,   127,   128,   129,   130,
       0,   147,   148,     0,     0,   132,     0,     0,     0,     0,
     133,   134,   135,   136,   137,   138,     0,     0,     0,   141,
     142,   143,   144,   145,   146,   126,   127,   128,   129,   130,
       0,   147,   148,     0,     0,   132,     0,     0,     0,     0,
     133,   134,   135,   136,   137,     0,     0,     0,     0,   141,
     142,   143,   144,   145,   146,   126,   127,   128,   129,   130,
       0,   147,   148,     0,     0,   132,     0,     0,     0,     0,
     133,   134,   135,   136,     0,     0,     0,     0,     0,   141,
     142,   143,   144,   145,   146,   126,   127,   128,   129,   130,
       0,   147,   148,     0,     0,   132,     0,     0,     0,     0,
     133,   134,     0,     0,     0,     0,     0,     0,     0,   141,
     142,   143,   144,   145,   146,   126,   127,   128,   129,   130,
       0,   147,   148,     0,     0,   132,     0,     0,     0,     0,
     133,   134,     0,   126,   127,   128,   129,   130,     0,   141,
     142,   143,   144,   132,     0,     0,     0,     0,     0,     0,
       0,   147,   148,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   147,
     148
  };

  const short
  parser::yycheck_[] =
  {
      40,     3,   162,   163,    55,    16,    17,    18,    19,    20,
      18,     0,   120,    52,    53,    26,    28,    25,    26,    59,
      31,    32,    26,    74,    75,     3,    25,    26,     3,     0,
      81,    82,    83,    84,    85,     6,    18,    19,    20,    27,
     200,    52,    53,    18,    26,    16,   206,     3,    52,    53,
      21,    26,    23,    52,    53,   106,    24,    18,   109,    69,
      70,    71,    18,    34,    29,    26,    52,    53,    24,    30,
      52,    53,    23,    43,    44,   126,   127,   128,   129,   130,
     120,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   204,    23,   149,    30,
      25,    72,    73,   154,    28,    24,    24,    24,    24,    24,
     161,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,   162,   163,     3,     3,    16,    17,    18,    19,
      20,    24,    26,    24,    29,    24,    26,    24,    23,    18,
      18,    31,    32,    33,   195,    24,   197,    23,    26,    28,
      40,    41,    42,    43,    44,    45,     5,    23,    52,    53,
     200,     3,    52,    53,   204,     3,   206,    46,    47,    48,
      49,    50,    51,     3,    51,    54,    18,    56,    57,    58,
      59,    60,    24,     3,    51,    25,    25,    66,    25,    66,
      69,    25,    69,     3,    23,    55,     3,     4,     3,    66,
      25,    34,    69,     3,    46,    47,    48,    49,    50,    51,
      17,    18,     6,    18,    21,    22,    39,    24,    18,    21,
      33,    26,   191,    59,    66,   204,    26,    69,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      47,    48,    49,    50,    16,    17,    18,    19,    20,    -1,
      -1,    23,    -1,    -1,    26,    62,    63,    64,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    40,    41,
      42,    43,    44,    45,    16,    17,    18,    19,    20,    -1,
      52,    53,    -1,    -1,    26,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    40,    41,
      42,    43,    44,    45,    16,    17,    18,    19,    20,    -1,
      52,    53,    -1,    25,    26,    -1,    -1,    -1,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    40,    41,
      42,    43,    44,    45,    16,    17,    18,    19,    20,    -1,
      52,    53,    -1,    25,    26,    -1,    -1,    -1,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    40,    41,
      42,    43,    44,    45,    16,    17,    18,    19,    20,    -1,
      52,    53,    -1,    25,    26,    -1,    -1,    -1,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    40,    41,
      42,    43,    44,    45,    16,    17,    18,    19,    20,    -1,
      52,    53,    -1,    -1,    26,    27,    -1,    -1,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    40,    41,
      42,    43,    44,    45,    16,    17,    18,    19,    20,    -1,
      52,    53,    -1,    -1,    26,    27,    -1,    -1,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    40,    41,
      42,    43,    44,    45,    16,    17,    18,    19,    20,    -1,
      52,    53,    -1,    -1,    26,    -1,    -1,    -1,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    16,    17,    18,    19,    20,    -1,
      52,    53,    -1,    25,    26,    -1,    -1,    -1,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    40,    41,
      42,    43,    44,    45,    -1,    16,    17,    18,    19,    20,
      52,    53,    23,    -1,    -1,    26,    -1,    -1,    -1,    -1,
      31,    32,    33,    34,    35,    36,    37,    38,    -1,    40,
      41,    42,    43,    44,    45,    16,    17,    18,    19,    20,
      -1,    52,    53,    -1,    -1,    26,    -1,    -1,    -1,    -1,
      31,    32,    33,    34,    35,    36,    37,    38,    -1,    40,
      41,    42,    43,    44,    45,    16,    17,    18,    19,    20,
      -1,    52,    53,    -1,    -1,    26,    -1,    -1,    -1,    -1,
      31,    32,    33,    34,    35,    36,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    45,    16,    17,    18,    19,    20,
      -1,    52,    53,    -1,    -1,    26,    -1,    -1,    -1,    -1,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    45,    16,    17,    18,    19,    20,
      -1,    52,    53,    -1,    -1,    26,    -1,    -1,    -1,    -1,
      31,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    45,    16,    17,    18,    19,    20,
      -1,    52,    53,    -1,    -1,    26,    -1,    -1,    -1,    -1,
      31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    45,    16,    17,    18,    19,    20,
      -1,    52,    53,    -1,    -1,    26,    -1,    -1,    -1,    -1,
      31,    32,    -1,    16,    17,    18,    19,    20,    -1,    40,
      41,    42,    43,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,
      53
  };

  const signed char
  parser::yystos_[] =
  {
       0,    51,    66,    69,    76,    77,    78,    79,    81,    89,
       3,     0,    77,     3,    18,    26,    28,    80,    27,    51,
      82,    83,    89,    24,     3,    29,    82,     3,    84,    86,
      89,    23,    23,    25,    30,    85,     3,    28,    87,    86,
      88,    85,     3,    18,    24,    46,    47,    48,    49,    50,
      54,    56,    57,    58,    59,    60,    87,    89,    90,    91,
      92,    94,    95,    96,   100,    24,     3,    95,    95,    24,
      24,    24,    24,    24,    24,    24,    24,    23,    23,     3,
       4,    17,    18,    21,    22,    24,    62,    63,    64,    98,
     100,     3,    29,    90,    23,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    26,    52,    53,    99,
      98,   101,    25,   101,   101,   101,    89,    89,    98,    98,
      97,    98,    98,    98,    98,    98,    16,    17,    18,    19,
      20,    23,    26,    31,    32,    33,    34,    35,    36,    37,
      38,    40,    41,    42,    43,    44,    45,    52,    53,     5,
      98,     3,     3,    98,    30,   102,    25,    25,    25,    25,
      25,    30,    25,    25,    92,    93,    25,    98,    98,    98,
      98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    98,    98,    98,    98,     3,     3,    98,
      27,    98,    98,    91,    91,    23,    27,    39,   102,    25,
      55,    98,    98,    91,    23,    93,    25,    91
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    75,    76,    77,    77,    78,    78,    80,    79,    81,
      82,    82,    83,    84,    84,    85,    85,    86,    88,    87,
      89,    89,    89,    89,    89,    90,    90,    91,    91,    91,
      92,    92,    92,    93,    93,    94,    94,    95,    95,    95,
      95,    95,    95,    96,    96,    96,    97,    96,    96,    96,
      96,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    98,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,   100,   100,   100,   100,   100,
     100,   101,   101,   102,   102
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     0,     2,     1,     1,     0,     7,     6,
       0,     2,     3,     0,     2,     0,     3,     2,     0,     4,
       1,     1,     2,     2,     3,     0,     2,     2,     1,     1,
       3,     1,     1,     0,     1,     2,     4,     1,     3,     2,
       4,     3,     3,     7,     5,     5,     0,    10,     2,     2,
       3,     3,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     5,     2,     2,     2,     1,     1,     1,     1,     3,
       3,     2,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     4,     4,     4,     4,
       6,     0,     2,     0,     3
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
  "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACKET", "RIGHT_BRACKET",
  "OPENING_BRACE", "CLOSING_BRACE", "COMMA", "LEFT_SHIFT", "RIGHT_SHIFT",
  "BIT_AND", "BIT_EX_OR", "BIT_OR", "LOGIC_AND", "LOGIC_OR", "QUESTION",
  "COLON", "LT", "LT_EQ", "GT", "GT_EQ", "EQ", "NOT_EQ", "PRINT", "READ",
  "FLUSH", "ALLOC", "ALLOC_ARRAY", "STRUCT", "ARROW", "DOT", "IF", "ELSE",
  "WHILE", "FOR", "CONTINUE", "BREAK", "RETURN", "ASSERT", "TRUE", "FALSE",
  "KW_NULL", "STRING", "BOOLEAN", "VOID", "CHAR", "INT", "NONE", "IFX",
  "CON_EXP", "UMINUS", "DEREF", "$accept", "program", "root_declarations",
  "root_declaration", "function", "$@1", "struct_def", "field_list",
  "field", "param_list", "param_list_follow", "param", "block", "$@2",
  "type", "statements", "statement", "simple_statement", "simple_optional",
  "declarator", "left_value", "control", "$@3", "expression",
  "assign_operator", "call", "arg_list", "arg_list_follow", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   100,   100,   111,   114,   122,   126,   133,   133,   143,
     152,   155,   163,   171,   174,   185,   186,   195,   202,   202,
     212,   213,   214,   215,   219,   227,   230,   238,   242,   246,
     253,   281,   283,   289,   292,   299,   303,   317,   321,   325,
     330,   336,   341,   353,   360,   366,   372,   372,   382,   386,
     390,   398,   399,   400,   402,   404,   406,   408,   410,   412,
     414,   416,   418,   420,   422,   424,   426,   428,   430,   432,
     434,   438,   442,   444,   446,   450,   452,   454,   456,   459,
     464,   473,   478,   484,   491,   492,   493,   494,   495,   496,
     497,   498,   499,   500,   501,   505,   513,   521,   529,   537,
     542,   552,   553,   562,   563
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
#line 2262 "Parser.cpp"

#line 570 "Parser.y"

