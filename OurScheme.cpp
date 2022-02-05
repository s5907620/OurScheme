# include <iostream>
# include <stdio.h>
# include <sstream>
# include <string>
# include <vector>

using namespace std;

enum Token_Type {
    LEFT_PAREN = 1, RIGHT_PAREN = 2, DOT = 3, QUOTE = 4, SYMBOL = 5,
    INT = 6, FLOAT = 7, STRING = 8, NILS = 9, TRUE = 10,
    SEMI_COLON = 11, ATOM = 12, PAREN_QUOTE = 13, DOT_PAREN = 14, 
    END_OF_LINE = -1, INITIAL = -2
};

enum Error_Type {
    Unexpected_Atom_or_Left_Paren = 100,
    Unexpected_Right_Paren = 200,
    No_Closing_Quote = 300,
    ERROR_EOF = 400,
    QUIT_Scheme = 500
};

struct Token {
    string name;
    int type;
    int line, column;
    Token * left;
    Token * right;
};

typedef Token * Node;

class Terminal {
public:
    string name;
    int type;

    Terminal() {
        name = "";
        type = 0;
    } // Terminal()
};

// Scanner 專門用來切出 token
class Scanner { 
public:
    int mLine, mColumn;
    void SkipWhiteSpace(); // 跳果所有 White spaces 直到第一個不是 White spaces 的 char
    void Gettoken( Token & token );
    void GetParen( Token & token );
    void GetSemiColon( Token & token );
    void GetPoundSign( Token & token );
    void GetSymbol( Token & token, char ch[80] );
    void GetQuote( Token & token );
    void GetIntOrFloat( Token & token );
    void GetFloat( Token & token, char tmp[80] );
    void GetInt( Token & token, char tmp[80] );
    void GetGarbage();
    void Initial();
    string CharToString( char ch[80] );

    Scanner() {
        mLine = 1;
        mColumn = 0;
    } // Scanner()
};

// Parser 處理文法跟建樹
class Parser {
public:
    Scanner mScanner;
    Token mPeekToken;
    Terminal mCurToken;
    Node mRoot;
    vector <Terminal> mTokenList; // 存放通過 parser 檢查的 token
    int mNumOfSpace = 0;
    bool mPreTokenIsRightParen = true;

    void InitialToken(); // initialization mPeekToken
    void DealWithError();
    void ReadExp();
    void Atom();
    void GetTerminal();
    void BuildTree();
    void AddNewLeaf( Node & cur, int & dot_paren, int & left_paren, bool & erase );
    void PrintTree( Node cur );
    void HowToPrint( string name, int type, int space, bool isRightParen);
    void CountSpace( string name, int type, int space );
    void InitialScheme();
    void DeleteTree( Node & cur );
    void CheckQuote();
    void ExtandQuote( int index );
    void CheckSpecialExit();
    void DeleteDotParen( int count );
    void DeleteDot( int index );
    Node SetLeaf( Terminal term );
    int DotParenAt();
    int DotParenCount();
    int PeekToken();
    int QuoteIsAt();

    Parser() {
        mRoot = NULL;
        mTokenList.clear();
        InitialToken();
    } // Parser()
};

class Exception {
public:
    int mType;
    string mToken;

    Exception() {
        // initial
        mType = 999;
        mToken = "";
    } // Exception()

    Exception( int type, int line, int column ) {
        // do not print token name
        mType = type; // setting mType

        if ( type == No_Closing_Quote ) {
            cout << "ERROR (no closing quote) : END-OF-LINE encountered at Line ";
            cout << line << " Column " << column << endl << endl;
        } // if
        else if ( type == ERROR_EOF )
            cout << "ERROR (no more input) : END-OF-FILE encountered";
        else if ( type == QUIT_Scheme )
            ; // do nothing
    } // Exception( int type, int line, int column )

    Exception( Token token, int type ) {
        mType = type;

        if ( type == Unexpected_Atom_or_Left_Paren ) {
            cout << "ERROR (unexpected token) : atom or '(' expected when token at ";
            cout << "Line " << token.line << " Column " << token.column << " is >>";
            cout << token.name << "<<" << endl << endl;
        } // if
        else if ( type == Unexpected_Right_Paren ) {
            cout << "ERROR (unexpected token) : ')' expected when token at ";
            cout << "Line " << token.line << " Column " << token.column << " is >>";
            cout << token.name << "<<" << endl << endl;
        } // else if
    } // Exception( Token token, int type )
};

