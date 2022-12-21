# include <map>
# include <vector>
# include <string>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <iostream>

using namespace std;

struct TokenType {
  int line;
  int column;
  bool is_lambda;
  bool is_procedure;
  string type;
  string token;
} ; // end struct TokenType

struct NodeType {
  TokenType item;
  NodeType *left;
  NodeType *right;
} ; // end struct NodeType

typedef NodeType* NodePtr;

NodeType* g_nil = new NodeType;
NodeType* g_true = new NodeType;

void GlobalNilTrueInitialization() {
  g_nil->left = NULL;
  g_nil->right = NULL;
  g_nil->item.type = "NIL";
  g_nil->item.token = "nil";

  g_true->left = NULL;
  g_true->right = NULL;
  g_true->item.type = "T";
  g_true->item.token = "#t";
} // end GlobalNilTrueInitialization()

int g_line = 1;
int g_column = 0;

bool GetChar( char & ch ) {
  if ( scanf( "%c", &ch ) != EOF ) {
    g_column++;
    return true;
  } // if
  else return false;
} // end GetChar()

bool PeekChar( char & ch ) {
  if ( cin.peek() != EOF ) {
    ch = cin.peek();
    return true;
  } // if
  else return false;
} // end PeekChar()

void SkipLine() {
  char ch = '\0';
  while ( scanf( "%c", &ch ) != EOF && ch != '\n' ) ;
  g_line++;
  g_column = 0;
} // end SkipLine()

class PrettyPrinter {
private:
  int m_column_;
  bool m_print_procedure_;
  map<string, NodeType*> m_user_defined_;

  bool IsUserDefined( string name ) {
    map<string, NodeType*>::iterator begin = m_user_defined_.begin();
    map<string, NodeType*>::iterator end = m_user_defined_.end();

    map<string, NodeType*>::iterator itr;
    for ( itr = begin ; itr != end ; itr++ ) { // check if the key in the map == name
      if ( itr->first == name ) return true;
    } // end for

    return false;
  } // end IsUserDefined()

  bool IsLambdaProc( NodeType* node ) {
    return m_print_procedure_ &&
           node && node->left && node->left->item.token == "lambda" && node->left->item.is_procedure;
  } // end IsLambdaProc()

  bool IsUserDefinedFunc( NodeType* node ) {
    return node && node->left && node->left->left &&
           IsUserDefined( node->left->left->item.token ) && node->left->left->item.is_procedure;
  } // end IsUserDefinedFunc()

  bool IsLeaf( NodeType* node ) {
    return node && ( ! node->left && ! node->right );
  } // end IsLeaf()

  void PrintItem( TokenType item ) {
    if ( item.type == "INT" ) {
      // cout << atoi( item.token.c_str() ) << '\n';
      printf( "%g\n", atof( item.token.c_str() ) );
    } // if
    else if ( item.type == "FLOAT" ) {
      printf( "%.3f\n", atof( item.token.c_str() ) );
    } // else if
    else if ( item.type == "NIL" ) {
      cout << "nil\n";
    } // else if
    else if ( item.type == "T" ) {
      cout << "#t\n";
    } // else if
    else if ( item.type == "STRING" ) {
      cout << item.token << '\n';
    } // else if
    else if ( item.type == "SYMBOL" ) {
      if ( item.is_lambda ) cout << "#<procedure lambda>" << '\n';
      else if ( item.is_procedure && m_print_procedure_ )
        cout << "#<procedure " << item.token << ">" << '\n';
      else cout << item.token << '\n';
    } // else if
    else if ( item.type == "QUOTE" ) {
      cout << "quote\n";
    } // else if
  } // end PrintItem()

  void Traverse( NodeType* node, int m, bool printLP ) {
    if ( node ) {
      // print
      if ( ( IsLambdaProc( node ) || IsUserDefinedFunc( node ) ) ) {
        // print M+2 spaces, print sn
        if ( m_column_ != ( m * 2 ) ) {
          for ( int i = 0 ; i < m * 2 ; i++ )
            cout << " ";
        } // if

        if ( IsLambdaProc( node ) ) PrintItem( node->left->item );
        else PrintItem( node->left->left->item );

        m_column_ = 0;
      } // if
      else {
        if ( ! node->item.token.empty() ) { // node is not empty
          // print M+2 spaces, print sn
          if ( m_column_ != ( m * 2 ) ) {
            for ( int i = 0 ; i < m * 2 ; i++ )
              cout << " ";
          } // if

          PrintItem( node->item );
          m_column_ = 0;
        } // if
        else if ( printLP ) {
          m++;
          m_column_ += 2;
          if ( m_column_ != ( m * 2 ) ) {
            for ( int i = 0 ; i < ( ( m - 1 ) * 2 ) ; i++ )
              cout << " ";
            m_column_ += ( ( m - 1 ) * 2 );
          } // if

          cout << "( ";
        } // else if

        // go left
        printLP = true;
        Traverse( node->left, m, printLP );

        // go right
        printLP = false;
        if ( IsLeaf( node->right ) || IsLambdaProc( node->right ) || IsUserDefinedFunc( node->right ) ) {
          if ( node->right == g_nil ) ; // cout << "nil" << endl
          else if ( IsLambdaProc( node->right ) || IsUserDefinedFunc( node->right ) ) {
            // print M+2 spaces, print '.', print '\n'
            for ( int i = 0 ; i < m * 2 ; i++ )
              cout << " ";
            cout << ".\n";
            // print M+2 spaces, print snn
            for ( int i = 0 ; i < m * 2 ; i++ )
              cout << " ";

            if ( IsLambdaProc( node->right ) ) PrintItem( node->right->left->item );
            else PrintItem( node->right->left->left->item );
            // cout << "#<procedure lambda>" << '\n';
          } // else if
          else {
            // print M+2 spaces, print '.', print '\n'
            for ( int i = 0 ; i < m * 2 ; i++ )
              cout << " ";
            cout << ".\n";
            // print M+2 spaces, print snn
            for ( int i = 0 ; i < m * 2 ; i++ )
              cout << " ";
            PrintItem( node->right->item );
          } // else

          m--;
          m_column_ = 0;
          // print M spaces, print ')', print '\n'
          for ( int i = 0 ; i < m * 2 ; i++ )
            cout << " ";
          cout << ")\n";
        } // if
        else Traverse( node->right, m, printLP );
      } // else
    } // if
  } // end Traverse()

public:
  PrettyPrinter() {
    m_column_ = 0;
    m_user_defined_.clear();
    m_print_procedure_ = true;
  } // end PrettyPrinter()

  void SetUserDefinedFunc( map<string, NodeType*>& user_defined_ ) {
    m_user_defined_ = user_defined_;
  } // end SetUserDefinedFunc()

  void SetPrintProcedure( bool print_procedure ) { // set to false when error occurs
    m_print_procedure_ = print_procedure;
  } // end SetPrintProcedure()

  void PrettyPrint( NodeType* root ) {
    m_column_ = 0;
    int m = 0, printLP = true;
    Traverse( root, m, printLP );
  } // end PrettyPrint()

  void Reset() {
    m_column_ = 0;
    m_user_defined_.clear();
    m_print_procedure_ = true;
  } // end Reset()

} ; // end class PrettyPrinter

class Scanner {
private:
  bool IsWhiteSpace( char ch ) {
    return ( ( ch == ' ' ) || ( ch == '\n' ) || ( ch == '\t' ) );
  } // end IsWhiteSpace()

  bool IsSeparator( char ch ) {
    return ( ( ch == '(' ) || ( ch == ')' ) || ( ch == '\'' ) )
             || ( ch == '\"' ) || ( ch == ';' ) || ( ch == '\\' );
  } // end IsSeparator()

  void ConstructTokenNode( TokenType & token_node, string token, int line, int column, string type ) {
    token_node.is_lambda = false;
    token_node.is_procedure = false;

    token_node.token = token;
    token_node.line = line;
    if ( type == "case1" )
      token_node.column = column - token.size() + 1;
    else // "case2"
      token_node.column = column;
  } // end ConstructTokenNode()

public:
  TokenType GetToken() {
    char ch = '\0';
    int token_line = 0, token_column = 0;
    string token;
    TokenType token_node;

    while ( PeekChar( ch ) ) {
      if ( IsWhiteSpace( ch ) ) {
        if ( ! token.empty() ) { // operand
          ConstructTokenNode( token_node, token, g_line, g_column, "case1" );
          return token_node;
        } // if

        GetChar( ch );

        if ( ch == '\n' ) {
          g_line++;
          g_column = 0;
        } // if
      } // if
      else if ( IsSeparator( ch ) && ! token.empty() ) { // operand
        ConstructTokenNode( token_node, token, g_line, g_column, "case1" );
        return token_node;
      } // else if
      else if ( IsSeparator( ch ) && token.empty() ) { // separator or () or string

        GetChar( ch );
        token_line = g_line;
        token_column = g_column;

        if ( ch == ';' ) { // comments
          SkipLine();
          if ( ! token.empty() ) { // operand (ignore comments)
            ConstructTokenNode( token_node, token, g_line, g_column, "case1" );
            return token_node;
          } // if
        } // if
        else {
          token += ch;

          if ( ch == '(' ) { // ()
            while ( IsWhiteSpace( cin.peek() ) ) {
              GetChar( ch );

              if ( ch == '\n' ) {
                g_line++;
                g_column = 0;
              } // if
            } // end while

            if ( cin.peek() == ')' ) {
              GetChar( ch );
              token += ch; // "()"
            } // if
          } // if
          else if ( ch == '\"' ) {
            while ( GetChar( ch ) && ch != '\"' && ch != '\n' ) {
              if ( ch == '\\' && cin.peek() == '\\' ) { // "\\"
                GetChar( ch );
                token += '\\';
              } // if
              else if ( ch == '\\' && cin.peek() == '\"' ) { // "\""
                GetChar( ch );
                token += '\"';
              } // else if
              else if ( ch == '\\' && cin.peek() == 'n' ) { // "\n"
                GetChar( ch );
                token += '\n';
              } // else if
              else if ( ch == '\\' && cin.peek() == 't' ) { // "\t"
                GetChar( ch );
                token += '\t';
              } // else if
              else token += ch;

            } // end while

            if ( ch == '\"' ) token += ch;
            else if ( ch == '\n' ) { // error_no_3
              ConstructTokenNode( token_node, "END-OF-LINE", g_line, g_column, "case2" );
              return token_node;
            } // else if
            else { // error_no_4
              if ( token != "" ) {
                if ( token[0] == '\"' )
                  ConstructTokenNode( token_node, "END-OF-LINE", g_line, ++g_column, "case2" );
                else ConstructTokenNode( token_node, token, token_line, token_column, "case2" );
              } // if
              else ConstructTokenNode( token_node, "END-OF-FILE", g_line, g_column, "case2" );
              return token_node;
            } // else
          } // else if

          ConstructTokenNode( token_node, token, token_line, token_column, "case2" );
          return token_node;
        } // else
      } // else if
      else {
        GetChar( ch );
        token += ch;
      } // else
    } // end while

    if ( token != "" ) {
      if ( token[0] == '\"' ) ConstructTokenNode( token_node, "END-OF-LINE", g_line, ++g_column, "case2" );
      else ConstructTokenNode( token_node, token, token_line, token_column, "case2" );
    } // if
    else ConstructTokenNode( token_node, "END-OF-FILE", g_line, g_column, "case1" );

    return token_node;
  } // end GetToken()
  