int main() {
    int testNum = 0;
    char ch = '\0';
    bool isExit = false;

    Parser parser;
    
    // input testNum
    cin >> testNum;
    scanf( "%c", &ch ); // get '\n'

    cout << "Welcome to OurScheme!" << endl << endl;

    while ( !isExit ) {
        try {
            cout << "> ";
            parser.ReadExp();
            parser.BuildTree();
            parser.PrintTree( parser.mRoot );
            parser.InitialScheme();
            cout << endl;
            // DeleteTree
        } // tru
        catch ( Exception * error ) {
            parser.DealWithError();
            if ( error -> mType == ERROR_EOF || error -> mType == QUIT_Scheme )
                isExit = true;
        } // catch
    } // while

    cout << endl;
    cout << "Thanks for using OurScheme!" << endl; // end
} // main()

// Scanner
void Scanner::SkipWhiteSpace() {
    char ch = '\0';

    while ( ( scanf( "%c", &ch ) != EOF ) ) {
        mColumn ++;

        if ( ch == '\n' ) { // new line
            mColumn = 0;
            mLine ++;
        } // if

        ch = cin.peek();
        if ( ch == '\n' || ch == '\t' || ch == ' ' )
            ; // do nothing      
        else 
            return; // get the first non-white-space char
    } // while
} // Scanner::SkipWhiteSpace()

void Scanner::Gettoken( Token & token ) {
    int ch = 0;
    char tmp[80] = { '\0' };

    ch = cin.peek();
    if ( ch == '\n' || ch == '\t' || ch == ' ' ) // 把white space讀乾淨
        SkipWhiteSpace();

    ch = cin.peek(); // 先偷看一下 但不讀入
    if ( ch == '(' || ch == ')' )
        // maybe '(' or '()'
        GetParen( token );
    else if ( ch == '#' )
        // maybe '#t' or '#f' or symbol
        GetPoundSign( token );
    else if ( ch == '\'' || ch == '"' )
        GetQuote( token );
    else if ( ch == ';' )
        GetSemiColon( token );
    else if ( ch == '+' || ch == '-' || ch == '.' ||
            ( ch >= '0' && ch <= '9' ) )
        GetIntOrFloat( token ); // 在裡面做詳細分類
    else if ( ch == EOF ) {
        token.name = "";
        token.type = END_OF_LINE;
    } // else if
    else
        GetSymbol( token, tmp );
    
} // Scanner::Gettoken()

void Scanner::GetParen( Token & token ) {
    int peek = 0, type = 999, tmp_column = -100;
    char ch = '\0';
    char tmp[3] = { '\0' };

    peek = cin.peek();
    if ( peek == '(' ) {
        // maybe '(' or '()'
        type = LEFT_PAREN;
        scanf( "%c", &ch ); // get '('
        mColumn ++;
        tmp_column = mColumn;
        tmp[0] = ch;

        peek = cin.peek();
        if ( peek == '\n' || peek == '\t' || peek == ' ' )
            SkipWhiteSpace();

        peek = cin.peek();
        if ( peek == ')' ) {
            // get '()'
            scanf( "%c", &ch );
            mColumn ++;
            tmp[1] = ch;
            type = NILS;
        } // if
        
    } // if
    else {
        // get ')'
        type = RIGHT_PAREN;
        scanf( "%c", &ch );
        mColumn ++;
        tmp_column = mColumn;
        tmp[0] = ch;
    } // else

    // set token inf.
    token.column = tmp_column;
    token.line = mLine;
    token.name = CharToString( tmp );
    token.type = type;
} // Scanner::GetParen()

void Scanner::GetSemiColon( Token & token ) {
    char tmp[3] = { '\0' };
    char ch = '\0';

    scanf( "%c", &ch ); // get ';'
    tmp[0] = ch;
    mColumn ++;
    token.column = mColumn;
    token.line = mLine;
    GetGarbage(); // 把註解吃掉
    mLine ++;
    mColumn = 0;

    token.name = CharToString( tmp );
    token.type = SEMI_COLON;
} // Scanner::GetSemiColon()

void Scanner::GetPoundSign( Token & token ) {
    int peek = 0, type = 999, i = 2;
    char ch = '\0';
    char tmp[80] = { '\0' };

    scanf( "%c", &ch ); // get '#'
    mColumn ++;
    token.column = mColumn;
    token.line = mLine;
    tmp[0] = ch;

    peek = cin.peek();
    if ( peek == 't' || peek == 'f' ) {
        // maybe '#t' or '#f'
        scanf( "%c", &ch ); // get 't' or 'f'
        mColumn ++;
        tmp[1] = ch;

        if ( peek == 't' ) // '#t'
            type = TRUE;
        else // '#f'
            type = NILS;

        peek = cin.peek();
        if ( peek == ' ' || peek == '\t' || peek == '\n' ||
             peek == '(' || peek == ')' || 
             peek == '\'' || peek == '"' || peek == ';' ) {
            // get '#t' or '#f'
            token.name = CharToString( tmp );
            token.type = type;
            return;
        } // if
        else
            GetSymbol( token, tmp );
            
    } // if
    else if ( peek == ' ' || peek == '\t' || peek == '\n' ) {
        token.name = CharToString( tmp );
        token.type = SYMBOL;
        return;
    } // else if
    else
        GetSymbol( token, tmp );

} // Scanner::GetPoundSign()

void Scanner::GetSymbol( Token & token, char ch[80] ) {
    int peek, type = 999;
    int i = strlen( ch ); // index
    char tmp = '\0';
    string s = "";
    bool stop = false;

    token.line = mLine;
    peek = cin.peek();
    if ( peek == ' ' || peek == '\t' || peek == '\n' ||
         peek == '(' || peek == ')' ||
         peek == '\'' || peek == '"' || peek == ';' )
        stop = true;

    while ( !stop && ( scanf( "%c", &tmp ) != EOF ) ) {
        mColumn ++;
        if ( token.column == 0 )
            token.column = mColumn;

        ch[i] = tmp;
        peek = cin.peek(); // 偷看下一個char

        if ( peek == ' ' || peek == '\t' || peek == '\n' ||
             peek == '(' || peek == ')' ||
             peek == '\'' || peek == '"' || peek == ';' )
            stop = true;

        i ++;
    } // while

    s = CharToString( ch );
    if ( s.compare( "t" ) == 0 )
        type = TRUE;
    else if ( s.compare( "nil" ) == 0 )
        type = NILS;
    else
        type = SYMBOL;

    token.name = s;
    token.type = type;
} // Scanner::GetSymbol()

void Scanner::GetQuote( Token & token ) {
    char ch = '\0', tmp[80] = { '\0' }, escape[3] = { '\0' };
    int peek, i = 1;

    peek = cin.peek();
    scanf( "%c", &ch ); // maybe '"' or '\''
    mColumn ++;

    // set token address
    token.line = mLine;
    token.column = mColumn;

    tmp[0] = ch;
    if ( peek == '\'' ) {
        // get '\''
        token.name = CharToString( tmp );
        token.type = QUOTE;
        return;
    } // if
    else {
        // String
        peek = cin.peek();
        while ( peek != '"' ) {
            if ( peek == '\\') {
                scanf( "%c", &ch );
                mColumn ++;
                escape[0] = ch; // get '\\'

                peek = cin.peek();
                if ( peek == '\n' )
                    throw new Exception( No_Closing_Quote, mLine, mColumn + 1 );

                scanf( "%c", &ch ); // maybe 'n' or '\\' or '"' or '\t' or other
                mColumn ++;
                if ( ch == 'n' )
                    tmp[i] = '\n';
                else if ( ch == 't' )
                    tmp[i] = '\t';
                else if ( ch == '\\' )
                    tmp[i] = '\\';
                else if ( ch == '"' )
                    tmp[i] = '"';
                else {
                    // other
                    escape[1] = ch;
                    strcat( tmp, escape );
                    escape[0] = '\0';
                    i = strlen( tmp ) - 1;
                } // else
            } // if
            else if ( peek == '\n' )
                throw new Exception( No_Closing_Quote, mLine, mColumn + 1 );
            else {
                scanf( "%c", &ch );
                mColumn ++;
                tmp[i] = ch;
            } // else 
                
            peek = cin.peek();
            i ++;
        } // while

        // maybe EOF or peek( '"' )
        if ( peek == EOF ) // ERROR
            throw new Exception( No_Closing_Quote, mLine, mColumn + 1 );
        else {
            scanf( "%c", &ch ); // get '"'
            mColumn ++;
            i = strlen( tmp );
            tmp[i] = ch;
            token.name = CharToString( tmp );
            token.type = STRING;
        } // else
    } // else
} // Scanner::GetQuote()