  void Reset() {
    char ch = '\0';
    
    // cout << "in reset with line " << g_line << " column " << g_column << endl;
    if ( g_column == 0 ) { // nothing in the front
      g_line = 1;
      g_column = 0;
    } // if
    else {
      g_line = 1;
      g_column = 0;
      
      while ( cin.peek() == ' ' || cin.peek() == '\t' ) {
        GetChar( ch );
      } // end while
      
      if ( cin.peek() == ';' ) {
        SkipLine();
        g_line = 1;
        g_column = 0;
      } // if
      else if ( cin.peek() == '\n' ) {
        scanf( "%c", &ch ); // skip '\n'
        g_line = 1;
        g_column = 0;
      } // if
    } // else
  } // end Reset()

} ; // end class Scanner

class Parser {
private:
  Scanner m_scanner_;
  TokenType m_cur_token_node_;
  vector<TokenType> m_result_expr_;
  NodeType* m_root_for_parse_tree_;

  // function for building parse tree //

  bool IsOkToInsertNil( int index ) {
    int level = 1, num_of_dot = 0;

    for ( ; index >= 0 ; index-- ) {
      if ( m_result_expr_[index].type == "LEFT-PAREN" ) level--;
      else if ( m_result_expr_[index].type == "RIGHT-PAREN" ) level++;

      if ( level == 1 && m_result_expr_[index].type == "DOT" ) num_of_dot++;

      if ( level == 0 ) { // same level
        if ( num_of_dot == 0 ) return true;
        else return false;
      } // if
    } // end for

    return false;
  } // end IsOkToInsertNil()

  NodeType* Insert( bool connect ) { // ((( 1 . 2 ) ( 3 4 ) 5 . 6 ) 7 . 8 )
    if ( ! m_result_expr_.empty() ) { // ( ( 1 . 2 ) . ( 3 . 4 ) )
      NodeType* node;
      TokenType front = m_result_expr_.front();

      if ( connect || front.type == "LEFT-PAREN" || front.type == "QUOTE" ) ;
      else m_result_expr_.erase( m_result_expr_.begin() );

      if ( front.type == "DOT" ) { // . ( sexp )
        connect = false;
        return Insert( connect );
      } // if
      else if ( front.token == "nil_node" ) return g_nil;
      else if ( connect ) {
        node = new NodeType;
        connect = false;
        node->left = Insert( connect );

        connect = true;
        node->right = Insert( connect );

        return node;
      } // else if
      else if ( front.type == "LEFT-PAREN" ) { // if there is "connect", keep the "("
        m_result_expr_.erase( m_result_expr_.begin() );
        node = new NodeType;
        connect = false;
        node->left = Insert( connect );

        connect = true;
        node->right = Insert( connect );

        return node;
      } // else if
      else if ( front.type == "QUOTE" ) {
        m_result_expr_.erase( m_result_expr_.begin() );
        node = new NodeType;

        node->left = new NodeType;
        node->left->item = front;
        node->left->left = NULL;
        node->left->right = NULL;

        connect = true;
        node->right = Insert( connect );

        return node;
      } // else if
      else if ( front.type == "NIL" ) return g_nil;
      else { // just atom
        node = new NodeType;
        node->item = front;
        node->left = NULL;
        node->right = NULL;

        return node;
      } // else

    } // if

    return g_nil;
  } // end Insert()

  void Transform() {
    TokenType tmp;
    bool isQuoteWithAtom = false;
    int num_of_insertion = 0;
    int num_of_insertion_quote = 0;
    if ( m_result_expr_.size() > 1 ) { // not atom
      for ( int i = 0 ; i < m_result_expr_.size() ; i++ ) { // Insert "nil" - RIGHT-PAREN
        if ( m_result_expr_[i].type == "RIGHT-PAREN" ) {
          if ( IsOkToInsertNil( i - 1 ) ) {
            tmp.token = ".";
            tmp.type = "DOT";
            m_result_expr_.insert( m_result_expr_.begin() + i++, tmp );
            tmp.token = "nil_node";
            tmp.type = "NIL";
            m_result_expr_.insert( m_result_expr_.begin() + i++, tmp );
          } // if

        } // if
      } // end for

      for ( int i = 0 ; i < m_result_expr_.size() ; i++ ) { // Insert "nil" - QUOTE
        int index = 0;
        if ( m_result_expr_[i].type == "QUOTE" ) {
          int num_of_quote = 1; // consecutive quote

          while ( m_result_expr_[++i].type == "QUOTE" ) num_of_quote++;

          if ( m_result_expr_[i].type == "LEFT-PAREN" ) { // "('"
            index = i + 1;

            int level = 1;
            for ( ; level != 0 ; index++ ) {
              if ( m_result_expr_[index].type == "RIGHT-PAREN" ) level--;
              else if ( m_result_expr_[index].type == "LEFT-PAREN" ) level++;
            } // end for

            if ( index < m_result_expr_.size() ) {
              for ( int k = 0 ; k < num_of_quote ; k++ ) {
                tmp.token = ".";
                tmp.type = "DOT";
                m_result_expr_.insert( m_result_expr_.begin() + index++, tmp );
                tmp.token = "nil_node";
                tmp.type = "NIL";
                m_result_expr_.insert( m_result_expr_.begin() + index++, tmp );
              } // end for
            } // if
            else {
              for ( int k = 0 ; k < num_of_quote ; k++ ) {
                tmp.token = ".";
                tmp.type = "DOT";
                m_result_expr_.push_back( tmp );
                tmp.token = "nil_node";
                tmp.type = "NIL";
                m_result_expr_.push_back( tmp );
              } // end for
            } // else
          } // if
          else { // ' atom
            index = i + 1;
            if ( index < m_result_expr_.size() ) {
              for ( int k = 0 ; k < num_of_quote ; k++ ) {
                tmp.token = ".";
                tmp.type = "DOT";
                m_result_expr_.insert( m_result_expr_.begin() + index++, tmp );
                tmp.token = "nil_node";
                tmp.type = "NIL";
                m_result_expr_.insert( m_result_expr_.begin() + index++, tmp );
              } // end for
            } // if
            else {
              for ( int k = 0 ; k < num_of_quote ; k++ ) {
                tmp.token = ".";
                tmp.type = "DOT";
                m_result_expr_.push_back( tmp );
                tmp.token = "nil_node";
                tmp.type = "NIL";
                m_result_expr_.push_back( tmp );
              } // end for
            } // else
          } // else

        } // if
      } // end for

      for ( int i = 0 ; i < m_result_expr_.size() ; i++ ) { // erase ")"
        if ( m_result_expr_[i].type == "RIGHT-PAREN" )
          m_result_expr_.erase( m_result_expr_.begin() + i-- );
      } // end for

    } // if
  } // end Transform()

  // condition check for parse //

  bool IsDigit( char ch ) {
    return ( '0' <= ch && ch <= '9' );
  } // end IsDigit()

  bool IsInt( string token ) {
    int index = 0;
    int num_of_digit = 0, num_of_sign = 0;

    for ( ; index < token.size() ; index++ ) {
      if ( token[index] == '+' || token[index] == '-' ) num_of_sign++;
      else if ( ! IsDigit( token[index] ) ) return false;
      else num_of_digit++;

    } // end for

    if ( num_of_digit > 0 ) {
      if ( num_of_sign == 0 ) return true;
      else if ( num_of_sign == 1 ) {
        if ( token[0] == '+' || token[0] == '-' ) return true;
        else return false;
      } // else if
      else return false;
    } // if
    else return false;
  } // end IsInt()

  bool IsFloat( string token ) {
    int index = 0;
    int num_of_digit = 0, num_of_sign = 0, num_of_dot = 0;

    for ( ; index < token.size() ; index++ ) {
      if ( token[index] == '.' ) num_of_dot++;
      else if ( token[index] == '+' || token[index] == '-' ) num_of_sign++;
      else if ( ! IsDigit( token[index] ) ) return false;
      else num_of_digit++;
    } // end for

    if ( num_of_digit > 0 && num_of_dot <= 1 ) {
      if ( num_of_sign == 0 ) return true;
      else if ( num_of_sign == 1 ) {
        if ( token[0] == '+' || token[0] == '-' ) return true;
        else return false;
      } // else if
      else return false;
    } // if
    else return false;
  } // end IsFloat()

  bool IsString( string token ) {
    return token[0] == '\"';
  } // end IsString()

  bool IsNil( string token ) {
    return ( token == "nil" || token == "#f" || token == "()" );
  } // end IsNil()

  bool IsT( string token ) {
    return ( token == "t" || token == "#t" );
  } // end IsT()

  bool IsLeftParen( string token ) {
    return token == "(";
  } // end IsLeftParen()

  bool IsRightParen( string token ) {
    return token == ")";
  } // end IsRightParen()

  bool IsDot( string token ) {
    return token == ".";
  } // end IsDot()

  bool IsQuote( string token ) {
    return token == "'"; // || token == "quote"
  } // end IsQuote()

  bool IsAtom( TokenType & token_node ) {
    // <ATOM>  ::= INT | FLOAT | STRING |
    //             NIL | LEFT-PAREN RIGHT-PAREN | T | SYMBOL
    if ( token_node.type == "INT" ) return true;
    else if ( token_node.type == "FLOAT" ) return true;
    else if ( token_node.type == "STRING" ) return true;
    else if ( token_node.type == "NIL" ) return true;
    else if ( token_node.type == "T" ) return true;
    else if ( token_node.type == "SYMBOL" ) return true;
    else return false;
  } // end IsAtom()

  void TypeCheck( TokenType & token_node ) {
    // check token type and assign its type to the token
    // '(' | ')' | ''' | '"' | ';' | '.'
    if ( IsInt( token_node.token ) ) token_node.type = "INT";
    else if ( IsFloat( token_node.token ) ) token_node.type = "FLOAT";
    else if ( IsString( token_node.token ) ) token_node.type = "STRING";
    else if ( IsNil( token_node.token ) ) token_node.type = "NIL";
    else if ( IsT( token_node.token ) ) token_node.type = "T";
    else if ( IsLeftParen( token_node.token ) ) token_node.type = "LEFT-PAREN";
    else if ( IsRightParen( token_node.token ) ) token_node.type = "RIGHT-PAREN";
    else if ( IsDot( token_node.token ) ) token_node.type = "DOT";
    else if ( IsQuote( token_node.token ) ) token_node.type = "QUOTE";
    else token_node.type = "SYMBOL";
  } // end TypeCheck()

public:
  int ReadSexp() { // read and parse a input string
    // <S-exp> ::= <ATOM> |
    //             LEFT-PAREN <S-exp> { <S-exp> } [ DOT <S-exp> ] RIGHT-PAREN |
    //             QUOTE <S-exp>
    // return 0 => syntax correct
    // return 1 => ERROR (unexpected token) : atom or '(' expected when token at Line X Column Y is >>...<<
    // return 2 => ERROR (unexpected token) : ')' expected when token at Line X Column Y is >>...<<
    // return 3 => ERROR (no closing quote) : END-OF-LINE encountered at Line X Column Y
    // return 4 => ERROR (no more input) : END-OF-FILE encountered
    // return 5 => DOT
    // return 6 => RIGHT-PAREN
    int return_no = 0;
    m_cur_token_node_ = m_scanner_.GetToken();

    if ( m_cur_token_node_.token == "END-OF-LINE" ) return 3;
    else if ( m_cur_token_node_.token == "END-OF-FILE" ) return 4;

    TypeCheck( m_cur_token_node_ );

    if ( IsAtom( m_cur_token_node_ ) ) { // <ATOM>
      m_result_expr_.push_back( m_cur_token_node_ );
      return 0;
    } // if
    else if ( m_cur_token_node_.type == "LEFT-PAREN" ) {
      // LEFT-PAREN <S-exp> { <S-exp> } [ DOT <S-exp> ] RIGHT-PAREN
      m_result_expr_.push_back( m_cur_token_node_ );

      return_no = ReadSexp();
      if ( return_no == 0 ) { // <S-exp>

        do {
          return_no = ReadSexp();
        } while ( return_no == 0 ) ; // { <S-exp> }

        if ( return_no == 1 ) return 1; // error_no_1
        else if ( return_no == 2 ) return 2; // error_no_2
        else if ( return_no == 3 ) return 3; // END-OF-LINE
        else if ( return_no == 4 ) return 4; // END-OF-FILE

        if ( m_cur_token_node_.type == "DOT" ) { // [ DOT <S-exp> ]
          m_result_expr_.push_back( m_cur_token_node_ );

          return_no = ReadSexp();
          if ( return_no == 0 ) ;
          else if ( return_no == 2 ) return 2; // error_no_2
          else if ( return_no == 3 ) return 3; // END-OF-LINE
          else if ( return_no == 4 ) return 4; // END-OF-FILE
          else { // error_no_1 ( 1 5 6 )
            return 1;
          } // else

          m_cur_token_node_ = m_scanner_.GetToken();
          TypeCheck( m_cur_token_node_ );
        } // if

        if ( m_cur_token_node_.type == "RIGHT-PAREN" ) {
          m_result_expr_.push_back( m_cur_token_node_ );
          return 0;
        } // if
        else { // error_no_2
          return 2;
        } // else

      } // if
      else if ( return_no == 2 ) return 2; // error_no_2
      else if ( return_no == 3 ) return 3; // END-OF-LINE
      else if ( return_no == 4 ) return 4; // END-OF-FILE
      else { // error_no_1 ( 1 5 6 )
        return 1;
      } // else
    } // else if
    else if ( m_cur_token_node_.type == "QUOTE" ) { // QUOTE <S-exp>
      m_result_expr_.push_back( m_cur_token_node_ ); // vector push

      return_no = ReadSexp();
      if ( return_no == 0 ) return 0;
      else if ( return_no == 2 ) return 2; // error_no_2
      else if ( return_no == 3 ) return 3; // END-OF-LINE
      else if ( return_no == 4 ) return 4; // END-OF-FILE
      else { // error_no_1 ( 1 5 6 )
        return 1;
      } // else
    } // else if
    else if ( ! m_result_expr_.empty() && m_cur_token_node_.type == "DOT" )
      return 5; // case: { <S-exp> } [ DOT <S-exp> ]
    else if ( ! m_result_expr_.empty() && m_cur_token_node_.type == "RIGHT-PAREN" )
      return 6; // case: { <S-exp> } RIGHT-PAREN
    else { // error_no_1
      return 1;
    } // else
  } // end ReadSexp()

  void PrintErrorMsg( int error_no ) {
    if ( error_no == 1 ) {
      cout << "ERROR (unexpected token) : atom or '(' expected when token at Line "
           << m_cur_token_node_.line << " Column " << m_cur_token_node_.column
           << " is >>" << m_cur_token_node_.token << "<<\n";
      SkipLine();
    } // if
    else if ( error_no == 2 ) {
      cout << "ERROR (unexpected token) : ')' expected when token at Line "
           << m_cur_token_node_.line << " Column " << m_cur_token_node_.column
           << " is >>" << m_cur_token_node_.token << "<<\n";
      SkipLine();
    } // else if
    else if ( error_no == 3 ) {
      cout << "ERROR (no closing quote) : END-OF-LINE encountered at Line "
           << m_cur_token_node_.line << " Column " << m_cur_token_node_.column  << '\n';
    } // else if
    else if ( error_no == 4 ) {
      cout << "ERROR (no more input) : END-OF-FILE encountered";
    } // else if
    else cout << "there is no error\n";
  } // end PrintErrorMsg()

  bool IsExit() {
    if ( m_root_for_parse_tree_ ) {
      if ( ! m_root_for_parse_tree_->item.token.empty() ) return false;
      if ( ! m_root_for_parse_tree_->left || m_root_for_parse_tree_->left->item.token != "exit" )
        return false;
      if ( ! m_root_for_parse_tree_->right || m_root_for_parse_tree_->right->item.type != "NIL" )
        return false;
      return true;
    } // if
    else return false;
  } // end IsExit()

  bool IsEOF( int error_no ) { // exit or EOF
    return error_no == 4;
  } // end IsEOF()

  void BuildTree() {
    Transform();
    bool connect = false;
    m_root_for_parse_tree_ = Insert( connect );
  } // end BuildTree()

  NodeType* GetRoot() {
    return m_root_for_parse_tree_;
  } // end GetRoot()

  void ResetScanner() {
    m_scanner_.Reset();
  } // end ResetScanner()

  void ResetParser() {
    m_result_expr_.clear();
  } // end ResetParser()

} ; // end class Parser

class Evaluator {
private:
  // bool m_pop_;
  int m_level_;
  string m_cur_status_;
  PrettyPrinter m_pretty_printer_;
  NodeType* m_root_for_parse_tree_;
  NodeType* m_root_for_evaluate_tree_;
  vector<string> m_internal_functions_;
  map<string, NodeType*> m_user_defined_;
  
  vector< map<string, NodeType*> > m_let_defs_;
  vector< map<string, NodeType*> > m_lambda_defs_;
  vector< map<string, NodeType*> > m_define_defs_;
  vector< map<string, NodeType*> > m_local_defs_;

  bool IsAtom( TokenType token ) {
    if ( token.type == "INT" || token.type == "FLOAT" ||
         token.type == "STRING" || token.type == "NIL" || token.type == "T" ) return true;
    else return false;
  } // end IsAtom()

  bool IsSymbol( TokenType token ) {
    if ( token.type == "SYMBOL" || token.type == "QUOTE" ) return true;
    else return false;
  } // end IsSymbol()

  bool IsInernalFunction( string name ) {
    vector<string>::iterator begin = m_internal_functions_.begin();
    vector<string>::iterator end = m_internal_functions_.end();

    vector<string>::iterator itr;
    for ( itr = begin ; itr != end ; itr++ ) {
      if ( *itr == name ) return true;
    } // end for

    return false;
  } // end IsInernalFunction()

  bool IsUserDefined( string name ) {
    map<string, NodeType*>::iterator begin = m_user_defined_.begin();
    map<string, NodeType*>::iterator end = m_user_defined_.end();

    map<string, NodeType*>::iterator itr;
    for ( itr = begin ; itr != end ; itr++ ) { // check if the key in the map == name
      if ( itr->first == name ) return true;
    } // end for

    return false;
  } // end IsUserDefined()

  bool IsLetDef( string name ) {
    map<string, NodeType*>::iterator itr;

    if ( m_let_defs_.empty() ) return false;

    // check if the key in the map == name
    for ( itr = m_let_defs_.back().begin() ; itr != m_let_defs_.back().end() ; itr++ ) {
      if ( itr->first == name ) return true;
    } // end for

    return false;
  } // end IsLetDef()

  bool IsLambdaDef( string name ) {
    map<string, NodeType*>::iterator itr;

    if ( m_lambda_defs_.empty() ) return false;

    // check if the key in the map == name
    for ( itr = m_lambda_defs_.back().begin() ; itr != m_lambda_defs_.back().end() ; itr++ ) {
      if ( itr->first == name ) return true;
    } // end for

    return false;
  } // end IsLambdaDef()
  
  bool IsDefineDef( string name ) {
    map<string, NodeType*>::iterator itr;

    if ( m_define_defs_.empty() ) return false;

    // check if the key in the map == name
    for ( itr = m_define_defs_.back().begin() ; itr != m_define_defs_.back().end() ; itr++ ) {
      if ( itr->first == name ) return true;
    } // end for

    return false;
  } // end IsDefineDef()

  bool IsLocalDef( string name, NodePtr& value ) {
    vector< map<string, NodeType*> >::iterator itr1;
    map<string, NodeType*>::iterator itr2;

    if ( m_local_defs_.empty() ) return false;

    for ( itr1 = m_local_defs_.end() ; itr1-- != m_local_defs_.begin() ; ) {
      for ( itr2 = itr1->begin() ; itr2 != itr1->end() ; itr2++ ) {
        if ( itr2->first == name ) {
          value = itr2->second;
          return true;
        } // if

      } // end for
    } // end for

    return false;
  } // end IsLocalDef()
  
  bool IsRecentLocalDef( string name ) {
    map<string, NodeType*>::iterator itr;
    
    if ( m_local_defs_.empty() ) return false;
    
    // check if the key in the map == name
    for ( itr = m_local_defs_.back().begin() ; itr != m_local_defs_.back().end() ; itr++ ) {
      if ( itr->first == name ) return true;
    } // end for
    
    return false;
  } // end IsRecentLocalDef()

  bool IsLeaf( NodeType* node ) {
    return node && ( ! node->left && ! node->right );
  } // end IsLeaf()

  // bool IsLambda( NodeType* node ) { // is lambda def + use
    // return node && node->left && node->left->left && node->left->left->item.token == "lambda";
  // } // end IsLambda()

  bool IsLambda( NodeType* node ) { // is lambda def
    return node && node->left && node->left->item.token == "lambda" && node->left->item.is_procedure;
  } // end IsLambda()

  bool IsUserDefinedFunc( NodeType* node ) {
    return node && node->left && node->left->left
           && IsUserDefined( node->left->left->item.token ) && node->left->left->item.is_procedure;
  } // end IsUserDefinedFunc()