void Scanner::GetIntOrFloat( Token & token ) {
    int peek, i = 0;
    char ch = '\0', tmp[80] = { '\0' };

    peek = cin.peek();
    if ( peek == '.' ) { // maybe float or symbol
        scanf( "%c", &ch ); // get '.'
        mColumn ++;
        token.column = mColumn;
        token.line = mLine;
        tmp[0] = ch; // save '.'
        i ++;
        peek = cin.peek();
        if ( peek == ' ' || peek == '\t' || peek == '\n' ||
             peek == '(' || peek == ')' || peek == EOF ||
             peek == '\'' || peek == '"' || peek == ';' ) {
            // DOT
            token.name = CharToString( tmp );
            token.type = DOT;
            return;
        } // if
        else if ( peek < '0' || peek > '9' )
            // not '0' ~ '9'
            GetSymbol( token, tmp );
        else // '0' ~ '9'
            GetFloat( token, tmp );

    } // if
    else if ( peek == '+' || peek == '-' ) {
        scanf( "%c", &ch ); // get '+' or '-'
        mColumn++;
        token.column = mColumn;
        token.line = mLine;
        tmp[0] = ch; // save '+' or '-'
        i ++;
        peek = cin.peek();
        if ( peek == ' ' || peek == '\t' || peek == '\n' ||
             peek == '(' || peek == ')' || peek == EOF ||
             peek == '\'' || peek == '"' || peek == ';' ) {
            token.name = CharToString( tmp );
            token.type = SYMBOL;
            return;
        } // if
        else if ( peek == '.' ) {
            scanf( "%c", &ch ); // get '.'
            tmp[1] = ch;
            mColumn ++;
            peek = cin.peek();
            if ( peek == ' ' || peek == '\t' || peek == '\n' ||
                 peek == '(' || peek == ')' || peek == EOF ||
                 peek == '\'' || peek == '"' || peek == ';' ) {
                // DOT
                token.name = CharToString( tmp );
                token.type = SYMBOL;
                return;
            } // if
            else if ( peek < '0' || peek > '9' )
                GetSymbol( token, tmp );
            else // '0' ~ '9'
                GetFloat( token, tmp );

        } // else if
        else if ( peek < '0' || peek > '9' )
            // not '0' ~ '9'
            GetSymbol( token, tmp );
        else // '0' ~ '9'
            GetInt( token, tmp );

    } // else if
    else  // '0' ~ '9'
        GetInt( token, tmp ); // int or float
} // Scanner::GetIntOrFloat()

void Scanner::GetFloat( Token & token, char tmp[80] ) {
    // 已經get '.'
    char ch = '\0';
    int peek;
    int i = strlen( tmp );

    peek = cin.peek();
    if ( peek == ' ' || peek == '\t' || peek == '\n' ||
         peek == '(' || peek == ')' ||
         peek == '\'' || peek == '"' || peek == ';' ) {
        token.name = CharToString( tmp );
        token.type = FLOAT;
        return;
    } // if 
    else if ( peek > '9' ) {
        GetSymbol( token, tmp );
        return;
    } // else if
    else if ( peek < '0' ) {
        GetSymbol( token, tmp );
        return;
    } // else if

    while ( scanf( "%c", &ch ) != EOF ) {
        tmp[i] = ch;
        mColumn ++;
        peek = cin.peek();

        if ( peek == ' ' || peek == '\t' || peek == '\n' ||
             peek == '(' || peek == ')' ||
             peek == '\'' || peek == '"' || peek == ';' ) {
            token.name = CharToString( tmp );
            token.type = FLOAT;
            return;
        } // if
        else if ( peek > '9' ) {
            GetSymbol( token, tmp );
            return;
        } // else if
        else if ( peek < '0' ) {
            GetSymbol( token, tmp );
            return;
        } // else if
        else  // '0' ~ '9'
            ; // do nothing

        i ++;
    } // while

    token.name = CharToString( tmp );
    token.type = FLOAT;
} // Scanner::GetFloat()

void Scanner::GetInt( Token & token, char tmp[20] ) {
    // 還是有可能碰到float
    int peek;
    int i = strlen( tmp );
    char ch = '\0';

    token.line = mLine;

    while ( scanf( "%c", &ch ) != EOF ) {
        tmp[i] = ch;

        mColumn ++;
        if ( token.column == 0 ) 
            token.column = mColumn;

        peek = cin.peek();

        if ( peek == '.' ) {
            scanf( "%c", &ch ); // get '.'
            mColumn ++;
            tmp[i + 1] = ch;    // save '.'
            GetFloat( token, tmp );
            return;
        } // if
        else if ( peek == ' ' || peek == '\t' || peek == '\n' ||
                  peek == '(' || peek == ')' ||
                  peek == '\'' || peek == '"' || peek == ';' ) {
            token.name = CharToString( tmp );
            token.type = INT;
            return;
        } // else if
        else if ( peek > '9' ) {
            GetSymbol( token, tmp );
            return;
        } // else if
        else if ( peek < '0' ) {
            GetSymbol( token, tmp );
            return;
        } // else if
        else // '0' ~ '9'
            ; // do nothing

        i ++;
    } // while

    token.name = CharToString( tmp );
    token.type = INT;
} // Scanner::GetInt()

void Scanner::GetGarbage() {
    int peek = 0;
    char ch = '\0';
    peek = cin.peek();

    while ( peek != '\n' ) {   
        // 讀到enter作結束
        scanf( "%c", &ch );
        peek = cin.peek();

        if ( peek == EOF )
            return;
    } // while

    scanf( "%c", &ch ); // get '\n'
} // Scanner::GetGarbage()

void Scanner::Initial() {
    char ch;
    int peek;

    peek = cin.peek();
    while ( peek == '\t' || peek == ' ' ) {
        scanf( "%c", &ch );
        mColumn ++;
        peek = cin.peek();
    } // while
        
} // Scanner::Initial()

string Scanner::CharToString( char ch[80] ) {
    string s = "";
    for ( int i = 0; ch[i] != '\0'; i ++ )
        s = s + ch[i];

    return s;
} // Scanner::CharToString

// Parser
void Parser::InitialToken() {
    mPeekToken.name = "";
    mPeekToken.column = 0;
    mPeekToken.line = 0;
    mPeekToken.type = INITIAL;
} // Parser:: InitialToken()

void Parser::InitialScheme() {
    char ch = '\0';
    int peek = 0;

    mScanner.mColumn = 0;
    mScanner.mLine = 1;

    peek = cin.peek();
    if ( peek == '\t' || peek == ' ' )
        mScanner.Initial();

    peek = cin.peek();
    if ( peek == ';' ) {
        mScanner.Gettoken( mPeekToken );
        mScanner.mColumn = 0;
        mScanner.mLine = 1;
    } // if
    else if ( peek == '\n' ) {
        scanf( "%c", &ch ); // get '\n'
        mScanner.mColumn = 0;
        mScanner.mLine = 1;
    } // else if
        
    DeleteTree( mRoot );
    mRoot = NULL;
    InitialToken();
    mTokenList.clear();
    mPreTokenIsRightParen = true;
    mNumOfSpace = 0;
} // Parser::InitialScheme()

void Parser::DealWithError() {
    mScanner.GetGarbage(); // 把同一行 input 吃掉
    mScanner.mColumn = 0;
    mScanner.mLine = 1;
    mTokenList.clear();
    InitialToken();        // initialization mPeekToken
} // Parser::DealWithError()

void Parser::ReadExp() {
    int type = 0;

    type = PeekToken();
    if ( type == ATOM ) {
        Atom(); // get <ATOM>
        if ( mCurToken.name.compare( "exit" ) == 0 ) {
            if ( mTokenList.size() == 1 )
                return;

            type = PeekToken();
            if ( type == RIGHT_PAREN && mTokenList.size() == 2 ) {
                // ( exit
                GetTerminal();
                mTokenList.push_back( mCurToken ); // get ')'
                throw new Exception( QUIT_Scheme, 0, 0 );
            } // if
        } // if

        return;
    } // if

    else if ( type == LEFT_PAREN ) {
        GetTerminal();
        mTokenList.push_back( mCurToken ); // get '('
        
        type = PeekToken();
        if ( type == ATOM || type == QUOTE || type == LEFT_PAREN )
            ReadExp();
        else
            throw new Exception( mPeekToken, Unexpected_Atom_or_Left_Paren );
        
        type = PeekToken();
        while ( type == ATOM || type == QUOTE || type == LEFT_PAREN ) {
            ReadExp();
            type = PeekToken();
        } // while

        type = PeekToken();
        if ( type == DOT ) {
            GetTerminal();
            mTokenList.push_back( mCurToken ); // get '.'
            ReadExp();
        } // if

        type = PeekToken();
        if ( type == RIGHT_PAREN ) {
            GetTerminal();
            mTokenList.push_back( mCurToken ); // get ')'
            return;
        } // else if
        else if ( type == END_OF_LINE )
            throw new Exception( ERROR_EOF, 0, 0 );
        else 
            throw new Exception( mPeekToken, Unexpected_Right_Paren );
        
    } // else if

    else if ( type == QUOTE ) {
        GetTerminal();
        mTokenList.push_back( mCurToken ); // get '\''
        ReadExp();
    } // else if

    else if ( type == END_OF_LINE )
        throw new Exception( ERROR_EOF, 0, 0 );

    else
        throw new Exception( mPeekToken, Unexpected_Atom_or_Left_Paren );

} // Parser::ReadExp()