  bool IsDuplicatedSym( vector<string>& v, string sym ) {
    for ( int i = 0 ; i < v.size() ; i++ ) {
      if ( v[i] == sym ) return true;
    } // end for
    
    return false;
  } // end IsDuplicatedSym()

  bool IsNotAPureList( NodeType* node ) {
    if ( node ) {
      // go right
      if ( IsLeaf( node->right ) ) {
        if ( node->right == g_nil ) return false;
        else return true; // with DOT in it
      } // if
      else return IsNotAPureList( node->right );
    } // if
    else return false;
  } // end IsNotAPureList()

  bool IsCorrectDefineFormat( NodeType* node ) {
    vector<string> sym_list;
    NodeType* defined_node = NULL;

    defined_node = GetArgument( node );
    if ( defined_node->item.type == "SYMBOL" && ! IsInernalFunction( defined_node->item.token ) ) { // case 1
      int num_of_args = 1;
      while ( GetArgument( node ) ) num_of_args++;

      if ( num_of_args == 2 ) return true;
      else return false;
    } // if
    else if ( defined_node->left && defined_node->right ) { // case 2
      NodeType* var = NULL;

      var = defined_node->left; // first arg definition
      if ( var->item.type == "SYMBOL" && ! IsInernalFunction( var->item.token ) ) {
        while ( var ) { // arg fetched & no error occurs
          // can only define symbol & this symbol can!!! be system primitives
          if ( var->item.type == "SYMBOL" && ! IsInernalFunction( var->item.token ) ) {
            if ( IsDuplicatedSym( sym_list, var->item.token ) ) return false;
            else sym_list.push_back( var->item.token );
          } // if
          else return false;

          var = GetArgument( defined_node );
        } // end while

        return true;
      } // if
      else return false;
    } // else if
    else return false;
  } // end IsCorrectDefineFormat()

  bool IsCorrectCondFormat( NodeType* node ) { // check if the args are non-list
    NodeType* arg = NULL;

    arg = GetArgument( node );
    while ( arg ) { // arg fetched
      if ( arg->left && arg->right && ! IsNotAPureList( arg ) && GetArgument( arg ) ) ;
      else return false;

      arg = GetArgument( node );
    } // end while

    return true;
  } // end IsCorrectCondFormat()

  bool IsCorrectLetFormat( NodeType* node ) {
    int num_of_args = 0;
    vector<string> sym_list;
    NodeType* var = NULL;
    NodeType* def = NULL;
    NodeType* defined_node = NULL;

    defined_node = GetArgument( node );

    if ( defined_node->item.token != "" && defined_node != g_nil ) return false; // ( let () 5 )

    def = defined_node->left;
    while ( def ) { // arg fetched
      var = def->left;
      // calc num of args
      while ( GetArgument( def ) ) num_of_args++;
      
      if ( num_of_args != 1 ) return false;
      
      // can only define symbol & this symbol can NOT be system primitives
      if ( var->item.type == "SYMBOL" && ! IsInernalFunction( var->item.token ) ) {
        if ( IsDuplicatedSym( sym_list, var->item.token ) ) return false;
        else sym_list.push_back( var->item.token );
      } // if
      else return false;

      num_of_args = 0;
      def = GetArgument( defined_node );
    } // end while

    return true;
  } // end IsCorrectLetFormat()

  bool IsCorrectLambdaFormat( NodeType* node ) {
    vector<string> sym_list;
    NodeType* var = NULL;
    NodeType* defined_node = NULL;

    defined_node = GetArgument( node );

    if ( defined_node->item.token != "" && defined_node != g_nil ) return false; // ( lambda () 5 )

    var = defined_node->left; // first arg definition
    while ( var ) { // arg fetched & no error occurs
      // can only define symbol & this symbol can!!! be system primitives
      if ( var->item.type == "SYMBOL" && ! IsInernalFunction( var->item.token ) ) {
        if ( IsDuplicatedSym( sym_list, var->item.token ) ) return false;
        else sym_list.push_back( var->item.token );
      } // if
      else return false;

      var = GetArgument( defined_node );
    } // end while

    return true;
  } // end IsCorrectLambdaFormat()

  bool IsCorrectNumOfArgs( NodeType* node, string function_name ) {
    int num_of_args = 0;

    while ( GetArgument( node ) ) num_of_args++;

    if ( function_name == "cons" ) return num_of_args == 2;
    else if ( function_name == "list" ) return num_of_args >= 0;
    else if ( function_name == "quote" || function_name == "'" ) return num_of_args == 1;
    else if ( function_name == "define" ) return num_of_args >= 2;
    else if ( function_name == "let" ) return num_of_args >= 2;
    else if ( function_name == "lambda" ) return num_of_args >= 2;
    else if ( function_name == "car" ) return num_of_args == 1;
    else if ( function_name == "cdr" ) return num_of_args == 1;
    else if ( function_name == "atom?" ) return num_of_args == 1;
    else if ( function_name == "pair?" ) return num_of_args == 1;
    else if ( function_name == "list?" ) return num_of_args == 1;
    else if ( function_name == "null?" ) return num_of_args == 1;
    else if ( function_name == "integer?" ) return num_of_args == 1;
    else if ( function_name == "real?" ) return num_of_args == 1;
    else if ( function_name == "number?" ) return num_of_args == 1;
    else if ( function_name == "string?" ) return num_of_args == 1;
    else if ( function_name == "boolean?" ) return num_of_args == 1;
    else if ( function_name == "symbol?" ) return num_of_args == 1;
    else if ( function_name == "+" ) return num_of_args >= 2;
    else if ( function_name == "-" ) return num_of_args >= 2;
    else if ( function_name == "*" ) return num_of_args >= 2;
    else if ( function_name == "/" ) return num_of_args >= 2;
    else if ( function_name == "not" ) return num_of_args == 1;
    else if ( function_name == "and" ) return num_of_args >= 2;
    else if ( function_name == "or" ) return num_of_args >= 2;
    else if ( function_name == ">" ) return num_of_args >= 2;
    else if ( function_name == ">=" ) return num_of_args >= 2;
    else if ( function_name == "<" ) return num_of_args >= 2;
    else if ( function_name == "<=" ) return num_of_args >= 2;
    else if ( function_name == "=" ) return num_of_args >= 2;
    else if ( function_name == "string-append" ) return num_of_args >= 2;
    else if ( function_name == "string>?" ) return num_of_args >= 2;
    else if ( function_name == "string<?" ) return num_of_args >= 2;
    else if ( function_name == "string=?" ) return num_of_args >= 2;
    else if ( function_name == "eqv?" ) return num_of_args == 2;
    else if ( function_name == "equal?" ) return num_of_args == 2;
    else if ( function_name == "begin" ) return num_of_args >= 1;
    else if ( function_name == "if" ) return num_of_args == 2 || num_of_args == 3;
    else if ( function_name == "cond" ) return num_of_args >= 1;
    else if ( function_name == "clean-environment" ) return num_of_args == 0;
    else if ( function_name == "exit" ) return num_of_args == 0;
    else return false;
  } // end IsCorrectNumOfArgs()

  bool IsCorrectType( NodeType* node, string function_name ) {
    if ( node && node->left && node->left->left && IsUserDefined( node->left->left->item.token ) &&
         m_user_defined_[node->left->left->item.token]->item.is_procedure )
      node = node->left->left;

    if ( function_name == "car" || function_name == "cdr" ) {
      return node->left && node->right;
    } // if
    else if ( function_name == "+" || function_name == "-" || function_name == "*" ||
              function_name == "/" || function_name == ">" || function_name == ">=" ||
              function_name == "<" || function_name == "<=" || function_name == "=" ) {
      return node->item.type == "INT" || node->item.type == "FLOAT";
    } // else if
    else if ( function_name == "string-append" || function_name == "string>?" ||
              function_name == "string<?" || function_name == "string=?" ) {
      return node->item.type == "STRING";
    } // else if

    return false;
  } // end IsCorrectType()

  NodeType* GetArgument( NodePtr & node ) {
    if ( node && node->right && node->right->left ) {
      node = node->right; // node = parten node of arg
      return node->left;
    } // if
    else return NULL;
  } // end GetArgument()