void Parser::Atom() {
    GetTerminal(); // get <ATOM>
    mTokenList.push_back( mCurToken );
} // Parser::Atom()

void Parser::GetTerminal() {
    // 從mPeekToken拿東西 放到mCurToken
    mCurToken.name = mPeekToken.name;
    mCurToken.type = mPeekToken.type;

    InitialToken(); // 拿走後要初始化
} // Parser::GetTerminal()

void Parser::BuildTree() {
    mRoot = NULL; // initial
    Terminal term;
    int dot_paren = 0; // ".("
    int left_paren = 0;
    bool erase = true;

    CheckSpecialExit();
    CheckQuote(); // 展開成好處理的樣子

    int count = DotParenCount();
    DeleteDotParen( count );

    
    // root node
    term = mTokenList[0];
    mTokenList.erase( mTokenList.begin() ); // 刪除最前面的
    mRoot = SetLeaf( term );
    
    // other node
    while ( !mTokenList.empty() ) {
        AddNewLeaf( mRoot, dot_paren, left_paren, erase );
    } // while
    
} // Parser::BuildTree()

void Parser::CheckSpecialExit() {
    if ( mTokenList.size() == 5 ) {
        if ( mTokenList[1].name.compare( "exit" ) == 0 )
            if ( mTokenList[2].type == DOT && mTokenList[3].type == NILS )
                throw new Exception( QUIT_Scheme, 0, 0 );
    } // if

} // Parser::CheckSpecialExit()

void Parser::AddNewLeaf( Node & cur, int & dot_paren, int & left_paren, bool & erase ) {
    Terminal term, tmp;
    tmp.name = "";
    tmp.type = INITIAL;

    if ( cur -> left == NULL ) // 直接建在左邊
        ; // do nothing
    else if ( cur -> right != NULL ) {
        AddNewLeaf( cur -> right, dot_paren, left_paren, erase ); // 右邊還有路
        return;
    } // else if
    else // 左邊有東西，右邊沒路走，建在右邊
        ; // do nothing

    term = mTokenList[0]; // get

    if ( term.type == SYMBOL || term.type == INT || term.type == FLOAT ||
         term.type == STRING || term.type == TRUE ) {
        if ( cur -> left == NULL ) {
            cur -> left = SetLeaf( term );
            mTokenList.erase( mTokenList.begin() );
        } // if
        else {
            // 建立一個 '.' 分開左右子樹
            cur -> right = SetLeaf( tmp );
            cur -> right -> left = SetLeaf( term );
            mTokenList.erase( mTokenList.begin() );
        } // else
            
    } // if
    else if ( term.type == NILS ) {
        if ( cur -> type == DOT ) {
            mTokenList.erase( mTokenList.begin() ); // 直接略過
            cur -> type = INITIAL;
        } // if
        else {
            if ( cur -> left == NULL ) {
                cur -> left = SetLeaf( term );
                mTokenList.erase( mTokenList.begin() );
            } // if
            else
                cur -> right = SetLeaf( tmp );
        } // else 
    } // else if
    else if ( term.type == DOT ) {
        if ( cur -> left == NULL )
            cur -> left = SetLeaf( term );
        else 
            cur -> right = SetLeaf( term );

        mTokenList.erase( mTokenList.begin() );
    } // else if
    else if ( term.type == LEFT_PAREN ) {
        /*
        if ( cur -> type == DOT && mTokenList[1].type != PAREN_QUOTE ) {
            cur -> type = DOT_PAREN; // ".("
            mTokenList.erase( mTokenList.begin() );
            dot_paren ++;
        } // if
        */
        if (1) {
            erase = false;
            left_paren ++;
            if ( cur -> left == NULL ) {
                cur -> left = SetLeaf( term );
                mTokenList.erase( mTokenList.begin() ); // 去掉'('

                term = mTokenList[0];
                while ( term.type != RIGHT_PAREN ) {
                    AddNewLeaf( cur -> left, dot_paren, left_paren, erase );
                    term = mTokenList[0];
                } // while

                AddNewLeaf( cur -> left, dot_paren, left_paren, erase ); // get ')'
            } // if
            else {
                cur -> right = SetLeaf( term );
                mTokenList.erase( mTokenList.begin() ); // 去掉'('

                term = mTokenList[0];
                while ( term.type != RIGHT_PAREN ) {
                    AddNewLeaf( cur -> right, dot_paren, left_paren, erase );
                    term = mTokenList[0];
                } // while

                AddNewLeaf( cur -> right, dot_paren, left_paren, erase ); // get ')'
            } // else 
        } // else 
    } // else if
    else if ( term.type == PAREN_QUOTE ) {
        cur -> left = SetLeaf( term );
        mTokenList.erase( mTokenList.begin() ); // 去掉'\''
    } // else if
    else if ( term.type == RIGHT_PAREN ) {
        if ( dot_paren != 0 && erase )
            dot_paren --;
        else if ( left_paren != 0 ) {
            left_paren --;
            if (  cur -> left == NULL )
                cur -> left = SetLeaf( term );
            else {
                cur -> right = SetLeaf( tmp );
                cur -> right -> left = SetLeaf( term );
            } // else

            erase = true;
        } // else if
        else { // left_paren = 0, dot_paren = 0
            if ( cur -> left == NULL )
                cur -> left = SetLeaf( term );
            else {
                cur -> right = SetLeaf( tmp );
                cur -> right -> left = SetLeaf( term );
            } // else 

            erase = true;
        } // else 

        mTokenList.erase(mTokenList.begin()); // 去掉')'
    } // else if
} // Parser::AddNewLeaf()

void Parser::PrintTree( Node cur ) {
    if ( cur != NULL ) {
        if ( cur -> type != DOT_PAREN && cur -> type != INITIAL ) {
            if ( cur -> type == LEFT_PAREN ) {
                HowToPrint( cur -> name, cur -> type, mNumOfSpace, mPreTokenIsRightParen );
                mPreTokenIsRightParen = true;
                mNumOfSpace = mNumOfSpace + 2;
            } // if
            else if ( cur -> type == RIGHT_PAREN ) {
                mNumOfSpace = mNumOfSpace - 2;
                HowToPrint( cur -> name, cur -> type, mNumOfSpace, mPreTokenIsRightParen );
                mPreTokenIsRightParen = false;
            } // else if
            else {
                HowToPrint( cur -> name, cur -> type, mNumOfSpace, mPreTokenIsRightParen );
                mPreTokenIsRightParen = false;
            } // else 
                
        } // if

        PrintTree( cur -> left );
        PrintTree( cur -> right );
    } // if
} // Parser::PrintTree()

void Parser::HowToPrint( string name, int type, int space, bool isRightParen ) {
    int i = 0;
    float f = 0;
    stringstream ss;
    
    if ( type == LEFT_PAREN ) {
        if ( isRightParen ) {
            // 上一個是'(' 直接印出來
            cout << name << " ";
        } // if
        else {
            CountSpace( name, type, space );
            cout << " ";
        } // else 
            
    } // if
    else if ( type == RIGHT_PAREN ) {
        CountSpace( name, type, space );
        cout << endl;
    } // else if
    else if ( type == FLOAT ) {
        if ( isRightParen ) {
            ss << name;
            ss >> f;
            printf( "%.3f\n", f );
        } // if
        else {
            CountSpace( name, type, space );
            cout << endl;
        } // else 
    } // else if
    else if ( type == INT ) {
        if ( isRightParen ) {
            ss << name;
            ss >> i;
            printf( "%d\n", i );
        } // if
        else {
            CountSpace( name, type, space );
            cout << endl;
        } // else 
    } // else if
    else { // other
        if ( type == NILS )
            name = "nil";
        else if ( type == TRUE )
            name = "#t";
        else if ( type == PAREN_QUOTE )
            name = "quote";

        if ( isRightParen )
            cout << name;
        else
            CountSpace( name, type, space );

        cout << endl;
    } // else 
} // Parser::HowToPrint()

void Parser::CountSpace( string name, int type, int space ) {
    stringstream ss;
    int i = 0;
    float f = 0;

    for ( int j = 0; j < space; j ++ )
        cout << " ";

    if ( type == FLOAT ) {
        ss << name;
        ss >> f;
        printf( "%.3f", f );
    } // if
    else if ( type == INT ) {
        ss << name;
        ss >> i;
        printf( "%d", i );
    } // else if
    else
        cout << name;
} // Parser::CountSpace()

void Parser::DeleteTree( Node & cur ) {
    if ( cur != NULL )
    {
        DeleteTree( cur -> left );
        DeleteTree( cur -> right );
        delete cur;
    }
} // Parser::DeleteTree