  NodeType* ExecuteFunction( NodeType* ori_node, NodeType* value ) {
    NodeType* node = NULL;
    if ( value ) {
      node = new NodeType;
      node->left = value;
      node->right = ori_node->right;
    } // if
    else node = ori_node;

    if ( node->left->item.token == "define" ) {
      if ( IsCorrectNumOfArgs( node, node->left->item.token ) && IsCorrectDefineFormat( node ) ) {
        return Define( node );
      } // if
      else {
        cout << "ERROR (DEFINE format) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node );
        throw "ERROR";
      } // else
    } // if
    else if ( node->left->item.token == "let" ) {
      if ( IsCorrectNumOfArgs( node, node->left->item.token ) && IsCorrectLetFormat( node ) ) {
        return Let( node );
      } // if
      else {
        cout << "ERROR (LET format) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node );
        throw "ERROR";
      } // else
    } // else if
    else if ( node->left->item.token == "lambda" ) { // lambda def
      if ( IsCorrectNumOfArgs( node, node->left->item.token ) && IsCorrectLambdaFormat( node ) ) {
        return Lambda( node );
      } // if
      else {
        cout << "ERROR (LAMBDA format) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node );
        throw "ERROR";
      } // else
    } // else if
    else if ( node->left->left && node->left->left->item.token == "lambda" ) { // lambda def + use
      if ( IsCorrectNumOfArgs( node->left, node->left->left->item.token )
           && IsCorrectLambdaFormat( node->left ) ) {
        return Lambda( node );
      } // if
      else {
        cout << "ERROR (LAMBDA format) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // else if
    else if ( node->left->item.token == "cond" ) {
      if ( IsCorrectNumOfArgs( node, node->left->item.token ) && IsCorrectCondFormat( node ) ) {
        return Cond( node );
      } // if
      else {
        cout << "ERROR (COND format) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node );
        throw "ERROR";
      } // else
    } // if
    else {
      if ( IsCorrectNumOfArgs( node, node->left->item.token ) ) {
        if ( node->left->item.token == "cons" ) return Cons( node );
        else if ( node->left->item.token == "list" ) return List( node );
        else if ( node->left->item.token == "quote" || node->left->item.token == "'" ) return Quote( node );
        else if ( node->left->item.token == "car" ) return Car( node );
        else if ( node->left->item.token == "cdr" ) return Cdr( node );
        else if ( node->left->item.token == "atom?" ) return AtomOrNot( node );
        else if ( node->left->item.token == "pair?" ) return PairOrNot( node );
        else if ( node->left->item.token == "list?" ) return ListOrNot( node );
        else if ( node->left->item.token == "null?" ) return NullOrNot( node );
        else if ( node->left->item.token == "integer?" ) return IntegerOrNot( node );
        else if ( node->left->item.token == "real?" ) return RealOrNot( node );
        else if ( node->left->item.token == "number?" ) return NumberOrNot( node );
        else if ( node->left->item.token == "string?" ) return StringOrNot( node );
        else if ( node->left->item.token == "boolean?" ) return BooleanOrNot( node );
        else if ( node->left->item.token == "symbol?" ) return SymbolOrNot( node );
        else if ( node->left->item.token == "+" ) return Plus( node );
        else if ( node->left->item.token == "-" ) return Minus( node );
        else if ( node->left->item.token == "*" ) return Multiply( node );
        else if ( node->left->item.token == "/" ) return Divide( node );
        else if ( node->left->item.token == "not" ) return Not( node );
        else if ( node->left->item.token == "and" ) return And( node );
        else if ( node->left->item.token == "or" ) return Or( node );
        else if ( node->left->item.token == ">" ) return Greater( node );
        else if ( node->left->item.token == ">=" ) return GreaterOrEqual( node );
        else if ( node->left->item.token == "<" ) return Less( node );
        else if ( node->left->item.token == "<=" ) return LessOrEqual( node );
        else if ( node->left->item.token == "=" ) return Equal( node );
        else if ( node->left->item.token == "string-append" ) return StringAppend( node );
        else if ( node->left->item.token == "string>?" ) return StringGreaterOrNot( node );
        else if ( node->left->item.token == "string<?" ) return StringLessOrNot( node );
        else if ( node->left->item.token == "string=?" ) return StringEqualOrNot( node );
        else if ( node->left->item.token == "eqv?" ) return EqvOrNot( node );
        else if ( node->left->item.token == "equal?" ) {
          NodeType* first_arg = NULL;
          NodeType* second_arg = NULL;

          first_arg = Evaluate( GetArgument( node ) );
          if ( first_arg ) { // first arg fetched && no error occurs
            second_arg = Evaluate( GetArgument( node ) );
            if ( second_arg ) // second arg fetched && no error occurs
              return EqualOrNot( first_arg, second_arg );
            else {
              cout << "ERROR (unbound parameter) : ";
              m_pretty_printer_.SetPrintProcedure( false );
              m_pretty_printer_.PrettyPrint( node->left );
              throw "ERROR";
            } // else
          } // if
          else {
            cout << "ERROR (unbound parameter) : ";
            m_pretty_printer_.SetPrintProcedure( false );
            m_pretty_printer_.PrettyPrint( node->left );
            throw "ERROR";
          } // else
        } // else if
        else if ( node->left->item.token == "begin" ) return Begin( node );
        else if ( node->left->item.token == "if" ) return If( node );
        else if ( node->left->item.token == "clean-environment" ) return CleanEnvironment();
        else if ( node->left ->item.token == "exit" ) throw "EXIT";
        else return NULL;
      } // if
      else {
        cout << "ERROR (incorrect number of arguments) : " << node->left->item.token << '\n';
        throw "ERROR";
      } // else
    } // else
  } // end ExecuteFunction()

  NodeType* Cons( NodeType* node ) { // cons(2)
    NodeType* evaluated_result = new NodeType;

    evaluated_result->left = Evaluate( GetArgument( node ) );
    if ( evaluated_result->left ) { // first arg fetched & no error occurs
      evaluated_result->right = Evaluate( GetArgument( node ) );
      if ( evaluated_result->right ) // second arg fetched & no error occurs
        return evaluated_result;
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else
  } // end Cons()

  NodeType* List( NodeType* node ) { // list(>=0)
    NodeType* evaluated_result = new NodeType;

    evaluated_result->left = GetArgument( node );
    if ( evaluated_result->left ) {
      evaluated_result->left = Evaluate( evaluated_result->left );
      if ( evaluated_result->left ) {
        evaluated_result->right = List( node );
        return evaluated_result;
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // if
    else return g_nil;
  } // end List()

  NodeType* Quote( NodeType* node ) { // quote(1)
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );

    return evaluated_result;
  } // end Quote()

  NodeType* Define( NodeType* node ) { // define(2)
    NodeType* var = NULL;
    NodeType* value = NULL;

    var = GetArgument( node );
    if ( var ) { // arg fetched & no error occurs
      if ( var->item.type == "SYMBOL" ) { // define symbol
        value = GetArgument( node );
        if ( value ) {
          value = Evaluate( value );
          if ( value ) { // arg fetched & no error occurs
            if ( value && value->left 
                 && value->left->item.token == "lambda" && value->left->item.is_procedure )
              value->item.is_procedure = true;
            m_user_defined_[var->item.token] = value;
            cout << var->item.token << " defined" << '\n';
          } // if
          else {
            cout << "ERROR (no return value) : ";
            m_pretty_printer_.SetPrintProcedure( false );
            m_pretty_printer_.PrettyPrint( node->left );
            throw "ERROR";
          } // else
        } // if
      } // if
      else { // define function
        node->item.is_procedure = true;
        node->left->left->item.is_procedure = true;
        m_user_defined_[var->left->item.token] = node;
        cout << var->left->item.token << " defined" << '\n';

        while ( GetArgument( node ) ) ; // get the first expr

        if ( node && node->left && node->left->left &&
             node->left->left->left && node->left->left->left->item.token == "lambda" )
          var->left->item.is_lambda = true;
        else if ( node && node->left && node->left->left && node->left->left->item.token == "lambda" )
          var->left->item.is_lambda = true;

      } // else

    } // if

    throw "SKIP";
  } // end Define()

  NodeType* FunctionCall( NodeType* node, NodeType* value ) {
    NodeType* evaluated_node = new NodeType;
    evaluated_node->left = value;
    evaluated_node->right = node->right;

    NodeType* arg = NULL;
    NodeType* var = NULL;

    NodeType* var_def_node = NULL;
    NodeType* func_def_node = evaluated_node->left;
    NodeType* func_name = func_def_node->left->left;

    NodeType* evaluated_result = NULL;
    
    string ori_status = "";
    map<string, NodeType*> define_def;

    ori_status = m_cur_status_; // save the original status

    // arg number check
    NodeType* tmp = evaluated_node;
    var_def_node = func_def_node->left;
    var = GetArgument( var_def_node );
    while ( var ) { // arg fetched & no error occurs

      arg = GetArgument( tmp );
      if ( arg ) ;
      else { // param num > arg num
        cout << "ERROR (incorrect number of arguments) : " << func_name->item.token << '\n';
        throw "ERROR";
      } // else

      var = GetArgument( var_def_node );
    } // end while

    if ( GetArgument( tmp ) ) { // arg num > param num
      cout << "ERROR (incorrect number of arguments) : " << func_name->item.token << '\n';
      throw "ERROR";
    } // if

    // pass arg
    // if ( ori_status == "" && ori_status != "lambda" && ori_status != "let" )
    m_cur_status_ = "pass-args";

    // cout << "start: " << m_cur_status_ << endl;

    var_def_node = func_def_node->left;
    var = GetArgument( var_def_node );
    while ( var ) { // arg fetched & no error occurs

      arg = GetArgument( evaluated_node );
      if ( arg ) {
        NodeType* tmpp = arg;
        arg = Evaluate( arg );

        // cout << func_def_node->left->left->item.token << '\n';

        // cout << var->item.token << " = " << arg->item.token << "(from " << tmpp->item.token << ")\n";

        if ( arg ) define_def[var->item.token] = arg;
        else {
          cout << "ERROR (unbound parameter) : ";
          m_pretty_printer_.SetPrintProcedure( false );
          m_pretty_printer_.PrettyPrint( evaluated_node->left );
          throw "ERROR";
        } // else
      } // if

      var = GetArgument( var_def_node );
    } // end while

    m_define_defs_.push_back( define_def );
    m_local_defs_.push_back( define_def );

    m_cur_status_ = "function-call";

    evaluated_result = Begin( func_def_node );

    m_cur_status_ = ori_status; // restore the original status
    
    m_define_defs_.pop_back();
    m_local_defs_.pop_back();


    return evaluated_result;
  } // end FunctionCall()

  NodeType* Let( NodeType* node ) { // let(2)
    NodeType* def = NULL;
    NodeType* var = NULL;
    NodeType* value = NULL;
    NodeType* defined_node = NULL;
    NodeType* evaluated_result = NULL;
    map<string, NodeType*> let_def;

    string ori_status = "";
    ori_status = m_cur_status_; // save the original status
    m_cur_status_ = "let";

    // local definition
    defined_node = GetArgument( node );
    def = defined_node->left; // first local definition
    while ( def ) { // arg fetched & no error occurs
      var = def->left;

      value = GetArgument( def );
      if ( value ) {
        value = Evaluate( value );

        if ( value ) { // arg fetched & no error occurs
          let_def[var->item.token] = value;
          // cout << var->item.token << " = " << value->item.token << '\n';
        } // if
        else {
          cout << "ERROR (no return value) : ";
          m_pretty_printer_.SetPrintProcedure( false );
          m_pretty_printer_.PrettyPrint( def->left );
          throw "ERROR";
        } // else
      } // if

      def = GetArgument( defined_node );
    } // end while

    m_let_defs_.push_back( let_def );
    m_local_defs_.push_back( let_def );

    m_cur_status_ = "let";

    evaluated_result = Begin( node );

    m_cur_status_ = ori_status; // restore the original status
    
    m_let_defs_.pop_back();
    m_local_defs_.pop_back();

    return evaluated_result;
  } // end Let()

  NodeType* Lambda( NodeType* node ) { // lambda(>=2)
    NodeType* evaluated_result = NULL;

    if ( node->left->item.token == "lambda" ) evaluated_result = node; // case 1 => def
    else { // case 2 => def + use
      NodeType* arg = NULL;
      NodeType* var = NULL;
      NodeType* defined_node = NULL;
      NodeType* lambda_def_node = node->left;

      map<string, NodeType*> lambda_def;

      string ori_status = "";
      ori_status = m_cur_status_; // save the original status

      // arg number check
      NodeType* tmp1 = lambda_def_node;
      NodeType* tmp2 = node;
      defined_node = GetArgument( tmp1 );
      var = defined_node->left; // first arg definition
      while ( var ) { // arg fetched & no error occurs
        arg = GetArgument( tmp2 );
        if ( arg ) ;
        else { // param num > arg num
          cout << "ERROR (incorrect number of arguments) : lambda" << '\n';
          throw "ERROR";
        } // else

        var = GetArgument( defined_node );
      } // end while

      if ( GetArgument( tmp2 ) ) { // arg num > param num
        cout << "ERROR (incorrect number of arguments) : lambda" << '\n';
        throw "ERROR";
      } // if
      
      // pass args
      m_cur_status_ = "pass-args";

      defined_node = GetArgument( lambda_def_node );
      var = defined_node->left; // first arg definition
      while ( var ) { // arg fetched & no error occurs
        arg = GetArgument( node );
        if ( arg ) {
          NodeType* tmpp = arg;
          arg = Evaluate( arg );
          // cout << "lllamn\n";
          // cout << node->left->item.token << '\n';
          // cout << var->item.token << " = " << arg->item.token << "(from " << tmpp->item.token << ")\n";

          if ( arg ) {
            lambda_def[var->item.token] = arg;
          } // if
          else {
            cout << "ERROR (unbound parameter) : ";
            m_pretty_printer_.SetPrintProcedure( false );
            m_pretty_printer_.PrettyPrint( node->left );
            throw "ERROR";
          } // else
        } // if

        var = GetArgument( defined_node );
      } // end while

      m_cur_status_ = "lambda";

      m_lambda_defs_.push_back( lambda_def );
      m_local_defs_.push_back( lambda_def );

      evaluated_result = Begin( lambda_def_node );

      m_cur_status_ = ori_status; // restore the original status
      
      m_lambda_defs_.pop_back();
      m_local_defs_.pop_back();
    } // else

    return evaluated_result;
  } // end Lambda()

  NodeType* Car( NodeType* node ) { // car(1)
    // the "left part" of a dotted pair
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( IsCorrectType( evaluated_result, "car" ) )
        evaluated_result = evaluated_result->left;
      else {
        cout << "ERROR (car with incorrect argument type) : ";
        m_pretty_printer_.SetPrintProcedure( true );
        m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
        m_pretty_printer_.PrettyPrint( evaluated_result );
        throw "ERROR";
      } // else
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end Car()

  NodeType* Cdr( NodeType* node ) { // cdr(1)
    // the "right part" of a dotted pair

    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( IsCorrectType( evaluated_result, "cdr" ) )
        evaluated_result = evaluated_result->right;
      else {
        cout << "ERROR (cdr with incorrect argument type) : ";
        m_pretty_printer_.SetPrintProcedure( true );
        m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
        m_pretty_printer_.PrettyPrint( evaluated_result );
        throw "ERROR";
      } // else
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end Cdr()

  NodeType* AtomOrNot( NodeType* node ) { // atom?(1)
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( IsAtom( evaluated_result->item ) || evaluated_result->item.type == "SYMBOL"
           || IsLambda( evaluated_result ) || IsUserDefinedFunc( evaluated_result ) )
        evaluated_result = g_true;
      else evaluated_result = g_nil;
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end AtomOrNot()

  NodeType* PairOrNot( NodeType* node ) { // pair?(1)
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( ! IsLambda( evaluated_result ) && ! IsUserDefinedFunc( evaluated_result ) &&
           evaluated_result->left && evaluated_result->right ) evaluated_result = g_true;
      else evaluated_result = g_nil;
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end PairOrNot()

  NodeType* ListOrNot( NodeType* node ) { // list?(1)
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( ! IsLambda( evaluated_result ) && ! IsUserDefinedFunc( evaluated_result ) &&
           evaluated_result->left && evaluated_result->right && ! IsNotAPureList( evaluated_result ) )
        evaluated_result = g_true;
      else evaluated_result = g_nil;
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end ListOrNot()

  NodeType* NullOrNot( NodeType* node ) { // null?(1)
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( evaluated_result == g_nil ) evaluated_result = g_true;
      else evaluated_result = g_nil;
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end NullOrNot()

  NodeType* IntegerOrNot( NodeType* node ) { // integer?(1)
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( evaluated_result->item.type == "INT" ) evaluated_result = g_true;
      else evaluated_result = g_nil;
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end IntegerOrNot()

  NodeType* RealOrNot( NodeType* node ) { // real?(1)
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( evaluated_result->item.type == "INT" || evaluated_result->item.type == "FLOAT" )
        evaluated_result = g_true;
      else evaluated_result = g_nil;
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end RealOrNot()

  NodeType* NumberOrNot( NodeType* node ) { // number?(1)
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( evaluated_result->item.type == "INT" || evaluated_result->item.type == "FLOAT" )
        evaluated_result = g_true;
      else evaluated_result = g_nil;
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end NumberOrNot()

  NodeType* StringOrNot( NodeType* node ) { // string?(1)
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( evaluated_result->item.type == "STRING" ) evaluated_result = g_true;
      else evaluated_result = g_nil;
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end StringOrNot()

  NodeType* BooleanOrNot( NodeType* node ) { // boolean?(1)
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( evaluated_result->item.type == "T" || evaluated_result->item.type == "NIL" )
        evaluated_result = g_true;
      else evaluated_result = g_nil;
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end BooleanOrNot()

  NodeType* SymbolOrNot( NodeType* node ) { // symbol?(1)
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( evaluated_result->item.type == "SYMBOL" 
           || IsLambda( evaluated_result ) || IsUserDefinedFunc( evaluated_result ) )
        evaluated_result = g_true;
      else evaluated_result = g_nil;
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end SymbolOrNot()

  NodeType* Plus( NodeType* node ) { // +(>=2)
    char str[255];
    double result = 0;
    int num_of_args = 0;
    bool is_float = false;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, "+" ) ) {
          if ( evaluated_result->item.type == "FLOAT" ) is_float = true;

          if ( num_of_args == 1 ) result = atof( evaluated_result->item.token.c_str() );
          else result += atof( evaluated_result->item.token.c_str() );
        } // if
        else {
          cout << "ERROR (+ with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    // build a node
    evaluated_result = new NodeType;
    evaluated_result->left = NULL;
    evaluated_result->right = NULL;
    sprintf( str, "%.3f", result );
    string tmp( str );
    evaluated_result->item.token = tmp;
    if ( is_float ) evaluated_result->item.type = "FLOAT";
    else evaluated_result->item.type = "INT";

    return evaluated_result;
  } // end Plus()

  NodeType* Minus( NodeType* node ) { // -(>=2)
    char str[255];
    double result = 0;
    int num_of_args = 0;
    bool is_float = false;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, "-" ) ) {
          if ( evaluated_result->item.type == "FLOAT" ) is_float = true;

          if ( num_of_args == 1 ) result = atof( evaluated_result->item.token.c_str() );
          else result -= atof( evaluated_result->item.token.c_str() );
        } // if
        else {
          cout << "ERROR (- with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else

    } // end while

    // build a node
    evaluated_result = new NodeType;
    evaluated_result->left = NULL;
    evaluated_result->right = NULL;
    sprintf( str, "%.3f", result );
    string tmp( str );
    evaluated_result->item.token = tmp;
    if ( is_float ) evaluated_result->item.type = "FLOAT";
    else evaluated_result->item.type = "INT";

    return evaluated_result;
  } // end Minus()

  NodeType* Multiply( NodeType* node ) { // *(>=2)
    char str[255];
    double result = 0;
    int num_of_args = 0;
    bool is_float = false;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, "*" ) ) {
          if ( evaluated_result->item.type == "FLOAT" ) is_float = true;

          if ( num_of_args == 1 ) result = atof( evaluated_result->item.token.c_str() );
          else if ( is_float ) result = result * atof( evaluated_result->item.token.c_str() );
          else result = ( int ) result * ( int ) atof( evaluated_result->item.token.c_str() );
        } // if
        else {
          cout << "ERROR (* with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    // build a node
    evaluated_result = new NodeType;
    evaluated_result->left = NULL;
    evaluated_result->right = NULL;
    sprintf( str, "%.3f", result );
    string tmp( str );
    evaluated_result->item.token = tmp;
    if ( is_float ) evaluated_result->item.type = "FLOAT";
    else evaluated_result->item.type = "INT";

    return evaluated_result;
  } // end Multiply()

  NodeType* Divide( NodeType* node ) { // /(>=2)
    char str[255];
    double result = 0;
    int num_of_args = 0;
    bool is_float = false;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, "/" ) ) {
          if ( num_of_args != 1 && atof( evaluated_result->item.token.c_str() ) == 0 ) {
            cout << "ERROR (division by zero) : /" << '\n';
            throw "ERROR";
          } // if

          if ( evaluated_result->item.type == "FLOAT" ) is_float = true;

          if ( num_of_args == 1 ) result = atof( evaluated_result->item.token.c_str() );
          else if ( is_float ) result = result / atof( evaluated_result->item.token.c_str() );
          else result = ( int ) result / ( int ) atof( evaluated_result->item.token.c_str() );
        } // if
        else {
          cout << "ERROR (/ with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    // build a node
    evaluated_result = new NodeType;
    evaluated_result->left = NULL;
    evaluated_result->right = NULL;
    sprintf( str, "%.3f", result );
    string tmp( str );
    evaluated_result->item.token = tmp;
    if ( is_float ) evaluated_result->item.type = "FLOAT";
    else evaluated_result->item.type = "INT";

    return evaluated_result;
  } // end Divide()

  NodeType* Not( NodeType* node ) { // not(1)
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // arg fetched & no error occurs
      if ( evaluated_result == g_nil ) evaluated_result = g_true;
      else evaluated_result = g_nil;
    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end Not()

  NodeType* And( NodeType* node ) { // and(>=2)

    NodeType* evaluated_result = NULL;
    NodeType* evaluated_result_of_last_one = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        if ( evaluated_result == g_nil ) return evaluated_result;

        evaluated_result_of_last_one = evaluated_result;
        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound condition) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    return evaluated_result_of_last_one;
  } // end And()

  NodeType* Or( NodeType* node ) { // or(>=2)
    NodeType* evaluated_result = NULL;
    NodeType* evaluated_result_of_last_one = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        if ( evaluated_result != g_nil ) return evaluated_result;

        evaluated_result_of_last_one = evaluated_result;
        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound condition) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    return evaluated_result_of_last_one;
  } // end Or()

  NodeType* Greater( NodeType* node ) { // >(>=2)
    double result = 0;
    int num_of_args = 0;
    bool is_greater = true;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, ">" ) ) {
          if ( num_of_args == 1 ) result = atof( evaluated_result->item.token.c_str() );
          else {
            if ( result > atof( evaluated_result->item.token.c_str() ) )
              result = atof( evaluated_result->item.token.c_str() );
            else is_greater = false;
          } // else
        } // if
        else {
          cout << "ERROR (> with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    if ( is_greater ) return g_true;
    else return g_nil;
  } // end Greater()

  NodeType* GreaterOrEqual( NodeType* node ) { // >=(>=2)
    double result = 0;
    int num_of_args = 0;
    bool is_greater_or_equal = true;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, ">=" ) ) {
          if ( num_of_args == 1 ) result = atof( evaluated_result->item.token.c_str() );
          else {
            if ( result >= atof( evaluated_result->item.token.c_str() ) )
              result = atof( evaluated_result->item.token.c_str() );
            else is_greater_or_equal = false;
          } // else
        } // if
        else {
          cout << "ERROR (>= with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    if ( is_greater_or_equal ) return g_true;
    else return g_nil;
  } // end GreaterOrEqual()

  NodeType* Less( NodeType* node ) { // <(>=2)
    double result = 0;
    int num_of_args = 0;
    bool is_less = true;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, "<" ) ) {
          if ( num_of_args == 1 ) result = atof( evaluated_result->item.token.c_str() );
          else {
            if ( result < atof( evaluated_result->item.token.c_str() ) )
              result = atof( evaluated_result->item.token.c_str() );
            else is_less = false;
          } // else
        } // if
        else {
          cout << "ERROR (< with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    if ( is_less ) return g_true;
    else return g_nil;
  } // end Less()

  NodeType* LessOrEqual( NodeType* node ) { // <=(>=2)
    double result = 0;
    int num_of_args = 0;
    bool is_less_or_equal = true;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, "<=" ) ) {
          if ( num_of_args == 1 ) result = atof( evaluated_result->item.token.c_str() );
          else {
            if ( result <= atof( evaluated_result->item.token.c_str() ) )
              result = atof( evaluated_result->item.token.c_str() );
            else is_less_or_equal = false;
          } // else
        } // if
        else {
          cout << "ERROR (<= with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    if ( is_less_or_equal ) return g_true;
    else return g_nil;
  } // end LessOrEqual()

  NodeType* Equal( NodeType* node ) { // =(>=2)
    double result = 0;
    int num_of_args = 0;
    bool is_equal = true;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, "=" ) ) {
          if ( num_of_args == 1 ) result = atof( evaluated_result->item.token.c_str() );
          else {
            if ( result == atof( evaluated_result->item.token.c_str() ) )
              result = atof( evaluated_result->item.token.c_str() );
            else is_equal = false;
          } // else
        } // if
        else {
          cout << "ERROR (= with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    if ( is_equal ) return g_true;
    else return g_nil;
  } // end Equal()

  NodeType* StringAppend( NodeType* node ) { // string-append(>=2)
    string result = "";
    int num_of_args = 0;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, "string-append" ) ) {
          if ( num_of_args == 1 ) result = evaluated_result->item.token;
          else {
            string tmp_1 = result.substr( 0, result.length() - 1 );
            string tmp_2 = evaluated_result->item.token.substr( 1, evaluated_result->item.token.length() );
            result = tmp_1 + tmp_2;
          } // else
        } // if
        else {
          cout << "ERROR (string-append with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    // build a node
    evaluated_result = new NodeType;
    evaluated_result->left = NULL;
    evaluated_result->right = NULL;
    evaluated_result->item.token = result;
    evaluated_result->item.type = "STRING";

    return evaluated_result;
  } // end StringAppend()

  NodeType* StringGreaterOrNot( NodeType* node ) { // string>?(>=2)
    string result = "";
    int num_of_args = 0;
    bool is_greater = true;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, "string>?" ) ) {
          if ( num_of_args == 1 ) result = evaluated_result->item.token;
          else {
            if ( result > evaluated_result->item.token )
              result = evaluated_result->item.token;
            else is_greater = false;
          } // else
        } // if
        else {
          cout << "ERROR (string>? with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    if ( is_greater ) return g_true;
    else return g_nil;
  } // end StringGreaterOrNot()

  NodeType* StringLessOrNot( NodeType* node ) { // string<?(>=2)
    string result = "";
    int num_of_args = 0;
    bool is_less = true;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, "string<?" ) ) {
          if ( num_of_args == 1 ) result = evaluated_result->item.token;
          else {
            if ( result < evaluated_result->item.token )
              result = evaluated_result->item.token;
            else is_less = false;
          } // else
        } // if
        else {
          cout << "ERROR (string<? with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    if ( is_less ) return g_true;
    else return g_nil;
  } // end StringLessOrNot()

  NodeType* StringEqualOrNot( NodeType* node ) { // string=?(>=2)
    string result = "";
    int num_of_args = 0;
    bool is_equal = true;
    NodeType* evaluated_result = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );
      if ( evaluated_result ) {
        num_of_args++;
        if ( IsCorrectType( evaluated_result, "string=?" ) ) {
          if ( num_of_args == 1 ) result = evaluated_result->item.token;
          else {
            if ( result == evaluated_result->item.token )
              result = evaluated_result->item.token;
            else is_equal = false;
          } // else
        } // if
        else {
          cout << "ERROR (string=? with incorrect argument type) : ";
          m_pretty_printer_.SetPrintProcedure( true );
          m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
          m_pretty_printer_.PrettyPrint( evaluated_result );
          throw "ERROR";
        } // else

        evaluated_result = GetArgument( node );
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else
    } // end while

    if ( is_equal ) return g_true;
    else return g_nil;
  } // end StringEqualOrNot()

  NodeType* EqvOrNot( NodeType* node ) { // eqv?(2)
    NodeType* first_arg = NULL;
    NodeType* second_arg = NULL;

    first_arg = Evaluate( GetArgument( node ) );
    if ( first_arg ) { // first arg fetched & no error occurs
      second_arg = Evaluate( GetArgument( node ) );
      if ( second_arg ) { // second arg fetched & no error occurs
        if ( first_arg->item.type != "STRING" && second_arg->item.type != "STRING" &&
             IsAtom( first_arg->item ) && IsAtom( second_arg->item ) ) {
          if ( first_arg->item.type == "FLOAT" && second_arg->item.type == "FLOAT" ) {
            if ( atof( first_arg->item.token.c_str() ) == atof( second_arg->item.token.c_str() ) )
              return g_true;
            else return g_nil;
          } // if
          else if ( first_arg->item.token == second_arg->item.token ) return g_true;
          else return g_nil;
        } // if
        else if ( first_arg == second_arg ) return g_true;
        else return g_nil;
      } // if
      else {
        cout << "ERROR (unbound parameter) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else

    } // if
    else {
      cout << "ERROR (unbound parameter) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return NULL;
  } // end EqvOrNot()

  NodeType* EqualOrNot( NodeType* first_arg, NodeType* second_arg ) { // equal?(2)
    if ( first_arg && second_arg ) {
      if ( first_arg->item.type == "FLOAT" && second_arg->item.type == "FLOAT" ) {
        if ( atof( first_arg->item.token.c_str() ) == atof( second_arg->item.token.c_str() ) ) {
          if ( EqualOrNot( first_arg->left, second_arg->left ) == g_true &&
               EqualOrNot( first_arg->right, second_arg->right ) == g_true )
            return g_true;
          else return g_nil;
        } // if
        else return g_nil;
      } // if
      else if ( first_arg->item.token == second_arg->item.token ) {
        if ( EqualOrNot( first_arg->left, second_arg->left ) == g_true &&
             EqualOrNot( first_arg->right, second_arg->right ) == g_true )
          return g_true;
        else return g_nil;
      } // if
      else return g_nil;
    } // if
    else if ( ! first_arg && ! second_arg ) return g_true;
    else return g_nil;

  } // end EqualOrNot()

  NodeType* Begin( NodeType* node ) { // begin(>=1)
    NodeType* evaluated_result = NULL;
    NodeType* evaluated_result_of_last_one = NULL;

    evaluated_result = GetArgument( node );
    while ( evaluated_result ) { // arg fetched & no error occurs
      evaluated_result = Evaluate( evaluated_result );

      evaluated_result_of_last_one = evaluated_result;
      evaluated_result = GetArgument( node );
    } // end while

    return evaluated_result_of_last_one;
  } // end Begin()

  NodeType* If( NodeType* node ) { // if(2 or 3)
    NodeType* evaluated_result = NULL;

    evaluated_result = Evaluate( GetArgument( node ) );
    if ( evaluated_result ) { // first arg fetched & no error occurs
      if ( evaluated_result == g_nil ) { // false => use third arg
        GetArgument( node ); // skip second arg
        evaluated_result = GetArgument( node );
        if ( evaluated_result ) // third arg fetched
          evaluated_result = Evaluate( evaluated_result );
        else return NULL;
      } // if
      else evaluated_result = Evaluate( GetArgument( node ) ); // true => use second arg
    } // if
    else {
      cout << "ERROR (unbound test-condition) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( node->left );
      throw "ERROR";
    } // else

    return evaluated_result;
  } // end If()

  NodeType* Cond( NodeType* node ) { // cond(>=1)
    NodeType* tmp = NULL;
    NodeType* list = NULL;
    NodeType* condition = NULL;

    list = GetArgument( node );
    while ( list ) { // first list fetched
      condition = list->left;

      tmp = node;
      if ( ! GetArgument( tmp ) && condition->item.token == "else" ) { // last list and SYM == "else"
        return Begin( list );
      } // if
      else {
        condition = Evaluate( condition );
        if ( condition ) { // no error occurs
          if ( condition == g_nil ) ; // false => fetched next condition
          else return Begin( list ); // true => return the last-evaluated result
        } // if
        else {
          cout << "ERROR (unbound test-condition) : ";
          m_pretty_printer_.SetPrintProcedure( false );
          m_pretty_printer_.PrettyPrint( list->left );
          throw "ERROR";
        } // else
      } // else

      list = GetArgument( node );
    } // end while

    return NULL;
  } // end Cond()

  NodeType* CleanEnvironment() { // clean-environment(0)
    m_user_defined_.clear();
    cout << "environment cleaned" << '\n';
    throw "SKIP";
  } // end CleanEnvironment()

  NodeType* Evaluate( NodeType* node ) {
    if ( ! node ) return NULL;

    NodeType* value = NULL;
    TokenType arg = node->item;
    NodeType* evaluated_result = NULL;

    // cout << m_cur_status_ << endl;

    if ( IsAtom( arg ) ) return node; // atom
    else if ( IsSymbol( arg ) ) { // symbol
      if ( m_cur_status_ == "pass-args" ) { // for passing args
        if ( IsLocalDef( arg.token, evaluated_result ) ) {
          return evaluated_result;
        } // else if
        else if ( IsUserDefined( arg.token ) ) {
          value = m_user_defined_[arg.token];
          return value; // atom or s-exp or internal function
        } // else if
        else if ( IsInernalFunction( arg.token ) ) {
          node->item.is_procedure = true;
          return node;  // system primitives
        } // else if
        else {
          cout << "ERROR (unbound symbol) : ";
          m_pretty_printer_.SetPrintProcedure( false );
          m_pretty_printer_.PrettyPrint( node );
          throw "ERROR";
        } // else
      } // if
      else if ( m_cur_status_ == "let" ) { // for let
        if ( IsLocalDef( arg.token, evaluated_result ) ) {
          return evaluated_result;
        } // else if
        else if ( IsUserDefined( arg.token ) ) {
          NodeType* value = m_user_defined_[arg.token];
          return value; // atom or s-exp or internal function
        } // else if
        else if ( IsInernalFunction( arg.token ) ) {
          node->item.is_procedure = true;
          return node;  // system primitives
        } // else if
        else {
          cout << "ERROR (unbound symbol) : ";
          m_pretty_printer_.SetPrintProcedure( false );
          m_pretty_printer_.PrettyPrint( node );
          throw "ERROR";
        } // else
      } // else if
      else if ( m_cur_status_ == "lambda" ) { // for lambda
        if ( IsLambdaDef( arg.token ) ) {
          NodeType* value = m_lambda_defs_.back()[arg.token];
          return value;
        } // if
        else if ( IsUserDefined( arg.token ) ) {
          NodeType* value = m_user_defined_[arg.token];
          return value; // atom or s-exp or internal function
        } // else if
        else if ( IsInernalFunction( arg.token ) ) {
          node->item.is_procedure = true;
          return node;  // system primitives
        } // else if
        else {
          cout << "ERROR (unbound symbol) : ";
          m_pretty_printer_.SetPrintProcedure( false );
          m_pretty_printer_.PrettyPrint( node );
          throw "ERROR";
        } // else
      } // else if
      else if ( m_cur_status_ == "function-call" ) { // for evaluating func
        if ( IsDefineDef( arg.token ) ) {
          NodeType* value = m_define_defs_.back()[arg.token];
          return value;
        } // if
        else if ( IsUserDefined( arg.token ) ) {
          value = m_user_defined_[arg.token];
          return value; // atom or s-exp or internal function
        } // else if
        else if ( IsInernalFunction( arg.token ) ) {
          node->item.is_procedure = true;
          return node;  // system primitives
        } // else if
        else {
          cout << "ERROR (unbound symbol) : ";
          m_pretty_printer_.SetPrintProcedure( false );
          m_pretty_printer_.PrettyPrint( node );
          throw "ERROR";
        } // else
      } // else if
      else if ( IsLocalDef( arg.token, evaluated_result ) ) {
        return evaluated_result;
      } // else if
      else if ( IsUserDefined( arg.token ) ) { // user-defined stuff
        value = m_user_defined_[arg.token];
        return value; // atom or s-exp or internal function
      } // else if
      else if ( IsInernalFunction( arg.token ) ) {
        node->item.is_procedure = true;
        return node;  // system primitives
      } // else if
      else {
        cout << "ERROR (unbound symbol) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node );
        throw "ERROR";
      } // else
    } // else if
    else {
      m_level_++;
      arg = node->left->item;

      if ( IsNotAPureList( node ) ) { // (...) is not a (pure) list
        cout << "ERROR (non-list) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node );
        throw "ERROR";
      } // if
      else if ( IsAtom( arg ) ) { // first argument of (...) is an atom ☆
        cout << "ERROR (attempt to apply non-function) : ";
        m_pretty_printer_.SetPrintProcedure( false );
        m_pretty_printer_.PrettyPrint( node->left );
        throw "ERROR";
      } // else if
      else if ( IsSymbol( arg ) ) { // first argument of (...) is a symbol SYM
        if ( IsInernalFunction( arg.token ) ) {
          if ( m_level_ > 1 && arg.token == "clean-environment" ) {
            cout << "ERROR (level of CLEAN-ENVIRONMENT)" << '\n';
            throw "ERROR";
          } // if
          else if ( m_level_ > 1 && arg.token == "define" ) {
            cout << "ERROR (level of DEFINE)" << '\n';
            throw "ERROR";
          } // if
          else if ( m_level_ > 1 && arg.token == "exit" ) {
            cout << "ERROR (level of EXIT)" << '\n';
            throw "ERROR";
          } // if

          node->left->item.is_procedure = true;
          evaluated_result = ExecuteFunction( node, NULL );
          return evaluated_result;
        } // if
        else { // SYM is 'abc', which is not the name of a known function
          value = Evaluate( node->left );
          
          if ( IsInernalFunction( value->item.token ) && value->item.is_procedure ) {
            if ( m_level_ > 1 && arg.token == "clean-environment" ) {
              cout << "ERROR (level of CLEAN-ENVIRONMENT)" << '\n';
              throw "ERROR";
            } // if
            else if ( m_level_ > 1 && arg.token == "define" ) {
              cout << "ERROR (level of DEFINE)" << '\n';
              throw "ERROR";
            } // else if
            else if ( m_level_ > 1 && arg.token == "exit" ) {
              cout << "ERROR (level of EXIT)" << '\n';
              throw "ERROR";
            } // else if

            // value->item.is_procedure = true;

            evaluated_result = ExecuteFunction( node, value );
            return evaluated_result;
          } // if
          else if ( IsLambda( value ) ) {
            // symbol definition for lambda def ( no use )
            // value->left->item.is_procedure = true;

            evaluated_result = ExecuteFunction( node, value );
            return evaluated_result;
          } // else if
          else if ( IsUserDefinedFunc( value ) ) {
            // function definition
            evaluated_result = FunctionCall( node, value );
            return evaluated_result;
          } // else if
          else {
            cout << "ERROR (attempt to apply non-function) : ";
            m_pretty_printer_.SetPrintProcedure( true );
            m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
            m_pretty_printer_.PrettyPrint( value );
            throw "ERROR";
          } // else
        } // else
      } // else if
      else { // the first argument of ( ... ) is ( 。。。 ), i.e., it is ( ( 。。。 ) ...... )
        NodeType* left = Evaluate( node->left );
        if ( left ) { // no error occurs
          if ( IsInernalFunction( left->item.token ) && left->item.is_procedure ) {
            if ( m_level_ > 1 && left->item.token == "clean-environment" ) {
              cout << "ERROR (level of CLEAN-ENVIRONMENT)" << '\n';
              throw "ERROR";
            } // if
            else if ( m_level_ > 1 && left->item.token == "define" ) {
              cout << "ERROR (level of DEFINE)" << '\n';
              throw "ERROR";
            } // else if
            else if ( m_level_ > 1 && left->item.token == "exit" ) {
              cout << "ERROR (level of EXIT)" << '\n';
              throw "ERROR";
            } // else if
            
            evaluated_result = ExecuteFunction( node, left );
            return evaluated_result;
          } // if
          else if ( IsLambda( left ) ) {
            // lambda def + use
            evaluated_result = ExecuteFunction( node, left );
            return evaluated_result;
          } // else if
          else if ( IsUserDefinedFunc( left ) ) {
            // function definition
            evaluated_result = FunctionCall( node, left );
            return evaluated_result;
          } // else if
          else {
            cout << "ERROR (attempt to apply non-function) : ";
            m_pretty_printer_.SetPrintProcedure( true );
            m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
            m_pretty_printer_.PrettyPrint( left );
            throw "ERROR";
          } // else
        } // if
        else {
          cout << "ERROR (no return value) : ";
          m_pretty_printer_.SetPrintProcedure( false );
          m_pretty_printer_.PrettyPrint( node->left );
          throw "ERROR";
        } // else
      } // else
    } // else

  } // end Evaluate()

public:
  Evaluator() {
    m_level_ = 0;
    m_cur_status_ = "";
    m_root_for_parse_tree_ = NULL;
    m_root_for_evaluate_tree_ = NULL;
    
    m_let_defs_.clear();
    m_lambda_defs_.clear();
    m_define_defs_.clear();
    m_local_defs_.clear();

    m_user_defined_.clear();
    m_internal_functions_.clear();
    m_internal_functions_.push_back( "cons" );
    m_internal_functions_.push_back( "list" );
    m_internal_functions_.push_back( "quote" );
    m_internal_functions_.push_back( "'" );
    m_internal_functions_.push_back( "define" );
    m_internal_functions_.push_back( "car" );
    m_internal_functions_.push_back( "cdr" );
    m_internal_functions_.push_back( "atom?" );
    m_internal_functions_.push_back( "pair?" );
    m_internal_functions_.push_back( "list?" );
    m_internal_functions_.push_back( "null?" );
    m_internal_functions_.push_back( "integer?" );
    m_internal_functions_.push_back( "real?" );
    m_internal_functions_.push_back( "number?" );
    m_internal_functions_.push_back( "string?" );
    m_internal_functions_.push_back( "boolean?" );
    m_internal_functions_.push_back( "symbol?" );
    m_internal_functions_.push_back( "+" );
    m_internal_functions_.push_back( "-" );
    m_internal_functions_.push_back( "*" );
    m_internal_functions_.push_back( "/" );
    m_internal_functions_.push_back( "not" );
    m_internal_functions_.push_back( "and" );
    m_internal_functions_.push_back( "or" );
    m_internal_functions_.push_back( ">" );
    m_internal_functions_.push_back( ">=" );
    m_internal_functions_.push_back( "<" );
    m_internal_functions_.push_back( "<=" );
    m_internal_functions_.push_back( "=" );
    m_internal_functions_.push_back( "string-append" );
    m_internal_functions_.push_back( "string>?" );
    m_internal_functions_.push_back( "string<?" );
    m_internal_functions_.push_back( "string=?" );
    m_internal_functions_.push_back( "eqv?" );
    m_internal_functions_.push_back( "equal?" );
    m_internal_functions_.push_back( "begin" );
    m_internal_functions_.push_back( "if" );
    m_internal_functions_.push_back( "cond" );
    m_internal_functions_.push_back( "clean-environment" );
    m_internal_functions_.push_back( "exit" );
    m_internal_functions_.push_back( "let" );
    m_internal_functions_.push_back( "lambda" );
  } // end Evaluator()

  void SetRootForParseTree( NodeType* root_for_parse_tree ) {
    m_level_ = 0;
    m_cur_status_ = "";
    
    m_let_defs_.clear();
    m_lambda_defs_.clear();
    m_define_defs_.clear();
    m_local_defs_.clear();

    m_pretty_printer_.Reset();
    m_root_for_parse_tree_ = root_for_parse_tree;
  } // end SetRootForParseTree()

  void Evaluate() {
    m_root_for_evaluate_tree_ = Evaluate( m_root_for_parse_tree_ );
  } // end Evaluate()

  void PrintEvaluation() {
    if ( m_root_for_evaluate_tree_ ) {
      m_pretty_printer_.SetPrintProcedure( true );
      m_pretty_printer_.SetUserDefinedFunc( m_user_defined_ );
      m_pretty_printer_.PrettyPrint( m_root_for_evaluate_tree_ );
    } // if
    else {
      cout << "ERROR (no return value) : ";
      m_pretty_printer_.SetPrintProcedure( false );
      m_pretty_printer_.PrettyPrint( m_root_for_parse_tree_ );
      throw "ERROR";
    } // else
  } // end PrintEvaluation()

} ; // end class Evaluator

int main() {
  Parser parser;
  Evaluator evaluator;
  int error_no = 0;
  char test_num = '\0';
  GlobalNilTrueInitialization();

  scanf( "%c", &test_num ); // read the test number
  scanf( "%c", &test_num ); // skip '\n'

  cout << "Welcome to OurScheme!\n";
  do {
    cout << "\n> ";
    parser.ResetParser();
    parser.ResetScanner();
    error_no = parser.ReadSexp();

    if ( error_no == 0 ) { // parse successfully
      parser.BuildTree();
      evaluator.SetRootForParseTree( parser.GetRoot() );

      try {
        evaluator.Evaluate();
        evaluator.PrintEvaluation();
      } // try
      catch ( char const* msg ) {
        // ERROR already be printed
      } // catch

    } // if
    else parser.PrintErrorMsg( error_no );

  } while ( ! parser.IsExit() && ! parser.IsEOF( error_no ) ) ;

  cout << "\nThanks for using OurScheme!\n";

  return 0;
} // main()