void Parser::CheckQuote() {
    int numOfQuote = 0, index = -1;
    for ( int i = 0 ; i < mTokenList.size() ; i ++ ) {
        if ( mTokenList[i].type == QUOTE )
            numOfQuote ++;
    } // for

    while ( numOfQuote > 0 ) {
        index = QuoteIsAt(); // get first quote
        ExtandQuote( index );
        numOfQuote --;
    } // while
    
} // Parser::CheckQuote()

void Parser::ExtandQuote( int index ) {
    int i = index + 2;  // Quote 的後一個element
    int numOfParen = 0; // num of left paren
    bool stop = false;
    Terminal tmp;
    tmp.name = "(";
    tmp.type = LEFT_PAREN;

    mTokenList[index].type = PAREN_QUOTE;                 // "( Quote"
    mTokenList.insert( mTokenList.begin() + index, tmp ); // 在 Quote 之前插入 "("

    while ( i < mTokenList.size() && !stop ) {

        // 類似 stack 找到成對paren
        if ( mTokenList[i].type == LEFT_PAREN )
            numOfParen ++;
        else if ( mTokenList[i].type == RIGHT_PAREN )
            numOfParen --;
        else
            ; // do nothing

        if ( numOfParen == 0 && mTokenList[i].type != QUOTE )
            stop = true;

        i ++;
    } // while

    tmp.name = ")";
    tmp.type = RIGHT_PAREN;
    if ( i == mTokenList.size() )
        mTokenList.push_back( tmp );
    else 
        mTokenList.insert( mTokenList.begin() + i, tmp ); // "( Quote )"

} // Parser::ExtandQuote()

void Parser::DeleteDotParen( int count ) {
    int index = 0;

    while ( count != 0 ) {
        index = DotParenAt();
        DeleteDot( index );
        count --;
    } // while

} // Parser::DeleteDotParen()

void Parser::DeleteDot( int index ) {
    bool stop = false;
    int numOfParen = 1; // num of left paren
    int i = index;

    mTokenList.erase( mTokenList.begin() + index ); // delete DOT
    mTokenList.erase( mTokenList.begin() + index ); // delete LP

    while ( i < mTokenList.size() && !stop ) {
        // 類似 stack 找到成對paren
        if ( mTokenList[i].type == LEFT_PAREN )
            numOfParen ++;
        else if ( mTokenList[i].type == RIGHT_PAREN )
            numOfParen --;
        else
            ; // do nothing

        if ( numOfParen == 0 && mTokenList[i].type == RIGHT_PAREN )
            stop = true;

        i ++;
    } // while

     mTokenList.erase( mTokenList.begin() + i - 1 );   // delete RP
} // Parser::DeleteDot()

int Parser::DotParenAt() {
    int index = 0;
    for ( int i = 0 ; i < mTokenList.size() ; i ++ ) {
        if ( mTokenList[i].type == DOT && mTokenList[i + 1].type == LEFT_PAREN )
            if ( mTokenList[i + 2].type != PAREN_QUOTE )
                return i;
    } // for

    return index;
} // Parser::DotParenAt()

int Parser::QuoteIsAt() {
    for ( int i = 0 ; i < mTokenList.size() ; i ++ ) {
        if ( mTokenList[i].type == QUOTE )
            return i;
    } // for

} // Parser::QuoteIsAt()

int Parser::PeekToken() {
    if ( mPeekToken.type == INITIAL ) {
        mScanner.Gettoken( mPeekToken );
        while ( mPeekToken.type == SEMI_COLON ) {
            mScanner.Gettoken( mPeekToken );
        } // while
    } // if

    if ( mPeekToken.type == SYMBOL || mPeekToken.type == INT || mPeekToken.type == FLOAT ||
         mPeekToken.type == STRING || mPeekToken.type == NILS || mPeekToken.type == TRUE )
        return ATOM;

    return mPeekToken.type;
} // Parser::PeekToken()

int Parser::DotParenCount() {
    int count = 0;
    for ( int i = 0 ; i < mTokenList.size() ; i ++ ) {
        if ( mTokenList[i].type == DOT && mTokenList[i + 1].type == LEFT_PAREN )
            if ( mTokenList[i + 2].type != PAREN_QUOTE )
                count ++;
    } // for

    return count;
} // Parser::DotParenCount()

Node Parser::SetLeaf( Terminal term ) {
    Node tmp = new Token;
    tmp -> name = term.name;
    tmp -> type = term.type;
    tmp -> left = NULL;
    tmp -> right = NULL;
    return tmp;
} // Parser::SetLeaf()