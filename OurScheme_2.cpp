# include <iostream>
# include <stdio.h>
# include <sstream>
# include <string.h>
# include <string>
# include <vector>

using namespace std;

enum Token_Type {
    LEFT_PAREN = 1, RIGHT_PAREN = 2, DOT = 3, QUOTE = 4, SYMBOL = 5,
    INT = 6, FLOAT = 7, STRING = 8, NILS = 9, TRUE = 10,
    SEMI_COLON = 11, ATOM = 12, PAREN_QUOTE = 13, DOT_PAREN = 14,
    END_OF_LINE = 98, INITIAL = 99, INTER_FUNCTION = 97, PROCEDURE = 96
};

enum Error_Type {
    UNEXPECTED_ATOM_OR_LEFT_PAREN = 100,
    UNEXPECTED_RIGHT_PAREN = 200,
    NO_CLOSING_QUOTE = 300,
    ERROR_EOF = 400,
    QUIT_SCHEME = 500,
    UNBOUND_SYMBOL = 600,
    NON_LIST = 700,
    NON_FUNCTION = 800,
    ERROR_LEVEL = 900, // CLEAN-ENVIRONMENT DEFINE EXIT
    COND_FORMAT = 1000,
    DEF_FORMAT = 1100,
    SET_FORMAT = 1200,
    LET_FORMAT = 1300,
    LAMBDA_FORMAT = 1400,
    ERROR_NUM_OF_ARGUMENTS = 1500,
    ERROR_ARGUMENT_TYPE = 1600,
    NO_RETURN_RESULT = 1700,
    JUST_ERROR = 1800
};

enum Print_Type {
    NORMAL = 0, DF = 1, CLEAN_ENV = 2, ERROR_PRINT = 3
};

struct Token {
    string name;
    int type;
    int line, column;
    Token* left;
    Token* right;
};

typedef Token* Node;

class Terminal {
public:
    string mName;
    int mType;

    Terminal() {
        mName = "";
        mType = 0;
    } // Terminal()
};

// Scanner 專門用來切出 token
class Scanner {
public:
    int mLine, mColumn;
    void SkipWhiteSpace(); // 跳果所有 White spaces 直到第一個不是 White spaces 的 char
    void Gettoken( Token& token ) ;
    void GetParen( Token& token ) ;
    void GetSemiColon( Token& token ) ;
    void GetPoundSign( Token& token ) ;
    void GetSymbol( Token& token, char ch[80] ) ;
    void GetQuote( Token& token ) ;
    void GetIntOrFloat( Token& token ) ;
    void GetFloat( Token& token, char tmp[80] ) ;
    void GetInt( Token& token, char tmp[80] ) ;
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

    void InitialToken() ; // initialization mPeekToken
    void DealWithError( int type ) ;
    void ReadExp() ;
    void Atom() ;
    void GetTerminal() ;
    void BuildTree() ;
    void CheckSpecialExit() ;
    void AddNewLeaf( Node& cur, int& dot_paren, int& left_paren ) ;
    void InitialScheme() ;
    void DeleteTree( Node& cur ) ;
    void CheckQuote() ;
    void ExtandQuote( int index ) ;
    void DeleteDotParen( int count ) ;
    void DeleteDot( int index ) ;
    Node SetLeaf( Terminal term ) ;
    int DotParenAt() ;
    int DotParenCount() ;
    int PeekToken() ;
    int QuoteIsAt() ;

    Parser() {
        mRoot = NULL;
        mTokenList.clear();
        InitialToken();
    } // Parser()
};

class Execuate {
public:
    Parser mParser;
    int mPrintType, mNumOfSpace;
    bool mPreTokenIsRightParen;
    Node mPrintRoot, mBindRoot;

    void CheckPureList(Node cur, Node root);
    void PrintEval(Node cur, int type);
    void TypeChange(Node& cur);
    void InitialExe();
    void JumpToNextArgu(Node& cur);
    void PrintTree(Node cur, int printType);
    void HowToPrint(string name, int type, int space, bool isRightParen, int printType);
    void CountSpace(string name, int type, int space);
    void Eval_exit(int level, int arguCount);
    void Eval_clean_env(int level, int arguCount);
    Node EvalSExp(Node cur, int level, int& printType);
    Node Eval_MultiExp(Node head, Node cur, int level, int& printType);
    Node Eval_exeInter(Node head, Node tmp, int level, int arguCount, int& printType);
    Node Eval_cons(Node cur, int level, int numOfargu, string name, int printType);
    Node Eval_def(Node root, Node cur, int level, int arguCount, int printType);
    Node Eval_quote( Node head, Node cur, int numOfargu, string name, int printType );
    Node Eval_car(Node cur, int level, int arguCount, int printType);
    Node Eval_cdr(Node cur, int level, int arguCount, int printType);
    Node Eval_begin(Node cur, int level, int arguCount, int printType);
    Node Eval_list(Node cur, int level, int arguCount, int printType);
    Node Eval_if(Node head, Node cur, int level, int arguCount, int printType);
    Node Eval_cond(Node head, Node cur, int level, int arguCount, int printType);
    void Cond_FormatCheck(Node head, Node cur);
    Node Condition(Node head, Node cur, int level, int printType, bool& stop, Node result, int& nunOfCond); // 執行由Eval_cond丟過來的條件
    Node Eval_add(Node cur, int level, int arguCount, int printType);
    Node Eval_sub(Node cur, int level, int arguCount, int printType);
    Node Eval_mul(Node cur, int level, int arguCount, int printType);
    Node Eval_div(Node cur, int level, int arguCount, int printType);
    Node Eval_not(Node cur, int level, int arguCount, int printType);
    Node Eval_and(Node cur, int level, int arguCount, int printType); // 如果遇到nil就直接輸出 不然就往下走到最後一個
    Node Eval_or(Node cur, int level, int arguCount, int printType);  // print first non-nil argu
    Node Eval_IsInteger(Node cur, int level, int arguCount, int printType);
    Node Eval_IsBoolean(Node cur, int level, int arguCount, int printType);
    Node Eval_IsNumber(Node cur, int level, int arguCount, int printType, string name);
    Node Eval_IsString(Node cur, int level, int arguCount, int printType);
    Node Eval_IsSymbol(Node cur, int level, int arguCount, int printType);
    Node Eval_IsAtom(Node cur, int level, int arguCount, int printType);
    Node Eval_IsNull(Node cur, int level, int arguCount, int printType);
    Node Eval_IsPair(Node cur, int level, int arguCount, int printType);
    Node Eval_IsList(Node cur, int level, int arguCount, int printType);
    Node Eval_append(Node cur, int level, int arguCount, int printType);
    Node Eval_great_than(Node cur, int level, int arguCount, int printType);
    Node Eval_less_than(Node cur, int level, int arguCount, int printType);
    Node Eval_no_less_than(Node cur, int level, int arguCount, int printType);
    Node Eval_no_more_than(Node cur, int level, int arguCount, int printType);
    Node Eval_equal_to(Node cur, int level, int arguCount, int printType);
    Node Eval_string_great_than(Node cur, int level, int arguCount, int printType);
    Node Eval_string_less_than(Node cur, int level, int arguCount, int printType);
    Node Eval_string_equal_to(Node cur, int level, int arguCount, int printType);
    Node Eval_eqv(Node cur, int level, int arguCount, int printType);
    Node Eval_equal_tree(Node cur, int level, int arguCount, int printType);
    Node CloneTree(Node cur, int& p_count); // 只能複製 同一 S_exp 的東西 in the same pair of parses
    Node GetBind(Node cur);
    Node NewBind(Node bind, string name, int type);
    Node GetNodeData(Node cur);
    bool CompareTwoTree(Node root1, Node root2); // 比較2棵樹是否長的一樣
    bool IsDataSame(Node root1, Node root2);     // 檢查該root連接的node data是否一樣
    bool IsSystemFunc(string name); // 系統內原有的function
    bool IsBinding(string name, Node& cur);
    bool IsThereDot(Node cur); // like CheckPureList, but do not throw error
    int CheckNumOfArgument(Node cur);
    float StringToNum(string str);
    string GetStr(string input);

    Execuate() {
        mPrintRoot = NULL;
        mBindRoot = NULL;
        mPrintType = 0;
        mNumOfSpace = 0;
        mPreTokenIsRightParen = true;
    } // Execuate()
};

class Exception {
public:
    int mType;
    Execuate mExec;
    string mName;
    Execuate mExe;

    Exception() {
        // initial
        mType = 999;
        mName = "";
    } // Exception()

    Exception( int type, int line, int column ) {
        // do not print token name
        mType = type; // setting mType

        if (type == NO_CLOSING_QUOTE) {
            cout << "ERROR (no closing quote) : END-OF-LINE encountered at Line ";
            cout << line << " Column " << column << endl << endl;
        } // if
        else if (type == ERROR_EOF)
            cout << "ERROR (no more input) : END-OF-FILE encountered";
        else if (type == QUIT_SCHEME)
            ; // do nothing
    } // Exception()

    Exception( Token token, int type ) {
        mType = type;

        if (type == UNEXPECTED_ATOM_OR_LEFT_PAREN) {
            cout << "ERROR (unexpected token) : atom or '(' expected when token at ";
            cout << "Line " << token.line << " Column " << token.column << " is >>";
            cout << token.name << "<<" << endl << endl;
        } // if
        else if (type == UNEXPECTED_RIGHT_PAREN) {
            cout << "ERROR (unexpected token) : ')' expected when token at ";
            cout << "Line " << token.line << " Column " << token.column << " is >>";
            cout << token.name << "<<" << endl << endl;
        } // else if
    } // Exception()

    Exception( string name, int type ) {
        mType = type;
        if (type == UNBOUND_SYMBOL)
            cout << "ERROR (unbound symbol) : " << name << endl << endl;
        else if (type == ERROR_LEVEL) {
            if (name.compare("clean-environment") == 0)
                cout << "ERROR (level of CLEAN-ENVIRONMENT)" << endl << endl;
            else if (name.compare("define") == 0)
                cout << "ERROR (level of DEFINE)" << endl << endl;
            else if (name.compare("exit") == 0)
                cout << "ERROR (level of EXIT)" << endl << endl;
        } // else if
        else if (type == ERROR_NUM_OF_ARGUMENTS)
            cout << "ERROR (incorrect number of arguments) : " << name << endl << endl;
        else if (type == JUST_ERROR)
            cout << "ERROR (division by zero) : " << name << endl << endl;
        else if (type == PROCEDURE)
            cout << "#<procedure " << name << ">" << endl << endl;
    } // Exception()

    Exception( string name, Node result ) {
        mType = ERROR_NUM_OF_ARGUMENTS;
        cout << "ERROR (" << name << " with incorrect argument type) : ";
        mExe.PrintTree( result, ERROR_PRINT );
        cout << endl;
    } // Exception()

    Exception( string name, Node result, int type ) {
        mType = type;
        cout << "ERROR (incorrect number of arguments) : ";
        mExe.PrintTree( result, ERROR_PRINT );
        cout << endl;
    } // Exception()

    Exception( int type, Node result ) {
        mType = type;
        if (type == NON_LIST)
            cout << "ERROR (non-list) : ";
        else if (type == DEF_FORMAT)
            cout << "ERROR (DEFINE format) : ";
        else if (type == NO_RETURN_RESULT)
            cout << "ERROR (no return value) : ";
        else if (type == COND_FORMAT)
            cout << "ERROR (COND format) : ";
        else if (type == NON_FUNCTION) {
            cout << "ERROR (attempt to apply non-function) : ";
            mExe.PrintTree(result, NORMAL);
            cout << endl;
            return;
        } // else if

        mExe.PrintTree(result, ERROR_PRINT);
        cout << endl;
    } // Exception()

    Exception( int type ) {
        mType = type;
        cout << "environment cleaned" << endl << endl;
    } // Exception()
};

int main() {
    int testNum = 0;
    char ch = '\0';
    bool isExit = false;

    Parser parser;
    Execuate exec;

    // input testNum
    cin >> testNum;
    scanf( "%c", &ch ); // get '\n'

    cout << "Welcome to OurScheme!" << endl << endl;

    while ( !isExit ) {
        try {
            cout << "> ";
            parser.ReadExp();
            parser.BuildTree();
            exec.mPrintRoot = exec.EvalSExp( parser.mRoot, 0, exec.mPrintType );
            exec.PrintEval( exec.mPrintRoot, exec.mPrintType );
            parser.InitialScheme();
            exec.InitialExe();
            cout << endl;
        } // try
        catch ( Exception* error ) {
            exec.InitialExe();
            parser.DealWithError( error->mType );
            if ( error->mType == ERROR_EOF || error->mType == QUIT_SCHEME )
                isExit = true;
        } // catch
    } // while

    cout << endl;
    cout << "Thanks for using OurScheme!" << endl; // end
} // main()

// Scanner
void Scanner::SkipWhiteSpace() {
    char ch = '\0';

    while ((scanf("%c", &ch) != EOF)) {
        mColumn++;

        if (ch == '\n') { // new line
            mColumn = 0;
            mLine++;
        } // if

        ch = cin.peek();
        if (ch == '\n' || ch == '\t' || ch == ' ')
            ; // do nothing    
        else
            return; // get the first non-white-space char
    } // while
} // Scanner::SkipWhiteSpace()

void Scanner::Gettoken( Token& token ) {
    int ch = 0;
    char tmp[80] = { '\0' };

    ch = cin.peek();
    if (ch == '\n' || ch == '\t' || ch == ' ') // 把white space讀乾淨
        SkipWhiteSpace();

    ch = cin.peek(); // 先偷看一下 但不讀入
    if (ch == '(' || ch == ')')
        // maybe '(' or '()'
        GetParen(token);
    else if (ch == '#')
        // maybe '#t' or '#f' or symbol
        GetPoundSign(token);
    else if (ch == '\'' || ch == '"')
        GetQuote(token);
    else if (ch == ';')
        GetSemiColon(token);
    else if (ch == '+' || ch == '-' || ch == '.' ||
        (ch >= '0' && ch <= '9'))
        GetIntOrFloat(token); // 在裡面做詳細分類
    else if (ch == EOF) {
        token.name = "";
        token.type = END_OF_LINE;
    } // else if
    else
        GetSymbol(token, tmp);

} // Scanner::Gettoken()

void Scanner::GetParen( Token& token ) {
    int peek = 0, type = 999, tmp_column = -100;
    char ch = '\0';
    char tmp[3] = { '\0' };

    peek = cin.peek();
    if (peek == '(') {
        // maybe '(' or '()'
        type = LEFT_PAREN;
        scanf("%c", &ch); // get '('
        mColumn++;
        tmp_column = mColumn;
        tmp[0] = ch;

        peek = cin.peek();
        if (peek == '\n' || peek == '\t' || peek == ' ')
            SkipWhiteSpace();

        peek = cin.peek();
        if (peek == ')') {
            // get '()'
            scanf("%c", &ch);
            mColumn++;
            tmp[1] = ch;
            type = NILS;
        } // if
    } // if
    else {
        // get ')'
        type = RIGHT_PAREN;
        scanf("%c", &ch);
        mColumn++;
        tmp_column = mColumn;
        tmp[0] = ch;
    } // else

    // set token inf.
    token.column = tmp_column;
    token.line = mLine;
    token.name = CharToString(tmp);
    token.type = type;
} // Scanner::GetParen()

void Scanner::GetSemiColon( Token& token ) {
    char tmp[3] = { '\0' };
    char ch = '\0';

    scanf("%c", &ch); // get ';'
    tmp[0] = ch;
    mColumn++;
    token.column = mColumn;
    token.line = mLine;
    GetGarbage(); // 把註解吃掉
    // scanf( "%c", &ch ); // get '\n'

    token.name = CharToString(tmp);
    token.type = SEMI_COLON;
} // Scanner::GetSemiColon()

void Scanner::GetPoundSign( Token& token ) {
    int peek = 0, type = 999, i = 2;
    char ch = '\0';
    char tmp[80] = { '\0' };

    scanf("%c", &ch); // get '#'
    mColumn++;
    token.column = mColumn;
    token.line = mLine;
    tmp[0] = ch;

    peek = cin.peek();
    if (peek == 't' || peek == 'f') {
        // maybe '#t' or '#f'
        scanf("%c", &ch); // get 't' or 'f'
        mColumn++;
        tmp[1] = ch;

        if (peek == 't') // '#t'
            type = TRUE;
        else // '#f'
            type = NILS;

        peek = cin.peek();
        if (peek == ' ' || peek == '\t' || peek == '\n' ||
            peek == '(' || peek == ')' ||
            peek == '\'' || peek == '"' || peek == ';') {
            // get '#t' or '#f'
            token.name = CharToString(tmp);
            token.type = type;
            return;
        } // if
        else
            GetSymbol(token, tmp);

    } // if
    else if (peek == ' ' || peek == '\t' || peek == '\n') {
        token.name = CharToString(tmp);
        token.type = SYMBOL;
        return;
    } // else if
    else
        GetSymbol(token, tmp);

} // Scanner::GetPoundSign()

void Scanner::GetSymbol( Token& token, char ch[80] ) {
    int peek, type = 999;
    int i = strlen(ch); // index
    char tmp = '\0';
    string s = "";
    bool stop = false;

    token.line = mLine;
    peek = cin.peek();
    if (peek == ' ' || peek == '\t' || peek == '\n' ||
        peek == '(' || peek == ')' ||
        peek == '\'' || peek == '"' || peek == ';')
        stop = true;

    while (!stop && (scanf("%c", &tmp) != EOF)) {
        mColumn++;
        if (token.column == 0)
            token.column = mColumn;

        ch[i] = tmp;
        peek = cin.peek(); // 偷看下一個char

        if (peek == ' ' || peek == '\t' || peek == '\n' ||
            peek == '(' || peek == ')' ||
            peek == '\'' || peek == '"' || peek == ';')
            stop = true;

        i++;
    } // while

    s = CharToString(ch);
    if (s.compare("t") == 0)
        type = TRUE;
    else if (s.compare("nil") == 0)
        type = NILS;
    else
        type = SYMBOL;

    token.name = s;
    token.type = type;
} // Scanner::GetSymbol()

void Scanner::GetQuote( Token& token ) {
    char ch = '\0', tmp[80] = { '\0' };
    char escape[3] = { '\0' };
    int peek, i = 1;

    peek = cin.peek();
    scanf("%c", &ch); // maybe '"' or '\''
    mColumn++;

    // set token address
    token.line = mLine;
    token.column = mColumn;

    tmp[0] = ch;
    if (peek == '\'') {
        // get '\''
        token.name = CharToString(tmp);
        token.type = QUOTE;
        return;
    } // if
    else {
        // String
        peek = cin.peek();
        while (peek != '"') {
            if (peek == '\\') {
                scanf("%c", &ch);
                mColumn++;
                escape[0] = ch; // get '\\'

                peek = cin.peek();
                if (peek == '\n')
                    throw new Exception(NO_CLOSING_QUOTE, mLine, mColumn + 1);

                scanf("%c", &ch); // maybe 'n' or '\\' or '"' or '\t' or other
                mColumn++;
                if (ch == 'n')
                    tmp[i] = '\n';
                else if (ch == 't')
                    tmp[i] = '\t';
                else if (ch == '\\')
                    tmp[i] = '\\';
                else if (ch == '"')
                    tmp[i] = '"';
                else {
                    // other
                    escape[1] = ch;
                    strcat(tmp, escape);
                    escape[0] = '\0';
                    i = strlen(tmp) - 1;
                } // else
            } // if
            else if (peek == '\n')
                throw new Exception(NO_CLOSING_QUOTE, mLine, mColumn + 1);
            else {
                scanf("%c", &ch);
                mColumn++;
                tmp[i] = ch;
            } // else 

            peek = cin.peek();
            i++;
        } // while

        // maybe EOF or peek( '"' )
        if (peek == EOF) // ERROR
            throw new Exception(NO_CLOSING_QUOTE, mLine, mColumn + 1);
        else {
            scanf("%c", &ch); // get '"'
            mColumn++;
            i = strlen(tmp);
            tmp[i] = ch;
            token.name = CharToString(tmp);
            token.type = STRING;
        } // else
    } // else
} // Scanner::GetQuote()

void Scanner::GetIntOrFloat( Token& token ) {
    int peek, i = 0;
    char ch = '\0';
    char tmp[80] = { '\0' };

    peek = cin.peek();
    if (peek == '.') { // maybe float or symbol
        scanf("%c", &ch); // get '.'
        mColumn++;
        token.column = mColumn;
        token.line = mLine;
        tmp[0] = ch; // save '.'
        i++;
        peek = cin.peek();
        if (peek == ' ' || peek == '\t' || peek == '\n' ||
            peek == '(' || peek == ')' || peek == EOF ||
            peek == '\'' || peek == '"' || peek == ';') {
            // DOT
            token.name = CharToString(tmp);
            token.type = DOT;
            return;
        } // if
        else if (peek < '0' || peek > '9')
            // not '0' ~ '9'
            GetSymbol(token, tmp);
        else // '0' ~ '9'
            GetFloat(token, tmp);

    } // if
    else if (peek == '+' || peek == '-') {
        scanf("%c", &ch); // get '+' or '-'
        mColumn++;
        token.column = mColumn;
        token.line = mLine;
        tmp[0] = ch; // save '+' or '-'
        i++;
        peek = cin.peek();
        if (peek == ' ' || peek == '\t' || peek == '\n' ||
            peek == '(' || peek == ')' || peek == EOF ||
            peek == '\'' || peek == '"' || peek == ';') {
            token.name = CharToString(tmp);
            token.type = SYMBOL;
            return;
        } // if
        else if (peek == '.') {
            scanf("%c", &ch); // get '.'
            tmp[1] = ch;
            mColumn++;
            peek = cin.peek();
            if (peek == ' ' || peek == '\t' || peek == '\n' ||
                peek == '(' || peek == ')' || peek == EOF ||
                peek == '\'' || peek == '"' || peek == ';') {
                // DOT
                token.name = CharToString(tmp);
                token.type = SYMBOL;
                return;
            } // if
            else if (peek < '0' || peek > '9')
                GetSymbol(token, tmp);
            else // '0' ~ '9'
                GetFloat(token, tmp);

        } // else if
        else if (peek < '0' || peek > '9')
            // not '0' ~ '9'
            GetSymbol(token, tmp);
        else // '0' ~ '9'
            GetInt(token, tmp);

    } // else if
    else  // '0' ~ '9'
        GetInt(token, tmp); // int or float
} // Scanner::GetIntOrFloat()

void Scanner::GetFloat( Token& token, char tmp[80] ) {
    // 已經get '.'
    char ch = '\0';
    int peek;
    int i = strlen(tmp);

    peek = cin.peek();
    if (peek == ' ' || peek == '\t' || peek == '\n' ||
        peek == '(' || peek == ')' ||
        peek == '\'' || peek == '"' || peek == ';') {
        token.name = CharToString(tmp);
        token.type = FLOAT;
        return;
    } // if 
    else if (peek > '9') {
        GetSymbol(token, tmp);
        return;
    } // else if
    else if (peek < '0') {
        GetSymbol(token, tmp);
        return;
    } // else if

    while (scanf("%c", &ch) != EOF) {
        tmp[i] = ch;
        mColumn++;
        peek = cin.peek();

        if (peek == ' ' || peek == '\t' || peek == '\n' ||
            peek == '(' || peek == ')' ||
            peek == '\'' || peek == '"' || peek == ';') {
            token.name = CharToString(tmp);
            token.type = FLOAT;
            return;
        } // if
        else if (peek > '9') {
            GetSymbol(token, tmp);
            return;
        } // else if
        else if (peek < '0') {
            GetSymbol(token, tmp);
            return;
        } // else if
        else  // '0' ~ '9'
            ; // do nothing

        i++;
    } // while

    token.name = CharToString(tmp);
    token.type = FLOAT;
} // Scanner::GetFloat()

void Scanner::GetInt( Token& token, char tmp[80] ) {
    // 還是有可能碰到float
    int peek;
    int i = strlen(tmp);
    char ch = '\0';

    token.line = mLine;

    while (scanf("%c", &ch) != EOF) {
        tmp[i] = ch;

        mColumn++;
        if (token.column == 0)
            token.column = mColumn;

        peek = cin.peek();

        if (peek == '.') {
            scanf("%c", &ch); // get '.'
            mColumn++;
            tmp[i + 1] = ch;  // save '.'
            GetFloat(token, tmp);
            return;
        } // if
        else if (peek == ' ' || peek == '\t' || peek == '\n' ||
            peek == '(' || peek == ')' ||
            peek == '\'' || peek == '"' || peek == ';') {
            token.name = CharToString(tmp);
            token.type = INT;
            return;
        } // else if
        else if (peek > '9') {
            GetSymbol(token, tmp);
            return;
        } // else if
        else if (peek < '0') {
            GetSymbol(token, tmp);
            return;
        } // else if
        else // '0' ~ '9'
            ; // do nothing

        i++;
    } // while

    token.name = CharToString(tmp);
    token.type = INT;
} // Scanner::GetInt()

void Scanner::GetGarbage() {
    int peek = 0;
    char ch = '\0';
    peek = cin.peek();

    while (peek != '\n') {
        // 讀到enter作結束
        scanf("%c", &ch);
        peek = cin.peek();

        if (peek == EOF)
            return;
    } // while
    // scanf( "%c", &ch ); // get '\n'
} // Scanner::GetGarbage()

void Scanner::Initial() {
    char ch = '\0';
    int peek = 0;

    peek = cin.peek();
    while (peek == '\t' || peek == ' ') {
        scanf("%c", &ch);
        mColumn++;
        peek = cin.peek();
    } // while

} // Scanner::Initial()

string Scanner::CharToString( char ch[80] ) {
    string s = "";
    for (int i = 0; ch[i] != '\0'; i++)
        s = s + ch[i];

    return s;
} // Scanner::CharToString()

// Parser
void Parser::InitialToken() {
    mPeekToken.name = "";
    mPeekToken.column = 0;
    mPeekToken.line = 0;
    mPeekToken.type = INITIAL;
} // Parser::InitialToken()

void Parser::InitialScheme() {
    char ch = '\0';
    int peek = 0;

    mScanner.mColumn = 0;
    mScanner.mLine = 1;

    peek = cin.peek();
    if (peek == '\t' || peek == ' ')
        mScanner.Initial();

    peek = cin.peek();
    if (peek == ';') {
        while (peek == ';') {
            mScanner.Gettoken(mPeekToken);
            peek = cin.peek();
        } // while

        // 修改 regular1 InitialScheme(), GetGarbage(), GetSemiColon(), DealWithError()
        scanf("%c", &ch); // get '\n'
        mScanner.mColumn = 0;
        mScanner.mLine = 1;
    } // if
    else if (peek == '\n') {
        scanf("%c", &ch); // get '\n'
        mScanner.mColumn = 0;
        mScanner.mLine = 1;
    } // else if
    // DeleteTree( mRoot );
    mRoot = NULL;
    InitialToken();
    mTokenList.clear();
} // Parser::InitialScheme()

void Parser::DealWithError( int type ) {
    char ch = '\0';
    int peek = 0;

    if (type == UNEXPECTED_ATOM_OR_LEFT_PAREN || type == UNEXPECTED_RIGHT_PAREN ||
        type == NO_CLOSING_QUOTE || type == ERROR_EOF || type == QUIT_SCHEME) {
        mScanner.GetGarbage(); // 把同一行 input 吃掉
        scanf("%c", &ch); // get '\n'
        mScanner.mColumn = 0;
        mScanner.mLine = 1;
    } // if
    else {
        mScanner.mColumn = 0;
        peek = cin.peek();
        if (peek == '\t' || peek == ' ')
            mScanner.Initial();

        peek = cin.peek();
        if (peek == ';') {
            while (peek == ';') {
                mScanner.Gettoken(mPeekToken);
                peek = cin.peek();
            } // while

            scanf("%c", &ch); // get '\n'
        } // if
        else if (peek == '\n')
            scanf("%c", &ch); // get '\n'

        mScanner.mLine = 1;
    } // else


    mTokenList.clear();
    InitialToken();    // initialization mPeekToken
} // Parser::DealWithError()

void Parser::ReadExp() {
    int type = 0;

    type = PeekToken();
    if (type == ATOM) {
        Atom(); // get <ATOM>
        if (mCurToken.mName.compare("exit") == 0) {
            if (mTokenList.size() == 1)
                return;

            type = PeekToken();
            if (type == RIGHT_PAREN && mTokenList.size() == 2) {
                // ( exit
                GetTerminal();
                mTokenList.push_back(mCurToken); // get ')'
                throw new Exception(QUIT_SCHEME, 0, 0);
            } // if
        } // if

        return;
    } // if
    else if (type == LEFT_PAREN) {
        GetTerminal();
        mTokenList.push_back(mCurToken); // get '('

        type = PeekToken();
        if (type == ATOM || type == QUOTE || type == LEFT_PAREN)
            ReadExp();
        else
            throw new Exception(mPeekToken, UNEXPECTED_ATOM_OR_LEFT_PAREN);

        type = PeekToken();
        while (type == ATOM || type == QUOTE || type == LEFT_PAREN) {
            ReadExp();
            type = PeekToken();
        } // while

        type = PeekToken();
        if (type == DOT) {
            GetTerminal();
            mTokenList.push_back(mCurToken); // get '.'
            ReadExp();
        } // if

        type = PeekToken();
        if (type == RIGHT_PAREN) {
            GetTerminal();
            mTokenList.push_back(mCurToken); // get ')'
            return;
        } // else if
        else if (type == END_OF_LINE)
            throw new Exception(ERROR_EOF, 0, 0);
        else
            throw new Exception(mPeekToken, UNEXPECTED_RIGHT_PAREN);

    } // else if
    else if (type == QUOTE) {
        GetTerminal();
        mTokenList.push_back(mCurToken); // get '\''
        ReadExp();
    } // else if
    else if (type == END_OF_LINE)
        throw new Exception(ERROR_EOF, 0, 0);
    else
        throw new Exception(mPeekToken, UNEXPECTED_ATOM_OR_LEFT_PAREN);

} // Parser::ReadExp()

void Parser::Atom() {
    GetTerminal(); // get <ATOM>
    mTokenList.push_back(mCurToken);
} // Parser::Atom()

void Parser::GetTerminal() {
    // 從mPeekToken拿東西 放到mCurToken
    mCurToken.mName = mPeekToken.name;
    mCurToken.mType = mPeekToken.type;

    InitialToken(); // 拿走後要初始化
} // Parser::GetTerminal()

void Parser::BuildTree() {
    mRoot = NULL; // initial
    Terminal term;
    int dot_paren = 0; // ".("
    int left_paren = 0;

    CheckSpecialExit();
    CheckQuote(); // 展開成好處理的樣子

    int count = DotParenCount();
    DeleteDotParen(count);

    // root node
    term = mTokenList[0];
    mTokenList.erase(mTokenList.begin()); // 刪除最前面的
    mRoot = SetLeaf(term);

    // other node
    while (!mTokenList.empty()) {
        AddNewLeaf(mRoot, dot_paren, left_paren);
    } // while
} // Parser::BuildTree()

void Parser::CheckSpecialExit() {
    if (mTokenList.size() == 5) {
        if (mTokenList[1].mName.compare("exit") == 0)
            if (mTokenList[2].mType == DOT && mTokenList[3].mType == NILS)
                throw new Exception(QUIT_SCHEME, 0, 0);
    } // if
} // Parser::CheckSpecialExit()

void Parser::AddNewLeaf( Node& cur, int& dot_paren, int& left_paren ) {
    Terminal term, tmp;
    tmp.mName = "";
    tmp.mType = INITIAL;

    if (cur->left == NULL) // 直接建在左邊
        ; // do nothing
    else if (cur->right != NULL) {
        AddNewLeaf(cur->right, dot_paren, left_paren); // 右邊還有路
        return;
    } // else if
    else // 左邊有東西，右邊沒路走，建在右邊
        ; // do nothing

    term = mTokenList[0]; // get

    if (term.mType == SYMBOL || term.mType == INT || term.mType == FLOAT ||
        term.mType == STRING || term.mType == TRUE) {
        if (cur->left == NULL) {
            cur->left = SetLeaf(term);
            mTokenList.erase(mTokenList.begin());
        } // if
        else {
            // 建立一個 '.' 分開左右子樹
            cur->right = SetLeaf(tmp);
            cur->right->left = SetLeaf(term);
            mTokenList.erase(mTokenList.begin());
        } // else
    } // if
    else if (term.mType == NILS) {
        if (cur->type == DOT) {
            mTokenList.erase(mTokenList.begin()); // 直接略過
            cur->type = INITIAL;
        } // if
        else {
            if (cur->left == NULL) {
                cur->left = SetLeaf(term);
                mTokenList.erase(mTokenList.begin());
            } // if
            else
                cur->right = SetLeaf(tmp);
        } // else 
    } // else if
    else if (term.mType == DOT) {
        if (cur->left == NULL)
            cur->left = SetLeaf(term);
        else
            cur->right = SetLeaf(term);

        mTokenList.erase(mTokenList.begin());
    } // else if
    else if (term.mType == LEFT_PAREN) {
        left_paren++;
        if (cur->left == NULL) {
            cur->left = SetLeaf(term);
            mTokenList.erase(mTokenList.begin()); // 去掉'('

            term = mTokenList[0];
            while (term.mType != RIGHT_PAREN) {
                AddNewLeaf(cur->left, dot_paren, left_paren);
                term = mTokenList[0];
            } // while

            AddNewLeaf(cur->left, dot_paren, left_paren); // get ')'
        } // if
        else {
            cur->right = SetLeaf(term);
            mTokenList.erase(mTokenList.begin()); // 去掉'('

            term = mTokenList[0];
            while (term.mType != RIGHT_PAREN) {
                AddNewLeaf(cur->right, dot_paren, left_paren);
                term = mTokenList[0];
            } // while

            AddNewLeaf(cur->right, dot_paren, left_paren); // get ')'
        } // else 
    } // else if
    else if (term.mType == PAREN_QUOTE) {
        cur->left = SetLeaf(term);
        mTokenList.erase(mTokenList.begin()); // 去掉'\''
    } // else if
    else if (term.mType == RIGHT_PAREN) {
        if (dot_paren != 0)
            dot_paren--;
        else if (left_paren != 0) {
            left_paren--;
            if (cur->left == NULL)
                cur->left = SetLeaf(term);
            else {
                cur->right = SetLeaf(tmp);
                cur->right->left = SetLeaf(term);
            } // else
        } // else if
        else { // left_paren = 0, dot_paren = 0
            if (cur->left == NULL)
                cur->left = SetLeaf(term);
            else {
                cur->right = SetLeaf(tmp);
                cur->right->left = SetLeaf(term);
            } // else
        } // else 

        mTokenList.erase(mTokenList.begin()); // 去掉')'
    } // else if
} // Parser::AddNewLeaf()

void Parser::DeleteTree( Node& cur ) {
    if (cur != NULL) {
        DeleteTree(cur->left);
        DeleteTree(cur->right);
        delete cur;
        cur = NULL;
    } // if
} // Parser::DeleteTree()

void Parser::CheckQuote() {
    int numOfQuote = 0, index = -1;
    for (int i = 0; i < mTokenList.size(); i++) {
        if (mTokenList[i].mType == QUOTE)
            numOfQuote++;
    } // for

    while (numOfQuote > 0) {
        index = QuoteIsAt(); // get first quote
        ExtandQuote(index);
        numOfQuote--;
    } // while
} // Parser::CheckQuote()

void Parser::ExtandQuote( int index ) {
    int i = index + 2;  // Quote 的後一個element
    int numOfParen = 0; // num of left paren
    bool stop = false;
    Terminal tmp;
    tmp.mName = "(";
    tmp.mType = LEFT_PAREN;

    mTokenList[index].mType = PAREN_QUOTE;         // "( Quote"
    mTokenList.insert(mTokenList.begin() + index, tmp); // 在 Quote 之前插入 "("

    while (i < mTokenList.size() && !stop) {

        // 類似 stack 找到成對paren
        if (mTokenList[i].mType == LEFT_PAREN)
            numOfParen++;
        else if (mTokenList[i].mType == RIGHT_PAREN)
            numOfParen--;
        else
            ; // do nothing

        if (numOfParen == 0 && mTokenList[i].mType != QUOTE)
            stop = true;

        i++;
    } // while

    tmp.mName = ")";
    tmp.mType = RIGHT_PAREN;
    if (i == mTokenList.size())
        mTokenList.push_back(tmp);
    else
        mTokenList.insert(mTokenList.begin() + i, tmp); // "( Quote )"

} // Parser::ExtandQuote()

void Parser::DeleteDotParen( int count ) {
    int index = 0;

    while (count != 0) {
        index = DotParenAt();
        DeleteDot(index);
        count--;
    } // while

} // Parser::DeleteDotParen()

void Parser::DeleteDot( int index ) {
    bool stop = false;
    int numOfParen = 1; // num of left paren
    int i = index;

    mTokenList.erase(mTokenList.begin() + index); // delete DOT
    mTokenList.erase(mTokenList.begin() + index); // delete LP

    while (i < mTokenList.size() && !stop) {
        // 類似 stack 找到成對paren
        if (mTokenList[i].mType == LEFT_PAREN)
            numOfParen++;
        else if (mTokenList[i].mType == RIGHT_PAREN)
            numOfParen--;
        else
            ; // do nothing

        if (numOfParen == 0 && mTokenList[i].mType == RIGHT_PAREN)
            stop = true;

        i++;
    } // while

    mTokenList.erase(mTokenList.begin() + i - 1);   // delete RP
} // Parser::DeleteDot()

int Parser::DotParenAt() {
    int index = 0;
    for (int i = 0; i < mTokenList.size(); i++) {
        if (mTokenList[i].mType == DOT && mTokenList[i + 1].mType == LEFT_PAREN)
            if (mTokenList[i + 2].mType != PAREN_QUOTE)
                return i;
    } // for

    return index;
} // Parser::DotParenAt()

int Parser::DotParenCount() {
    int count = 0;
    for (int i = 0; i < mTokenList.size(); i++) {
        if (mTokenList[i].mType == DOT && mTokenList[i + 1].mType == LEFT_PAREN)
            if (mTokenList[i + 2].mType != PAREN_QUOTE)
                count++;
    } // for

    return count;
} // Parser::DotParenCount()

int Parser::QuoteIsAt() {
    for (int i = 0; i < mTokenList.size(); i++) {
        if (mTokenList[i].mType == QUOTE)
            return i;
    } // for

    return 0;
} // Parser::QuoteIsAt()

int Parser::PeekToken() {
    if (mPeekToken.type == INITIAL) {
        mScanner.Gettoken(mPeekToken);
        while (mPeekToken.type == SEMI_COLON) {
            mScanner.Gettoken(mPeekToken);
        } // while
    } // if

    if (mPeekToken.type == SYMBOL || mPeekToken.type == INT || mPeekToken.type == FLOAT ||
        mPeekToken.type == STRING || mPeekToken.type == NILS || mPeekToken.type == TRUE)
        return ATOM;

    return mPeekToken.type;
} // Parser::PeekToken()

Node Parser::SetLeaf( Terminal term ) {
    Node tmp = new Token;
    tmp->name = term.mName;
    tmp->type = term.mType;
    tmp->left = NULL;
    tmp->right = NULL;
    return tmp;
} // Parser::SetLeaf()

// Execuate
void Execuate::InitialExe() {
    mPrintRoot = NULL;
    mPrintType = 0;
    mNumOfSpace = 0;
    mPreTokenIsRightParen = true;
} // Execuate::InitialExe()

Node Execuate::EvalSExp( Node cur, int level, int& printType ) {
    Node head = cur, tmp = NULL;
    string curFunc = "";
    int arguCount = 0;

    if (cur->type == STRING || cur->type == INT || cur->type == FLOAT ||
        cur->type == NILS || cur->type == TRUE)
        return cur;
    else if (cur->type == SYMBOL) {
        // Check whether it is bound to an S-expression or an internal function
        if (IsSystemFunc(cur->name))
            throw new Exception(cur->name, PROCEDURE);
        else if (IsBinding(cur->name, cur)) {
            if (cur->type == INTER_FUNCTION)
                throw new Exception(cur->right->name, PROCEDURE);
            else {
                printType = NORMAL;
                return cur->right;
            } // else 
        } // else if
        else
            throw new Exception(cur->name, UNBOUND_SYMBOL); // unbound
    } // else if
    else {
        // cur -> name == LP
        CheckPureList(head->right, head); // check pure list

        // first argument is an atom X, which is not a symbol
        cur = cur->left;
        if (cur->type == STRING || cur->type == INT || cur->type == FLOAT ||
            cur->type == NILS || cur->type == TRUE)
            throw new Exception(NON_FUNCTION, cur);
        else if (cur->type == SYMBOL || cur->type == PAREN_QUOTE) {
            curFunc = cur->name;
            // check whether  curFunc is the name of a func
            if (IsSystemFunc(curFunc)) {
                arguCount = CheckNumOfArgument(head->right);
                cur = head->right;
                level++;
                if (curFunc.compare("cons") == 0) {
                    head = Eval_cons(cur, level, arguCount, curFunc, printType); // EvalCons
                    printType = NORMAL;
                    return head;
                } // if
                else if (curFunc.compare("define") == 0) {
                    head = Eval_def(head, cur, level, arguCount, printType);
                    printType = DF;
                    return head;
                } // else if
                else if (curFunc.compare("exit") == 0)
                    Eval_exit(level, arguCount);
                else if (curFunc.compare("clean-environment") == 0) {
                    Eval_clean_env(level, arguCount);
                    throw new Exception(CLEAN_ENV);
                } // else if
                else if (curFunc.compare("car") == 0) {
                    head = Eval_car(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("cdr") == 0) {
                    head = Eval_cdr(head, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("begin") == 0) {
                    head = Eval_begin(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("list") == 0) {
                    head = Eval_list(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("if") == 0) {
                    head = Eval_if(head, cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("cond") == 0) {
                    head = Eval_cond(head, cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("'") == 0 || curFunc.compare("quote") == 0) {
                    head = Eval_quote( head, cur, arguCount, curFunc, printType );
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("+") == 0) {
                    head = Eval_add(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("-") == 0) {
                    head = Eval_sub(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("*") == 0) {
                    head = Eval_mul(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("/") == 0) {
                    head = Eval_div(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("not") == 0) {
                    head = Eval_not(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("and") == 0) {
                    head = Eval_and(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("or") == 0) {
                    head = Eval_or(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("integer?") == 0) {
                    head = Eval_IsInteger(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("number?") == 0 || curFunc.compare("real?") == 0) {
                    // number? = real?
                    head = Eval_IsNumber(cur, level, arguCount, printType, curFunc);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("string?") == 0) {
                    head = Eval_IsString(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("atom?") == 0) {
                    head = Eval_IsAtom(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("null?") == 0) {
                    head = Eval_IsNull(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("symbol?") == 0) {
                    head = Eval_IsSymbol(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("boolean?") == 0) {
                    head = Eval_IsBoolean(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("pair?") == 0) {
                    head = Eval_IsPair(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("list?") == 0) {
                    head = Eval_IsList(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("string-append") == 0) {
                    head = Eval_append(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare(">") == 0) {
                    head = Eval_great_than(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("<") == 0) {
                    head = Eval_less_than(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare(">=") == 0) {
                    head = Eval_no_less_than(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("<=") == 0) {
                    head = Eval_no_more_than(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("=") == 0) {
                    head = Eval_equal_to(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("string>?") == 0) {
                    head = Eval_string_great_than(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("string<?") == 0) {
                    head = Eval_string_less_than(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("string=?") == 0) {
                    head = Eval_string_equal_to(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
                else if (curFunc.compare("eqv?") == 0) {
                    head = Eval_eqv(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if 
                else if (curFunc.compare("equal?") == 0) {
                    head = Eval_equal_tree(cur, level, arguCount, printType);
                    printType = NORMAL;
                    return head;
                } // else if
            } // if
            else if (IsBinding(curFunc, tmp)) {
                // tmp gets binding data
                if (tmp->type != INTER_FUNCTION)
                    throw new Exception(NON_FUNCTION, tmp->right);
                else
                    head = Eval_exeInter(head, tmp, level, arguCount, printType); // execute internal func.
                return head;
            } // else if
            else
                throw new Exception(curFunc, UNBOUND_SYMBOL);
        } // else if
        else { // LP
            head = Eval_MultiExp(head, cur, level, printType);
            return head;
        } // else
    } // else

    printType = 0;
    return NULL;
} // Execuate::EvalSExp()

Node Execuate::Eval_MultiExp( Node head, Node cur, int level, int& printType ) {
    // EvalSExp: cur = cur -> left;
    Node result = NULL, tmp = NULL;
    int argu = 0;

    tmp = EvalSExp(cur, level, printType);
    if (tmp->type == LEFT_PAREN)
        throw new Exception(NON_FUNCTION, tmp);

    cur = head->right;
    argu = CheckNumOfArgument(head->right);
    if (tmp->name == "define")
        result = Eval_def(head, cur, 1, argu, printType);
    else if (tmp->name == "cond")
        result = Eval_cond(head, cur, level, argu, printType);
    else if (tmp->name == "if")
        result = Eval_if(head, cur, level, argu, printType);
    else {
        head->left = tmp;
        result = EvalSExp(head, level, printType);
    } // else

    return result;
} // Execuate::Eval_MultiExp()

Node Execuate::Eval_exeInter( Node head, Node tmp, int level, int arguCount, int& printType ) {
    // head -> LP, tmp -> symbol
    // check num of argu
    head->left = tmp->right; // swap
    head = EvalSExp(head, level, printType);
    return head;
} // Execuate::Eval_exeInter()

void Execuate::Eval_exit( int level, int arguCount ) {
    if ( level != 1 )
        throw new Exception( "exit", ERROR_LEVEL );
    else if (arguCount > 0)
        throw new Exception( "exit", ERROR_NUM_OF_ARGUMENTS );

    throw new Exception( QUIT_SCHEME, 0, 0 );
} // Execuate::Eval_exit()

void Execuate::Eval_clean_env( int level, int arguCount ) {
    if ( level != 1 )
        throw new Exception( "clean-environment", ERROR_LEVEL );
    else if ( arguCount > 0 )
        throw new Exception( "clean-environment", ERROR_NUM_OF_ARGUMENTS );

    // delete all binding
    // mParser.DeleteTree( mBindRoot );
    mBindRoot = NULL;
} // Execuate::Eval_clean_env()

Node Execuate::Eval_cons( Node cur, int level, int numOfargu, string name, int printType ) {
    if ( numOfargu != 2 )
        throw new Exception( "cons", ERROR_NUM_OF_ARGUMENTS );

    Node root = NULL, tmp = NULL, clone = NULL;
    Terminal term;
    term.mName = "(";
    term.mType = LEFT_PAREN;

    root = mParser.SetLeaf( term ); // 把 cons 的東西包起來

    term.mName = ".";
    term.mType = DOT;
    root->right = mParser.SetLeaf( term ); // cons 中間的Dot

    // left part
    if ( cur->type == INITIAL ) {
        tmp = cur->left;
        if ( tmp->type == SYMBOL ) {
            if ( IsSystemFunc( tmp->name ) )
                root->left = tmp;
            else if ( IsBinding( tmp->name, tmp ) ) {
                if ( tmp->type == INTER_FUNCTION ) {
                    term.mName = tmp->right->name;
                    term.mType = SYMBOL;
                    root->left = mParser.SetLeaf( term );
                } // if
                else // get binding
                    root->left = GetBind( tmp -> right );
            } // else if
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // if
        else
            root->left = tmp;

        // go to next argu
        cur = cur->right;
    } // if
    else { // LP
        tmp = cur;

        // cur go to next argu
        JumpToNextArgu( cur );
        tmp = EvalSExp( tmp, level, printType );
        root->left = tmp;
        cur = cur->right;  // another argu
    } // else 

    // right part
    if ( cur->type == INITIAL ) {
        tmp = cur->left;
        if ( tmp->type == SYMBOL ) {
            if ( IsSystemFunc( tmp->name ) ) {
                term.mName = tmp->name;
                term.mType = SYMBOL;
                root->right->left = mParser.SetLeaf( term );
            } // if
            else if ( IsBinding( tmp->name, tmp ) ) {
                if ( tmp->type == INTER_FUNCTION ) {
                    term.mName = tmp->right->name;
                    term.mType = SYMBOL;
                    root->right->left = mParser.SetLeaf( term );
                } // if
                else {
                    clone = GetBind( tmp->right );
                    if ( clone->type == LEFT_PAREN ) {
                        tmp = clone;
                        clone->type = INITIAL;
                        clone->name = "";

                        while ( tmp->right->right != NULL ) {
                            tmp = tmp->right;
                        } // while

                        // delete RP & DOT
                        delete tmp->right;
                        tmp->right = NULL;
                        delete root->right;
                        root->right = NULL;

                        // 接上新 argu
                        root->right = clone;
                    } // if
                    else
                        root->right->left = clone;
                } // else
            } // else if
            else
                throw new Exception( tmp->name, UNBOUND_SYMBOL );
        } // if
        else {
            // not symbol
            if ( tmp->type == NILS ) {
                delete root->right;
                root->right = NULL;
            } // if
            else // not NILS
                root->right->left = tmp;
        } // else
    } // if
    else {
        // S_exp
        clone = EvalSExp( cur, level, printType );
        if ( clone->type == LEFT_PAREN ) {
            tmp = clone;
            clone->type = INITIAL;
            clone->name = "";

            while ( tmp->right->right != NULL ) {
                tmp = tmp->right;
            } // while

            // delete RP & DOT
            delete tmp->right;
            tmp->right = NULL;
            delete root->right;
            root->right = NULL;

            // 接上新 argu
            root->right = clone;
        } // if
        else {
            if ( clone->type == NILS ) {
                delete root->right;
                root->right = NULL;
            } // if
            else
                root->right->left = clone;
        } // else
    } // else

    // 補上RP
    tmp = root;
    while ( tmp->right != NULL )
        tmp = tmp->right;

    // insert INITIAL
    term.mName = "";
    term.mType = INITIAL;
    tmp->right = mParser.SetLeaf( term );

    // insert RP
    tmp = tmp->right;
    term.mName = ")";
    term.mType = RIGHT_PAREN;
    tmp->left = mParser.SetLeaf( term );

    return root;
} // Execuate::Eval_cons()

Node Execuate::Eval_def( Node root, Node cur, int level, int arguCount, int printType ) {
    if ( level != 1 )
        throw new Exception( "define", ERROR_LEVEL );
    else if ( arguCount != 2 )
        throw new Exception( DEF_FORMAT, root );

    Node tmp = NULL, walk = NULL;
    string nameOfsymbol = "";
    Terminal term;

    // first argu
    if ( cur->type == INITIAL ) {
        tmp = cur->left;
        if ( IsSystemFunc( tmp->name ) ) // cannot redefine a system primitive
            throw new Exception( DEF_FORMAT, root );
        else if ( IsBinding( tmp->name, walk ) ) {
            // redefine walk
            cur = cur->right; // get second argu
            if ( cur->type == INITIAL ) {
                tmp = cur->left;
                if ( tmp->type == SYMBOL ) {
                    if ( IsSystemFunc( tmp->name ) ) {
                        // define internal func
                        term.mName = tmp->name;
                        term.mType = INTER_FUNCTION;
                        walk->right = mParser.SetLeaf( term );
                    } // if
                    else if ( IsBinding( tmp->name, tmp ) ) {
                        walk->right = tmp->right;
                        if ( tmp->type == INTER_FUNCTION )
                            walk->type = INTER_FUNCTION;
                        else
                            walk->type = SYMBOL;
                    } // else if
                    else // second argu is UNBOUND_SYMBOL
                        throw new Exception( tmp->name, UNBOUND_SYMBOL );
                } // if
                else { // INT FLOAT STRING ...
                    walk->right = tmp;
                    walk->type = SYMBOL;
                } // else
            } // if
            else { // LP
                tmp = EvalSExp( cur, level, printType );
                walk->right = tmp;
                walk->type = SYMBOL;
            } // else

            return walk;
        } // else if
        else {
            if ( tmp->type != SYMBOL )
                throw new Exception( DEF_FORMAT, root );

            // new binding
            nameOfsymbol = tmp->name; // the name of symbol
            cur = cur->right; // get second argu

            if ( cur->type == INITIAL ) {
                tmp = cur->left;  // second argu
                if ( tmp->type == SYMBOL ) {
                    if (IsSystemFunc(tmp->name)) {
                        term.mName = tmp->name;
                        term.mType = SYMBOL;
                        walk = mParser.SetLeaf(term);
                        walk = NewBind(walk, nameOfsymbol, INTER_FUNCTION);
                    } // if
                    else if (IsBinding(tmp->name, tmp)) {
                        walk = tmp->right;
                        if (tmp->type == INTER_FUNCTION)
                            walk = NewBind(walk, nameOfsymbol, INTER_FUNCTION);
                        else
                            walk = NewBind(walk, nameOfsymbol, SYMBOL);
                    } // else if
                    else
                        throw new Exception(tmp->name, UNBOUND_SYMBOL);
                } // if
                else {
                    term.mName = tmp->name;
                    term.mType = tmp->type;
                    walk = mParser.SetLeaf(term);
                    walk = NewBind(walk, nameOfsymbol, SYMBOL);
                } // else
            } // if
            else { // LP
                tmp = EvalSExp(cur, level, printType);
                walk = tmp;
                walk = NewBind(walk, nameOfsymbol, SYMBOL);
            } // else 

            return walk;
        } // else
    } // if 
    else {
        throw new Exception( DEF_FORMAT, root );
    } // else

    return NULL;
} // Execuate::Eval_def()

Node Execuate::Eval_quote( Node head, Node cur, int numOfargu, string name, int printType ) {
    Node clone = NULL;
    int p_count = 0, type = 0;

    head = CloneTree( head, p_count );

    if ( name == "'" ) {
        if ( cur->type == INITIAL ) {
            clone = GetBind( cur->left );
            if ( clone->type == SYMBOL )
                clone->type = ATOM;

            return clone;
        } // if
        else {
            clone = CloneTree( cur, p_count );
            TypeChange( clone );
            return clone;
        } // else 
    } // if
    else if ( name == "quote" ) {
        if ( cur->type == LEFT_PAREN ) {
            if ( numOfargu != 1 )
                throw new Exception( name, head, ERROR_NUM_OF_ARGUMENTS );

            clone = CloneTree( cur, p_count );
            TypeChange( clone );
            return clone;
        } // if
        else {
            // only one argu
            if ( numOfargu != 1 )
                throw new Exception( name, head, ERROR_NUM_OF_ARGUMENTS );
            else {
                clone = GetBind( cur->left );
                if ( clone->type == SYMBOL )
                    clone->type = ATOM;

                return clone;
            } // else
        } // else 
    } // else if

    return NULL;
} // Execuate::Eval_quote()

Node Execuate::Eval_car( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount != 1 )
        throw new Exception( "car", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL, clone = NULL;

    if (cur->type == INITIAL) {
        tmp = cur->left;
        if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                throw new Exception("car", tmp); // error type
            else if (IsBinding(tmp->name, tmp)) {
                tmp = tmp->right;
                if (tmp->type != LEFT_PAREN)
                    throw new Exception("car", tmp);
                else {
                    clone = GetBind(tmp);
                    result = tmp->left;
                } // else
            } // else if
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // if
        else
            throw new Exception("car", tmp); // error type
    } // if
    else if (cur->type == LEFT_PAREN) {
        tmp = EvalSExp(cur, level, printType);
        if (tmp->type == LEFT_PAREN)
            result = tmp->left;
        else
            // ATOM
            throw new Exception("car", tmp);
    } // else if

    return result;
} // Execuate::Eval_car()

Node Execuate::Eval_cdr( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount != 1 )
        throw new Exception( "cdr", ERROR_NUM_OF_ARGUMENTS );

    int p_count = 0;
    cur = CloneTree( cur, p_count );
    cur = cur->right;

    Node result = NULL, tmp = NULL, clone = NULL;
    Terminal term;

    if ( cur->type == INITIAL ) {
        tmp = cur->left;
        if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                throw new Exception("cdr", tmp); // error type
            else if (IsBinding(tmp->name, tmp)) {
                tmp = tmp->right;
                if (tmp->type != LEFT_PAREN)
                    throw new Exception("cdr", tmp);
                else {
                    clone = GetBind(tmp);
                    tmp = clone->right;

                    if (tmp->type == DOT)
                        result = tmp->left;
                    else {
                        // set LP
                        term.mName = "(";
                        term.mType = LEFT_PAREN;
                        result = mParser.SetLeaf(term);

                        if (tmp->type == INITIAL) {
                            result->left = tmp->left;
                            tmp = tmp->right;
                            cur = tmp;
                        } // if
                        else {
                            result->left = tmp;
                            JumpToNextArgu(tmp);
                            cur = tmp->right;
                            tmp->right = NULL;
                        } // else

                        result->right = cur;
                    } // else
                } // else
            } // else if
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // if
        else
            throw new Exception("cdr", tmp); // error type
    } // if
    else if ( cur->type == LEFT_PAREN ) {
        tmp = EvalSExp(cur, level, printType);
        if (tmp->type == LEFT_PAREN) {
            tmp = tmp->right;
            if (tmp->type == DOT) {
                result = tmp->left;
                return result;
            } // if
            else {
                // set LP
                term.mName = "(";
                term.mType = LEFT_PAREN;
                result = mParser.SetLeaf(term);

                if (tmp->type == INITIAL) {
                    result->left = tmp->left;
                    tmp = tmp->right;
                    cur = tmp;
                } // if
                else {
                    result->left = tmp;
                    JumpToNextArgu(tmp);
                    cur = tmp->right;
                    tmp->right = NULL;
                } // else

                result->right = cur;
            } // else
        } // if
        else // ATOM
            throw new Exception("cdr", tmp);
    } // else if

    if ( result->left->type == RIGHT_PAREN ) {
        term.mType = NILS;
        delete result;
        result = mParser.SetLeaf(term);
    } // if

    return result;
} // Execuate::Eval_cdr()

Node Execuate::Eval_begin( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount == 0 )
        throw new Exception( "begin", ERROR_NUM_OF_ARGUMENTS );

    Node tmp = NULL;

    while ( cur != NULL ) {
        if (cur->type == INITIAL) {
            if (cur->left->type == SYMBOL) {
                tmp = cur->left;
                if (IsSystemFunc(tmp->name))
                    ;
                else if (IsBinding(tmp->name, tmp))
                    tmp = GetBind(tmp->right); // go to bind
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else if (cur->left->type == RIGHT_PAREN)
                cur = NULL; // 強制結束
            else
                tmp = cur->left; // ATOM but not symbol
        } // if
        else { // LP
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu(cur);

            tmp = EvalSExp(tmp, level, printType);
        } // else 

        if (cur != NULL)
            cur = cur->right; // go to another argu.
    } // while

    return tmp;
} // Execuate::Eval_begin()

Node Execuate::Eval_list( Node cur, int level, int arguCount, int printType ) {
    Node root = NULL, tmp = NULL, walk = NULL, walk_pre = NULL, clone = NULL;
    Terminal initial, term;
    int p_count = 0;

    // 強制結束
    if ( arguCount == 0 ) {
        term.mType = NILS;
        root = mParser.SetLeaf( term );
        return root;
    } // if

    initial.mType = INITIAL;

    // set LP
    term.mName = "(";
    term.mType = LEFT_PAREN;
    root = mParser.SetLeaf(term);

    // get first argu
    if ( cur->type == INITIAL ) {
        tmp = cur->left;
        if ( tmp->type == SYMBOL ) {
            if ( IsSystemFunc( tmp->name ) )
                root->left = tmp;
            else if ( IsBinding( tmp->name, tmp ) ) {
                clone = GetBind( tmp->right );
                root->left = clone;
            } // else if
            else
                throw new Exception( tmp->name, UNBOUND_SYMBOL );
        } // if
        else // ATOM but not symbol
            root->left = tmp;
    } // if
    else {
        tmp = cur; // tmp -> type == LP

        // cur go to next argu
        JumpToNextArgu( cur );

        tmp = EvalSExp(tmp, level, printType);
        root->left = tmp;
    } // else

    cur = cur->right;
    root->right = mParser.SetLeaf( initial );
    walk_pre = root;      // parent
    walk = root->right; // child

    while ( cur != NULL ) {
        if (cur->type == INITIAL) {
            if (cur->left->type == SYMBOL) {
                tmp = cur->left;
                if (IsSystemFunc(tmp->name))
                    walk->left = tmp;
                else if (IsBinding(tmp->name, tmp)) {
                    clone = GetBind(tmp->right);
                    if (clone->type != LEFT_PAREN)
                        walk->left = clone;
                    else {
                        // LP
                        // Now, walk -> initial
                        walk_pre->right = clone;
                        walk = walk_pre->right;
                        JumpToNextArgu(walk);
                    } // else
                } // else if 
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else if (cur->left->type == RIGHT_PAREN)
                cur = NULL;
            else
                walk->left = cur->left; // ATOM but not symbol
        } // if
        else { // LP
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu(cur);

            tmp = EvalSExp(tmp, level, printType);
            if (tmp->type == LEFT_PAREN) {
                walk_pre->right = tmp;
                walk = walk_pre->right;
                JumpToNextArgu(walk);
            } // if
            else
                walk->left = tmp;
        } // else

        if (cur != NULL) {
            walk->right = mParser.SetLeaf(initial);
            walk_pre = walk;
            walk = walk->right;
            cur = cur->right; // go to another argu.
        } // if         
    } // while

    // set RP
    term.mName = ")";
    term.mType = RIGHT_PAREN;
    walk->left = mParser.SetLeaf( term );

    return root;
} // Execuate::Eval_list()

Node Execuate::Eval_if( Node head, Node cur, int level, int arguCount, int printType ) {
    int p_count = 0;
    head = CloneTree( head, p_count );
    if ( arguCount != 2 && arguCount != 3 )
        throw new Exception( "if", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;

    // get boolean
    if (cur->type == INITIAL) {
        tmp = cur->left;
        if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                ;
            else if (IsBinding(tmp->name, tmp))
                tmp = tmp->right;
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // if
        else
            ; // ATOM but not symbol
    } // if
    else { // LP
        tmp = cur;
        JumpToNextArgu(cur);
        tmp = EvalSExp(tmp, level, printType);
    } // else

    cur = cur->right; // go to next argu
    if (tmp->type != NILS)
        ; // get second argu
    else {
        // go to get third argu
        // cur is at second argu
        if (cur->type == LEFT_PAREN)
            JumpToNextArgu(cur);

        cur = cur->right; // go to next argu
    } // else 

    if (cur->type == INITIAL) {
        tmp = cur->left;
        if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                result = tmp;
            else if (IsBinding(tmp->name, tmp))
                result = tmp->right;
            else
                throw new Exception( tmp->name, UNBOUND_SYMBOL );
        } // if
        else if ( tmp->type == RIGHT_PAREN )
            throw new Exception( NO_RETURN_RESULT, head );
        else
            result = tmp;
    } // if
    else // LP
        result = EvalSExp( cur, level, printType );

    mParser.DeleteTree( head );
    return result;
} // Execuate::Eval_if()

Node Execuate::Eval_cond( Node head, Node cur, int level, int arguCount, int printType ) {
    int p_count = 0;
    head = CloneTree( head, p_count );
    if ( arguCount < 1 )
        throw new Exception( COND_FORMAT, head );

    Cond_FormatCheck(head, cur);

    Node result = NULL, tmp = NULL;
    bool stopCond = false;

    while (cur != NULL) {
        if (cur->type == LEFT_PAREN) {
            tmp = cur;
            JumpToNextArgu(cur); // cur go to next argu
            result = Condition(head, tmp, level, printType, stopCond, result, arguCount);
        } // if
        else if (cur->type == INITIAL) {
            if (cur->left->type == RIGHT_PAREN)
                cur = NULL;
            else
                throw new Exception(COND_FORMAT, head);
        } // else if

        if (cur != NULL)
            cur = cur->right;
    } // while

    if (result == NULL) // cur == NULL
        throw new Exception(NO_RETURN_RESULT, head);

    mParser.DeleteTree(head);
    return result; // tmp
} // Execuate::Eval_cond()

void Execuate::Cond_FormatCheck( Node head, Node cur ) {
    Node tmp = NULL;
    int argu = 0;

    while (cur != NULL) {
        if (cur->type == LEFT_PAREN) {
            tmp = cur;
            JumpToNextArgu(cur); // cur go to next argu
            if (IsThereDot(tmp->right))
                throw new Exception(COND_FORMAT, head);

            argu = CheckNumOfArgument(tmp->right);
            if (argu == 0)
                throw new Exception(COND_FORMAT, head);
        } // if
        else if (cur->type == INITIAL) {
            if (cur->left->type == RIGHT_PAREN)
                cur = NULL;
            else
                throw new Exception(COND_FORMAT, head);
        } // else if

        if (cur != NULL)
            cur = cur->right;
    } // while
} // Execuate::Cond_FormatCheck()

Node Execuate::Condition( Node head, Node cur, int level, int printType, bool& stop, Node result, int& nunOfCond ) {
    // 執行由Eval_cond丟過來的條件
    if (stop)
        return result;

    string name = "";
    Node tmp = NULL;
    tmp = cur->left;
    cur = cur->right;

    nunOfCond--;
    if (tmp->type == LEFT_PAREN)
        tmp = EvalSExp(tmp, level, printType);
    else if (tmp->type == SYMBOL) {
        if (IsSystemFunc(tmp->name))
            ;
        else if (IsBinding(tmp->name, tmp)) {
            name = tmp->name;
            tmp = tmp->right;
        } // else if
        else {
            if (nunOfCond == 0) {
                if (tmp->name == "else")
                    result = Eval_begin(cur, level, 5, printType);
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // else
    } // else if

    if (tmp->type == NILS) {
        if (name == "else")
            result = Eval_begin(cur, level, 5, printType);
        else
            return NULL;
    } // if
    else {
        // Not nil
        result = Eval_begin(cur, level, 5, printType);
        stop = true;
    } // else

    return result;
} // Execuate::Condition()

Node Execuate::Eval_IsInteger( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount != 1 )
        throw new Exception( "integer?", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    Terminal term;

    if (cur->type == INITIAL) {
        tmp = cur->left;

        if (tmp->type == INT)
            term.mType = TRUE;
        else if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                term.mType = NILS;
            else if (IsBinding(tmp->name, tmp)) {
                if (tmp->right->type == INT)
                    term.mType = TRUE;
                else
                    term.mType = NILS;
            } // else if
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // else if
        else
            term.mType = NILS;
    } // if
    else {
        tmp = EvalSExp(cur, level, printType);

        if (tmp->type == INT)
            term.mType = TRUE;
        else
            term.mType = NILS;
    } // else

    result = mParser.SetLeaf(term);
    return result;
} // Execuate::Eval_IsInteger()

Node Execuate::Eval_IsNumber( Node cur, int level, int arguCount, int printType, string name ) {
    if ( arguCount != 1 )
        throw new Exception( name, ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    Terminal term;

    if (cur->type == INITIAL) {
        tmp = cur->left;

        if (tmp->type == INT || tmp->type == FLOAT)
            term.mType = TRUE;
        else if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                term.mType = NILS;
            else if (IsBinding(tmp->name, tmp)) {
                if (tmp->right->type == INT || tmp->right->type == FLOAT)
                    term.mType = TRUE;
                else
                    term.mType = NILS;
            } // else if
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // else if
        else
            term.mType = NILS;
    } // if
    else {
        tmp = EvalSExp(cur, level, printType);

        if (tmp->type == INT || tmp->type == FLOAT)
            term.mType = TRUE;
        else
            term.mType = NILS;
    } // else

    result = mParser.SetLeaf(term);
    return result;
} // Execuate::Eval_Isnumber()

Node Execuate::Eval_IsString( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount != 1 )
        throw new Exception( "string?", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    Terminal term;

    if (cur->type == INITIAL) {
        tmp = cur->left;

        if (tmp->type == STRING)
            term.mType = TRUE;
        else if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                term.mType = NILS;
            else if (IsBinding(tmp->name, tmp)) {
                if (tmp->right->type == STRING)
                    term.mType = TRUE;
                else
                    term.mType = NILS;
            } // else if
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // else if
        else
            term.mType = NILS;
    } // if
    else {

        tmp = EvalSExp(cur, level, printType);

        if (tmp->type == STRING)
            term.mType = TRUE;
        else
            term.mType = NILS;
    } // else

    result = mParser.SetLeaf(term);
    return result;
} // Execuate::Eval_IsString()

Node Execuate::Eval_IsSymbol( Node cur, int level, int arguCount, int printType ) {
    if (arguCount != 1)
        throw new Exception( "symbol?", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    Terminal term;

    if (cur->type == INITIAL) {
        tmp = cur->left;

        if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                term.mType = TRUE;
            else if (IsBinding(tmp->name, tmp)) {
                if (tmp->right->type == SYMBOL || tmp->right->type == ATOM)
                    term.mType = TRUE;
                else
                    term.mType = NILS;
            } // else if
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // if
        else
            term.mType = NILS;
    } // if
    else { // LP
        tmp = EvalSExp(cur, level, printType);

        if (tmp->type == SYMBOL || tmp->type == ATOM)
            term.mType = TRUE;
        else
            term.mType = NILS;
    } // else

    result = mParser.SetLeaf(term);
    return result;
} // Execuate::Eval_IsSymbol()

Node Execuate::Eval_IsAtom( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount != 1 )
        throw new Exception( "atom?", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    Terminal term;

    if ( cur->type == INITIAL ) {
        tmp = cur->left;

        if ( tmp->type == STRING || tmp->type == INT || tmp->type == FLOAT ||
            tmp->type == NILS || tmp->type == TRUE )
            term.mType = TRUE;
        else if ( tmp->type == SYMBOL ) {
            if ( IsSystemFunc( tmp->name ) )
                term.mType = TRUE;
            else if ( IsBinding( tmp->name, tmp ) ) {
                tmp = tmp->right;
                if ( tmp->type == STRING || tmp->type == INT || tmp->type == FLOAT ||
                    tmp->type == NILS || tmp->type == TRUE || tmp->type == SYMBOL )
                    term.mType = TRUE;
                else
                    term.mType = NILS;
            } // else if
            else
                throw new Exception( tmp->name, UNBOUND_SYMBOL );
        } // else if
        else
            term.mType = NILS;
    } // if
    else { // LP
        tmp = EvalSExp( cur, level, printType );

        if ( tmp->type == STRING || tmp->type == INT || tmp->type == FLOAT ||
            tmp->type == NILS || tmp->type == TRUE || tmp->type == SYMBOL )
            term.mType = TRUE;
        else
            term.mType = NILS;
    } // else

    result = mParser.SetLeaf( term );
    return result;
} // Execuate::Eval_IsAtom()

Node Execuate::Eval_IsNull( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount != 1 )
        throw new Exception( "null?", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    Terminal term;

    if ( cur->type == INITIAL ) {
        tmp = cur->left;

        if (tmp->type == NILS)
            term.mType = TRUE;
        else if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                term.mType = NILS;
            else if (IsBinding(tmp->name, tmp)) {
                if (tmp->right->type == NILS)
                    term.mType = TRUE;
                else
                    term.mType = NILS;
            } // else if
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // else if
        else
            term.mType = NILS;
    } // if
    else {
        tmp = EvalSExp(cur, level, printType);

        if (tmp->type == NILS)
            term.mType = TRUE;
        else
            term.mType = NILS;
    } // else

    result = mParser.SetLeaf( term );
    return result;
} // Execuate::Eval_IsNull()

Node Execuate::Eval_IsBoolean( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount != 1 )
        throw new Exception( "boolean?", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    Terminal term;

    if (cur->type == INITIAL) {
        tmp = cur->left;

        if (tmp->type == TRUE || tmp->type == NILS)
            term.mType = TRUE;
        else if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                term.mType = NILS;
            else if (IsBinding(tmp->name, tmp)) {
                tmp = tmp->right;
                if (tmp->type == TRUE || tmp->type == NILS)
                    term.mType = TRUE;
                else
                    term.mType = NILS;
            } // else if
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // else if
        else
            term.mType = NILS;
    } // if
    else {
        tmp = EvalSExp(cur, level, printType);

        if (tmp->type == TRUE || tmp->type == NILS)
            term.mType = TRUE;
        else
            term.mType = NILS;
    } // else

    result = mParser.SetLeaf( term );
    return result;
} // Execuate::Eval_IsBoolean()

Node Execuate::Eval_IsPair( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount != 1 )
        throw new Exception( "pair?", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    Terminal term;

    if (cur->type == INITIAL) {
        tmp = cur->left;
        if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                term.mType = NILS;
            else if (IsBinding(tmp->name, tmp)) {
                tmp = tmp->right;
                if (tmp->type == LEFT_PAREN)
                    term.mType = TRUE;
                else
                    term.mType = NILS;
            } // else if
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // if
        else // not symbol
            term.mType = NILS;
    } // if
    else { // LP
        tmp = EvalSExp(cur, level, printType);
        if (tmp->type == LEFT_PAREN) // pair
            term.mType = TRUE;
        else // not LP
            term.mType = NILS;
    } // else 

    result = mParser.SetLeaf(term);
    return result;
} // Execuate::Eval_IsPair()

Node Execuate::Eval_IsList( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount != 1 )
        throw new Exception( "list?", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    Terminal term;

    if (cur->type == INITIAL) {
        tmp = cur->left;
        if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                term.mType = NILS;
            else if (IsBinding(tmp->name, tmp)) {
                if (tmp->right->type == LEFT_PAREN) {
                    tmp = tmp->right; // the root of binding data
                    if (IsThereDot(tmp->right))
                        term.mType = NILS; // DOT in the pair
                    else // No DOT
                        term.mType = TRUE;
                } // if
                else
                    term.mType = NILS;
            } // else if
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // if
        else // not symbol
            term.mType = NILS;
    } // if
    else { // LP
        tmp = EvalSExp(cur, level, printType);
        if (tmp->type == LEFT_PAREN) {
            if (IsThereDot(tmp->right))
                term.mType = NILS; // DOT in the pair
            else // No DOT
                term.mType = TRUE;
        } // if
        else // not LP
            term.mType = NILS;
    } // else 

    result = mParser.SetLeaf(term);
    return result;
} // Execuate::Eval_IsList()

Node Execuate::Eval_add( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( "+", ERROR_NUM_OF_ARGUMENTS );

    Node tmp = NULL, result = NULL;
    stringstream ss;
    bool isFloat = false;   // 只要有一個 float 全部都是 float 
    float sum = 0, num = 0; // 都先用 float 存

    while ( cur != NULL ) {
        if ( cur->type == INITIAL ) {
            tmp = cur->left;
            if ( tmp->type == SYMBOL ) {
                if (IsSystemFunc(tmp->name))
                    throw new Exception("+", tmp); // error type
                else if (IsBinding(tmp->name, tmp)) {
                    tmp = tmp->right; // go to bind
                    if (tmp->type == LEFT_PAREN)
                        throw new Exception("+", tmp); // error type
                    else {
                        if (tmp->type == INT || tmp->type == FLOAT) {
                            if (tmp->type == FLOAT) // type of print
                                isFloat = true;

                            // string to float
                            num = StringToNum(tmp->name);
                        } // if
                        else
                            throw new Exception("+", tmp); // error type
                    } // else
                } // else if
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else if ( tmp->type == INT || tmp->type == FLOAT ) {
                if ( tmp->type == FLOAT ) // type of print
                    isFloat = true;

                // string to float
                num = StringToNum( tmp->name );
            } // else if
            else if ( tmp->type == RIGHT_PAREN ) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception( "+", tmp ); // error type
        } // if
        else {
            tmp = cur; // tmp -> type == LP
            // cur go to next argu
            JumpToNextArgu(cur);

            tmp = EvalSExp(tmp, level, printType);

            if (tmp->type == INT || tmp->type == FLOAT) {
                if (tmp->type == FLOAT) // type of print
                    isFloat = true;

                // string to float or int
                num = StringToNum(tmp->name);
            } // if
            else
                throw new Exception("+", tmp); // error type
        } // else 

        sum = sum + num;
        num = 0;
        tmp = NULL;

        if ( cur != NULL )
            cur = cur->right; // go to another argu.
    } // while

    Terminal term;
    int int_num = 0;

    // assign type
    if ( !isFloat ) {
        int_num = (int)sum; // type change
        ss << int_num;
        term.mType = INT;
    } // if
    else {
        ss << sum;
        term.mType = FLOAT;
    } // else

    // assign value
    term.mName = ss.str();
    result = mParser.SetLeaf( term );

    return result;
} // Execuate::Eval_add()

Node Execuate::Eval_sub( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( "-", ERROR_NUM_OF_ARGUMENTS );

    Node tmp = NULL, result = NULL;
    stringstream ss;        // string to num
    bool isFloat = false;   // 只要有一個 float 全部都是 float
    bool isFirst = true;    // 第一個 argu 先 assign
    float sum = 0, num = 0; // 都先用 float 存

    while ( cur != NULL ) {
        if ( cur->type == INITIAL ) {
            tmp = cur->left;
            if ( tmp->type == SYMBOL ) {
                if (IsSystemFunc(tmp->name))
                    throw new Exception("-", tmp); // error type
                else if (IsBinding(tmp->name, tmp)) {
                    tmp = tmp->right; // go to bind
                    if (tmp->type == LEFT_PAREN)
                        throw new Exception("-", tmp); // error type
                    else {
                        if (tmp->type == INT || tmp->type == FLOAT) {
                            if (tmp->type == FLOAT) // type of print
                                isFloat = true;

                            // string to float
                            num = StringToNum(tmp->name);
                        } // if
                        else
                            throw new Exception("-", tmp); // error type
                    } // else
                } // else if
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else if ( tmp->type == INT || tmp->type == FLOAT ) {
                if (tmp->type == FLOAT) // type of print
                    isFloat = true;

                // string to float
                num = StringToNum( tmp->name );
            } // else if
            else if ( tmp->type == RIGHT_PAREN ) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception( "-", tmp ); // error type
        } // if
        else {
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu(cur);

            tmp = EvalSExp(tmp, level, printType);
            if (tmp->type == INT || tmp->type == FLOAT) {
                if (tmp->type == FLOAT) // type of print
                    isFloat = true;

                // string to float
                num = StringToNum(tmp->name);
            } // if
            else
                throw new Exception("-", tmp); // error type
        } // else 

        if ( isFirst ) {
            sum = num;
            isFirst = false;
        } // if
        else
            sum = sum - num;

        num = 0;
        tmp = NULL;

        if ( cur != NULL )
            cur = cur->right; // go to another argu.
    } // while

    Terminal term;
    int int_num = 0;

    // assign type
    if ( !isFloat ) {
        int_num = (int)sum; // type change
        ss << int_num;
        term.mType = INT;
    } // if
    else {
        ss << sum;
        term.mType = FLOAT;
    } // else

    // assign value
    term.mName = ss.str();
    result = mParser.SetLeaf(term);

    return result;
} // Execuate::Eval_sub()

Node Execuate::Eval_mul( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( "*", ERROR_NUM_OF_ARGUMENTS );

    Node tmp = NULL, result = NULL;
    stringstream ss;        // string to num
    bool isFloat = false;   // 只要有一個 float 全部都是 float
    bool isFirst = true;    // 第一個 argu 先 assign
    float sum = 0, num = 0; // 都先用 float 存

    while ( cur != NULL ) {
        if ( cur->type == INITIAL ) {
            tmp = cur->left;
            if ( tmp->type == SYMBOL ) {
                if ( IsSystemFunc( tmp->name ) )
                    throw new Exception( "*", tmp ); // error type
                else if ( IsBinding( tmp->name, tmp ) ) {
                    tmp = tmp->right; // go to bind
                    if ( tmp->type == LEFT_PAREN )
                        throw new Exception( "*", tmp ); // error type
                    else {
                        if ( tmp->type == INT || tmp->type == FLOAT ) {
                            if ( tmp->type == FLOAT ) // type of print
                                isFloat = true;

                            // string to float
                            num = StringToNum( tmp->name );
                        } // if
                        else
                            throw new Exception( "*", tmp ); // error type
                    } // else
                } // else if
                else
                    throw new Exception( tmp->name, UNBOUND_SYMBOL );
            } // if
            else if ( tmp->type == INT || tmp->type == FLOAT ) {
                if ( tmp->type == FLOAT ) // type of print
                    isFloat = true;

                // string to float
                num = StringToNum(tmp->name);
            } // else if
            else if ( tmp->type == RIGHT_PAREN ) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception( "*", tmp ); // error type
        } // if
        else {
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu( cur );

            tmp = EvalSExp( tmp, level, printType );
            if ( tmp->type == INT || tmp->type == FLOAT ) {
                if ( tmp->type == FLOAT ) // type of print
                    isFloat = true;

                // string to float
                num = StringToNum( tmp->name );
            } // if
            else
                throw new Exception( "*", tmp ); // error type
        } // else 

        if ( cur != NULL ) {
            if (isFirst) {
                sum = num;
                isFirst = false;
            } // if
            else
                sum = sum * num;

            num = 0;
            cur = cur->right; // go to another argu.
        } // if

        tmp = NULL;
    } // while

    Terminal term;
    int int_num = 0;

    // assign type
    if ( !isFloat ) {
        int_num = (int)sum; // type change
        ss << int_num;
        term.mType = INT;
    } // if
    else {
        ss << sum;
        term.mType = FLOAT;
    } // else

    // assign value
    term.mName = ss.str();
    result = mParser.SetLeaf(term);

    return result;
} // Execuate::Eval_mul()

Node Execuate::Eval_div( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( "/", ERROR_NUM_OF_ARGUMENTS );

    Node tmp = NULL, result = NULL;
    stringstream ss;        // string to num
    bool isFloat = false;   // 只要有一個 float 全部都是 float
    bool isFirst = true;    // 第一個 argu 先 assign
    float sum = 0, num = 0; // 都先用 float 存

    while ( cur != NULL ) {
        if (cur->type == INITIAL) {
            tmp = cur->left;
            if (tmp->type == SYMBOL) {
                if (IsSystemFunc(tmp->name))
                    throw new Exception("/", tmp); // error type
                else if (IsBinding(tmp->name, tmp)) {
                    tmp = tmp->right; // go to bind
                    if (tmp->type == LEFT_PAREN)
                        throw new Exception("/", tmp); // error type
                    else {
                        if (tmp->type == INT || tmp->type == FLOAT) {
                            if (tmp->type == FLOAT) // type of print
                                isFloat = true;

                            // string to float
                            num = StringToNum(tmp->name);
                        } // if
                        else
                            throw new Exception("/", tmp); // error type
                    } // else
                } // else if
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else if (tmp->type == INT || tmp->type == FLOAT) {
                if (tmp->type == FLOAT) // type of print
                    isFloat = true;

                // string to float
                num = StringToNum(tmp->name);
            } // else if
            else if ( tmp->type == RIGHT_PAREN ) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception("/", tmp); // error type
        } // if
        else {
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu(cur);

            tmp = EvalSExp(tmp, level, printType);

            if (tmp->type == INT || tmp->type == FLOAT) {
                if (tmp->type == FLOAT) // type of print
                    isFloat = true;

                // string to float
                num = StringToNum(tmp->name);
            } // if
            else
                throw new Exception("/", tmp); // error type
        } // else 

        if (cur != NULL) {
            if (isFirst) {
                sum = num;
                isFirst = false;
            } // if
            else {
                if (num != 0)
                    sum = sum / num;
                else
                    throw new Exception("/", JUST_ERROR);
            } // else

            num = 0;
            cur = cur->right; // go to another argu.
        } // if

        tmp = NULL;
    } // while

    Terminal term;
    int int_num = 0;

    // assign type
    if ( !isFloat ) {
        int_num = (int)sum; // type change
        ss << int_num;
        term.mType = INT;
    } // if
    else {
        ss << sum;
        term.mType = FLOAT;
    } // else

    // assign value
    term.mName = ss.str();
    result = mParser.SetLeaf(term);

    return result;
} // Execuate::Eval_div()

Node Execuate::Eval_not( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount != 1 )
        throw new Exception( "not", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL;
    Terminal term;

    if (cur->type == INITIAL) {
        result = cur->left;
        if (result->type == SYMBOL) {
            if (IsSystemFunc(result->name))
                throw new Exception("not", result); // error type
            else if (IsBinding(result->name, result)) {
                result = result->right;
                if (result->type == NILS || result->type == TRUE) {
                    if (result->type == NILS)
                        term.mType = TRUE;
                    else
                        term.mType = NILS;
                } // if
                else
                    term.mType = NILS;
            } // else if
            else
                throw new Exception(result->name, UNBOUND_SYMBOL);
        } // if
        else if (result->type == NILS || result->type == TRUE) {
            if (result->type == NILS)
                term.mType = TRUE;
            else
                term.mType = NILS;
        } // else if
        else
            term.mType = NILS;
    } // if
    else { // LP
        result = EvalSExp(cur, level, printType);
        if (result->type == NILS || result->type == TRUE) {
            if (result->type == NILS)
                term.mType = TRUE;
            else
                term.mType = NILS;
        } // if
        else
            term.mType = NILS;
    } // else 

    result = mParser.SetLeaf(term);

    return result;
} // Execuate::Eval_not()

Node Execuate::Eval_and( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( "and", ERROR_NUM_OF_ARGUMENTS );

    Node tmp = NULL, result = NULL, clone = NULL;

    while (cur != NULL) {
        if (cur->type == INITIAL) {
            tmp = cur->left;
            if (tmp->type == SYMBOL) {
                if (IsSystemFunc(tmp->name))
                    result = tmp; // get procedure
                else if (IsBinding(tmp->name, tmp)) {
                    tmp = tmp->right; // go to bind
                    clone = GetBind(tmp);
                    if (tmp->type != NILS)
                        result = clone;
                    else if (tmp->type == NILS) {
                        result = clone;
                        return result;
                    } // else if
                } // else if
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else if (tmp->type == RIGHT_PAREN) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else if (tmp->type == NILS) {
                result = tmp;
                return result;
            } // else if
            else // ATOM or S-exp but not NIL
                result = tmp;
        } // if
        else { // LP
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu(cur);

            tmp = EvalSExp(tmp, level, printType);

            if (tmp->type == NILS) {
                result = tmp;
                return result;
            } // if
            else
                result = tmp;
        } // else 

        if (cur != NULL)
            cur = cur->right; // go to another argu.
        tmp = NULL;
    } // while

    return result;
} // Execuate::Eval_and()

Node Execuate::Eval_or( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( "or", ERROR_NUM_OF_ARGUMENTS );

    Node tmp = NULL, result = NULL, clone = NULL;

    while ( cur != NULL ) {
        if (cur->type == INITIAL) {
            tmp = cur->left;
            if (tmp->type == SYMBOL) {
                if (IsSystemFunc(tmp->name)) {
                    result = tmp; // get procedure
                    return result;
                } // if
                else if (IsBinding(tmp->name, tmp)) {
                    tmp = tmp->right; // go to bind
                    clone = GetBind(tmp);
                    if (tmp->type != NILS) {
                        result = clone;
                        return result;
                    } // if
                    else if (tmp->type == NILS)
                        ; // go to get next argu
                } // else if
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else if (tmp->type == RIGHT_PAREN) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else if (tmp->type == NILS)
                ; // go to get next argu
            else { // ATOM or S-exp but not NIL
                result = tmp;
                return result;
            } // else
        } // if
        else { // LP
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu(cur);

            tmp = EvalSExp(tmp, level, printType);
            if (tmp->type == NILS)
                ;
            else { // ATOM or S-exp but not NIL
                result = tmp;
                return result;
            } // else
        } // else 

        if (cur != NULL)
            cur = cur->right; // go to another argu.
        tmp = NULL;
    } // while

    return result;
} // Execuate::Eval_or()

Node Execuate::Eval_append( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( "string-append", ERROR_NUM_OF_ARGUMENTS );

    Node tmp = NULL, result = NULL;
    string str = "", t_str = "";

    while ( cur != NULL ) {
        if (cur->type == INITIAL) {
            tmp = cur->left;
            if (tmp->type == SYMBOL) {
                if (IsSystemFunc(tmp->name))
                    throw new Exception("string-append", tmp); // error type
                else if (IsBinding(tmp->name, tmp)) {
                    tmp = tmp->right; // go to bind
                    if (tmp->type == LEFT_PAREN)
                        throw new Exception("string-append", tmp); // error type
                    else {
                        if (tmp->type == STRING)
                            t_str = GetStr(tmp->name);
                        else
                            throw new Exception("string-append", tmp); // error type
                    } // else
                } // else if
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else if (tmp->type == STRING)
                t_str = GetStr(tmp->name);
            else if (tmp->type == RIGHT_PAREN) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception("string-append", tmp); // error type
        } // if
        else {
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu(cur);

            tmp = EvalSExp(tmp, level, printType);

            if (tmp->type == STRING)
                t_str = GetStr(tmp->name);
            else
                throw new Exception("string-append", tmp); // error type
        } // else 

        str = str + t_str;
        tmp = NULL;
        t_str = ""; // initial

        if (cur != NULL)
            cur = cur->right; // go to another argu.
    } // while

    Terminal term;
    term.mName = "\"" + str + "\"";
    term.mType = STRING;

    // assign value
    result = mParser.SetLeaf(term);

    return result;
} // Execuate::Eval_append()

Node Execuate::Eval_great_than( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( ">", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    float num1 = 0, num2 = 0;
    bool isFirst = true, isGreatThan = true;

    while ( cur != NULL ) {
        if ( cur->type == INITIAL ) {
            tmp = cur->left;
            if ( tmp->type == SYMBOL ) {
                if ( IsSystemFunc( tmp->name ) )
                    throw new Exception( ">", tmp ); // error type
                else if ( IsBinding( tmp->name, tmp ) ) {
                    tmp = tmp->right; // go to bind
                    if ( tmp->type == LEFT_PAREN )
                        throw new Exception( ">", tmp ); // error type
                    else {
                        if ( tmp->type == INT || tmp->type == FLOAT )
                            // string to float
                            num2 = StringToNum( tmp->name );
                        else
                            throw new Exception( ">", tmp ); // error type
                    } // else
                } // else if
                else
                    throw new Exception( tmp->name, UNBOUND_SYMBOL );
            } // if
            else if ( tmp->type == INT || tmp->type == FLOAT )
                // string to float
                num2 = StringToNum( tmp->name );
            else if ( tmp->type == RIGHT_PAREN ) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception( ">", tmp ); // error type
        } // if
        else { // LP
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu( cur );

            tmp = EvalSExp( tmp, level, printType );
            if ( tmp->type == INT || tmp->type == FLOAT )
                // string to float
                num2 = StringToNum( tmp->name );
            else
                throw new Exception( ">", tmp ); // error type
        } // else

        if ( cur != NULL ) {
            if ( isFirst ) { // only one argu : num1
                num1 = num2;
                isFirst = false;
            } // if
            else {           // two argus : num1 and num2
                if ( num1 > num2 )
                    ;        // do nothing
                else       // num1 <= num2
                    isGreatThan = false;
            } // else

            num1 = num2; // swap
            cur = cur->right;
        } // if

        tmp = NULL;
    } // while

    Terminal term;
    if (isGreatThan)
        term.mType = TRUE;
    else // not greater than
        term.mType = NILS;

    result = mParser.SetLeaf(term);

    return result;
} // Execuate::Eval_great_than()

Node Execuate::Eval_less_than( Node cur, int level, int arguCount, int printType ) {
    if (arguCount < 2)
        throw new Exception("<", ERROR_NUM_OF_ARGUMENTS);

    Node result = NULL, tmp = NULL;
    float num1 = 0, num2 = 0;
    bool isFirst = true, isLessThan = true;

    while (cur != NULL) {
        if (cur->type == INITIAL) {
            tmp = cur->left;
            if (tmp->type == SYMBOL) {
                if (IsSystemFunc(tmp->name))
                    throw new Exception("<", tmp); // error type
                else if (IsBinding(tmp->name, tmp)) {
                    tmp = tmp->right; // go to bind
                    if (tmp->type == LEFT_PAREN)
                        throw new Exception("<", tmp); // error type
                    else {
                        if (tmp->type == INT || tmp->type == FLOAT)
                            // string to float
                            num2 = StringToNum(tmp->name);
                        else
                            throw new Exception("<", tmp); // error type
                    } // else
                } // else if
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else if (tmp->type == INT || tmp->type == FLOAT)
                // string to float
                num2 = StringToNum(tmp->name);
            else if (tmp->type == RIGHT_PAREN) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception("<", tmp); // error type
        } // if
        else { // LP
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu(cur);

            tmp = EvalSExp(tmp, level, printType);
            if (tmp->type == INT || tmp->type == FLOAT)
                // string to float
                num2 = StringToNum(tmp->name);
            else
                throw new Exception("<", tmp); // error type
        } // else

        if (cur != NULL) {
            if (isFirst) { // only one argu : num1
                num1 = num2;
                isFirst = false;
            } // if
            else {           // two argus : num1 and num2
                if (num1 < num2)
                    ;        // do nothing
                else       // num1 >= num2
                    isLessThan = false;
            } // else

            num1 = num2; // swap
            cur = cur->right;
        } // if

        tmp = NULL;
    } // while

    Terminal term;
    if (isLessThan)
        term.mType = TRUE;
    else // not greater than
        term.mType = NILS;

    result = mParser.SetLeaf(term);

    return result;
} // Execuate::Eval_less_than()

Node Execuate::Eval_no_less_than( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( ">=", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    float num1 = 0, num2 = 0;
    bool isFirst = true, isGreatThan = true;

    while ( cur != NULL ) {
        if ( cur->type == INITIAL ) {
            tmp = cur->left;
            if ( tmp->type == SYMBOL ) {
                if ( IsSystemFunc( tmp->name ) )
                    throw new Exception( ">=", tmp ); // error type
                else if ( IsBinding( tmp->name, tmp ) ) {
                    tmp = tmp->right; // go to bind
                    if ( tmp->type == LEFT_PAREN )
                        throw new Exception( ">=", tmp ); // error type
                    else {
                        if ( tmp->type == INT || tmp->type == FLOAT )
                            // string to float
                            num2 = StringToNum( tmp->name );
                        else
                            throw new Exception( ">=", tmp ); // error type
                    } // else
                } // else if
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else if ( tmp->type == INT || tmp->type == FLOAT )
                // string to float
                num2 = StringToNum( tmp->name );
            else if ( tmp->type == RIGHT_PAREN ) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception(">=", tmp); // error type
        } // if
        else { // LP
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu( cur );

            tmp = EvalSExp( tmp, level, printType );
            if ( tmp->type == INT || tmp->type == FLOAT )
                // string to float
                num2 = StringToNum( tmp->name );
            else
                throw new Exception( ">=", tmp ); // error type
        } // else

        if ( cur != NULL ) {
            if (isFirst) { // only one argu : num1
                num1 = num2;
                isFirst = false;
            } // if
            else {           // two argus : num1 and num2
                if (num1 >= num2)
                    ;        // do nothing
                else       // num1 < num2
                    isGreatThan = false;
            } // else

            num1 = num2; // swap
            cur = cur->right;
        } // if

        tmp = NULL;
    } // while

    Terminal term;
    if ( isGreatThan )
        term.mType = TRUE;
    else // not greater than
        term.mType = NILS;

    result = mParser.SetLeaf( term );

    return result;
} // Execuate::Eval_no_less_than()

Node Execuate::Eval_no_more_than( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( "<=", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    float num1 = 0, num2 = 0;
    bool isFirst = true, isLessThan = true;

    while ( cur != NULL ) {
        if ( cur->type == INITIAL ) {
            tmp = cur->left;
            if ( tmp->type == SYMBOL ) {
                if (IsSystemFunc(tmp->name))
                    throw new Exception("<=", tmp); // error type
                else if (IsBinding(tmp->name, tmp)) {
                    tmp = tmp->right; // go to bind
                    if (tmp->type == LEFT_PAREN)
                        throw new Exception("<=", tmp); // error type
                    else {
                        if (tmp->type == INT || tmp->type == FLOAT)
                            // string to float
                            num2 = StringToNum(tmp->name);
                        else
                            throw new Exception("<=", tmp); // error type
                    } // else
                } // else if
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else if ( tmp->type == INT || tmp->type == FLOAT )
                // string to float
                num2 = StringToNum( tmp->name );
            else if ( tmp->type == RIGHT_PAREN ) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception( "<=", tmp ); // error type
        } // if
        else { // LP
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu(cur);

            tmp = EvalSExp(tmp, level, printType);
            if (tmp->type == INT || tmp->type == FLOAT)
                // string to float
                num2 = StringToNum(tmp->name);
            else
                throw new Exception("<=", tmp); // error type
        } // else

        if ( cur != NULL ) {
            if (isFirst) { // only one argu : num1
                num1 = num2;
                isFirst = false;
            } // if
            else {           // two argus : num1 and num2
                if (num1 <= num2)
                    ;        // do nothing
                else       // num1 > num2
                    isLessThan = false;
            } // else

            num1 = num2; // swap
            cur = cur->right;
        } // if

        tmp = NULL;
    } // while

    Terminal term;
    if ( isLessThan )
        term.mType = TRUE;
    else // not greater than
        term.mType = NILS;

    result = mParser.SetLeaf( term );

    return result;
} // Execuate::Eval_no_more_than()

Node Execuate::Eval_equal_to( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( "=", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    float num1 = 0, num2 = 0;
    bool isFirst = true, isEqualTo = true;

    while ( cur != NULL ) {
        if ( cur->type == INITIAL ) {
            tmp = cur->left;
            if ( tmp->type == SYMBOL ) {
                if ( IsSystemFunc( tmp->name ) )
                    throw new Exception( "=", tmp ); // error type
                else if ( IsBinding( tmp->name, tmp ) ) {
                    tmp = tmp->right; // go to bind
                    if ( tmp->type == LEFT_PAREN )
                        throw new Exception( "=", tmp ); // error type
                    else {
                        if ( tmp->type == INT || tmp->type == FLOAT )
                            // string to float
                            num2 = StringToNum( tmp->name );
                        else
                            throw new Exception("=", tmp); // error type
                    } // else
                } // else if
                else
                    throw new Exception( tmp->name, UNBOUND_SYMBOL );
            } // if
            else if ( tmp->type == INT || tmp->type == FLOAT )
                // string to float
                num2 = StringToNum( tmp->name );
            else if ( tmp->type == RIGHT_PAREN ) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception( "=", tmp ); // error type
        } // if
        else { // LP
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu(cur);

            tmp = EvalSExp( tmp, level, printType);
            if ( tmp->type == INT || tmp->type == FLOAT )
                // string to float
                num2 = StringToNum( tmp->name );
            else
                throw new Exception( "=", tmp ); // error type
        } // else

        if (cur != NULL) {
            if (isFirst) { // only one argu : num1
                num1 = num2;
                isFirst = false;
            } // if
            else {           // two argus : num1 and num2
                if (num1 == num2)
                    ;        // do nothing
                else         // num1 != num2
                    isEqualTo = false;
            } // else

            num1 = num2; // swap
            cur = cur->right;
        } // if

        tmp = NULL;
    } // while

    Terminal term;
    if ( isEqualTo )
        term.mType = TRUE;
    else // not greater than
        term.mType = NILS;

    result = mParser.SetLeaf( term );

    return result;
} // Execuate::Eval_equal_to()

Node Execuate::Eval_string_great_than( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( "string>?", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    string str1 = "", str2 = "";
    bool isFirst = true, isGreatThan = true;

    while ( cur != NULL ) {
        if ( cur->type == INITIAL ) {
            tmp = cur->left;
            if ( tmp->type == SYMBOL ) {
                if ( IsSystemFunc( tmp->name ) )
                    throw new Exception( "string>?", tmp ); // error type
                else if ( IsBinding( tmp->name, tmp ) ) {
                    tmp = tmp->right; // go to bind
                    if ( tmp->type == LEFT_PAREN )
                        throw new Exception( "string>?", tmp ); // error type
                    else {
                        if ( tmp->type == STRING )
                            str2 = GetStr( tmp->name );
                        else
                            throw new Exception( "string>?", tmp ); // error type
                    } // else
                } // else if
                else
                    throw new Exception( tmp->name, UNBOUND_SYMBOL );
            } // if
            else if ( tmp->type == STRING )
                str2 = GetStr( tmp->name );
            else if ( tmp->type == RIGHT_PAREN ) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception( "string>?", tmp ); // error type
        } // if
        else { // LP
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu( cur );

            tmp = EvalSExp(tmp, level, printType);
            if ( tmp->type == STRING )
                str2 = GetStr(tmp->name);
            else
                throw new Exception( "string>?", tmp ); // error type
        } // else

        if (cur != NULL) {
            if (isFirst) { // only one argu : num1
                str1 = str2;
                isFirst = false;
            } // if
            else {           // two argus : str1 and str2
                if (str1.compare(str2) > 0)
                    ;        // do nothing
                else         // str1 <= str2
                    isGreatThan = false;
            } // else

            str1 = str2; // swap
            cur = cur->right;
        } // if

        tmp = NULL;
    } // while

    Terminal term;
    if (isGreatThan)
        term.mType = TRUE;
    else // not greater than
        term.mType = NILS;

    result = mParser.SetLeaf(term);

    return result;
} // Execuate::Eval_string_great_than()

Node Execuate::Eval_string_less_than( Node cur, int level, int arguCount, int printType ) {
    if  ( arguCount < 2 )
        throw new Exception( "string<?", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    string str1 = "", str2 = "";
    bool isFirst = true, isLessThan = true;

    while ( cur != NULL ) {
        if ( cur->type == INITIAL ) {
            tmp = cur->left;
            if ( tmp->type == SYMBOL ) {
                if ( IsSystemFunc( tmp->name ) )
                    throw new Exception( "string<?", tmp ); // error type
                else if ( IsBinding( tmp->name, tmp ) ) {
                    tmp = tmp->right; // go to bind
                    if ( tmp->type == LEFT_PAREN )
                        throw new Exception( "string<?", tmp ); // error type
                    else {
                        if ( tmp->type == STRING )
                            str2 = GetStr( tmp->name );
                        else
                            throw new Exception( "string<?", tmp ); // error type
                    } // else
                } // else if
                else
                    throw new Exception( tmp->name, UNBOUND_SYMBOL );
            } // if
            else if ( tmp->type == STRING )
                str2 = GetStr( tmp->name );
            else if ( tmp->type == RIGHT_PAREN ) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception( "string<?", tmp ); // error type
        } // if
        else { // LP
            tmp = cur; // tmp -> type == LP
            JumpToNextArgu( cur ); // cur go to next argu
            tmp = EvalSExp( tmp, level, printType );
            if ( tmp->type == STRING )
                str2 = GetStr( tmp->name );
            else
                throw new Exception( "string<?", tmp ); // error type
        } // else

        if ( cur != NULL ) {
            if ( isFirst ) { // only one argu : num1
                str1 = str2;
                isFirst = false;
            } // if
            else {           // two argus : str1 and str2
                if ( str1.compare( str2 ) < 0 )
                    ;        // do nothing
                else         // str1 >= str2
                    isLessThan = false;
            } // else

            str1 = str2; // swap
            cur = cur->right;
        } // if

        tmp = NULL;
    } // while

    Terminal term;
    if (isLessThan)
        term.mType = TRUE;
    else // not greater than
        term.mType = NILS;

    result = mParser.SetLeaf(term);

    return result;
} // Execuate::Eval_string_less_than()

Node Execuate::Eval_string_equal_to( Node cur, int level, int arguCount, int printType ) {
    if ( arguCount < 2 )
        throw new Exception( "string=?", ERROR_NUM_OF_ARGUMENTS );

    Node result = NULL, tmp = NULL;
    string str1 = "", str2 = "";
    bool isFirst = true, isEqu = true;

    while ( cur != NULL ) {
        if ( cur->type == INITIAL ) {
            tmp = cur->left;
            if (tmp->type == SYMBOL) {
                if (IsSystemFunc(tmp->name))
                    throw new Exception( "string=?", tmp ); // error type
                else if (IsBinding(tmp->name, tmp)) {
                    tmp = tmp->right; // go to bind
                    if (tmp->type == LEFT_PAREN)
                        throw new Exception( "string=?", tmp ); // error type
                    else {
                        if (tmp->type == STRING)
                            str2 = GetStr(tmp->name);
                        else
                            throw new Exception( "string=?", tmp ); // error type
                    } // else
                } // else if
                else
                    throw new Exception(tmp->name, UNBOUND_SYMBOL);
            } // if
            else if (tmp->type == STRING)
                str2 = GetStr(tmp->name);
            else if (tmp->type == RIGHT_PAREN) // tree 底下沒東西了
                cur = NULL; // 強制結束
            else
                throw new Exception( "string=?", tmp ); // error type
        } // if
        else { // LP
            tmp = cur; // tmp -> type == LP

            // cur go to next argu
            JumpToNextArgu( cur );

            tmp = EvalSExp( tmp, level, printType );
            if ( tmp->type == STRING )
                str2 = GetStr( tmp->name );
            else
                throw new Exception( "string=?", tmp ); // error type
        } // else

        if ( cur != NULL ) {
            if ( isFirst ) { // only one argu : num1
                str1 = str2;
                isFirst = false;
            } // if
            else {           // two argus : str1 and str2
                if ( str1.compare( str2 ) == 0 )
                    ;        // do nothing
                else         // str1 != str2
                    isEqu = false;
            } // else

            str1 = str2; // swap
            cur = cur->right;
        } // if

        tmp = NULL;
    } // while

    Terminal term;
    if ( isEqu )
        term.mType = TRUE;
    else // not greater than
        term.mType = NILS;

    result = mParser.SetLeaf( term );

    return result;
} // Execuate::Eval_string_equal_to()

Node Execuate::Eval_eqv(Node cur, int level, int arguCount, int printType) {
    if ( arguCount != 2 )
        throw new Exception( "eqv?", ERROR_NUM_OF_ARGUMENTS );

    Node tmp = NULL;
    Node arg1 = NULL, arg2 = NULL;

    // first argu.
    if ( cur->type == INITIAL ) {
        tmp = cur->left;
        if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                arg1 = tmp;
            else if (IsBinding(tmp->name, tmp))
                arg1 = tmp->right;
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // if
        else
            arg1 = tmp;

        // go to next argu
        cur = cur->right;
    } // if
    else { // LP
        tmp = cur;

        JumpToNextArgu( cur );
        arg1 = EvalSExp( tmp, level, printType );

        // go to next argu
        cur = cur->right;
    } // else

    // second argu.
    tmp = NULL;
    if ( cur->type == INITIAL ) {
        tmp = cur->left;
        if (tmp->type == SYMBOL) {
            if (IsSystemFunc(tmp->name))
                arg2 = tmp;
            else if (IsBinding(tmp->name, tmp))
                arg2 = tmp->right;
            else
                throw new Exception(tmp->name, UNBOUND_SYMBOL);
        } // if
        else
            arg2 = tmp;
    } // if
    else { // LP
        tmp = cur;
        arg2 = EvalSExp( tmp, level, printType );
    } // else 

    Terminal term;
    stringstream ss1, ss2;
    int i_num1 = 0, i_num2 = 0;
    float f_num1 = 0, f_num2 = 0;

    // compare arg1, arg2
    if ( arg1->type == arg2->type ) {
        if ( arg1->type == SYMBOL ) {
            if (arg1->name == arg2->name)
                term.mType = TRUE;
            else
                term.mType = NILS;
        } // if
        else if ( arg1->type == INT ) {
            // str to int
            ss1 << arg1->name;
            ss2 << arg2->name;

            // assign
            ss1 >> i_num1;
            ss2 >> i_num2;

            if (i_num1 == i_num2)
                term.mType = TRUE;
            else
                term.mType = NILS;
        } // else if
        else if ( arg1->type == FLOAT ) {
            // str to int
            ss1 << arg1->name;
            ss2 << arg2->name;

            // assign
            ss1 >> f_num1;
            ss2 >> f_num2;

            if (f_num1 == f_num2)
                term.mType = TRUE;
            else
                term.mType = NILS;
        } // else if
        else if ( arg1->type == STRING ) {
            if (arg1 == arg2)
                term.mType = TRUE;
            else
                term.mType = NILS;
        } // else if
        else if ( arg1->type == TRUE || arg1->type == NILS )
            term.mType = TRUE;
        else if ( arg1->type == LEFT_PAREN ) {
            if (arg1 == arg2)
                term.mType = TRUE;
            else
                term.mType = NILS;
        } // else if
    } // if
    else
        term.mType = NILS;

    Node result = NULL;
    result = mParser.SetLeaf( term );

    return result;
} // Execuate::Eval_eqv()

Node Execuate::Eval_equal_tree( Node cur, int level, int arguCount, int printType ) {
    if (arguCount != 2)
        throw new Exception( "euqal?", ERROR_NUM_OF_ARGUMENTS );

    Node tmp = NULL;
    Node arg1 = NULL, arg2 = NULL;

    // first argu.
    if ( cur->type == INITIAL ) {
        tmp = cur->left;
        if ( tmp->type == SYMBOL ) {
            if ( IsSystemFunc( tmp->name ) )
                arg1 = tmp;
            else if ( IsBinding( tmp->name, tmp ) )
                arg1 = tmp->right;
            else
                throw new Exception( tmp -> name, UNBOUND_SYMBOL );
        } // if
        else
            arg1 = tmp;

        // go to next argu
        cur = cur->right;
    } // if
    else { // LP
        tmp = cur;

        JumpToNextArgu( cur );
        arg1 = EvalSExp( tmp, level, printType );

        // go to next argu
        cur = cur->right;
    } // else

    // second argu.
    tmp = NULL;
    if ( cur->type == INITIAL ) {
        tmp = cur->left;
        if ( tmp->type == SYMBOL ) {
            if ( IsSystemFunc( tmp->name ) )
                arg2 = tmp;
            else if ( IsBinding( tmp->name, tmp ) )
                arg2 = tmp->right;
            else
                throw new Exception( tmp->name, UNBOUND_SYMBOL );
        } // if
        else
            arg2 = tmp;
    } // if
    else { // LP
        tmp = cur;
        arg2 = EvalSExp( tmp, level, printType );
    } // else 

    Terminal term;
    stringstream ss1, ss2;
    int i_num1 = 0, i_num2 = 0;
    float f_num1 = 0, f_num2 = 0;

    // compare arg1, arg2
    if ( arg1->type == arg2->type ) {
        if ( arg1->type == SYMBOL ) {
            if ( arg1->name == arg2->name )
                term.mType = TRUE;
            else
                term.mType = NILS;
        } // if
        else if ( arg1->type == INT ) {
            // str to int
            ss1 << arg1->name;
            ss2 << arg2->name;

            // assign
            ss1 >> i_num1;
            ss2 >> i_num2;

            if ( i_num1 == i_num2 )
                term.mType = TRUE;
            else
                term.mType = NILS;
        } // else if
        else if ( arg1->type == FLOAT ) {
            // str to int
            ss1 << arg1->name;
            ss2 << arg2->name;

            // assign
            ss1 >> f_num1;
            ss2 >> f_num2;

            if ( f_num1 == f_num2 )
                term.mType = TRUE;
            else
                term.mType = NILS;
        } // else if
        else if ( arg1->type == STRING ) {
            if ( arg1->name == arg2->name )
                term.mType = TRUE;
            else
                term.mType = NILS;
        } // else if
        else if ( arg1 -> type == TRUE || arg1 -> type == NILS )
            term.mType = TRUE;
        else if ( arg1 -> type == LEFT_PAREN ) {
            if ( CompareTwoTree( arg1, arg2 ) )
                term.mType = TRUE;
            else
                term.mType = NILS;
        } // else if
    } // if
    else
        term.mType = NILS;

    Node result = NULL;
    result = mParser.SetLeaf( term );

    return result;
} // Execuate::Eval_equal_tree()

Node Execuate::CloneTree( Node cur, int& p_count ) {
    if ( cur == NULL )
        return NULL;
    Node newNode = NULL;
    newNode = GetNodeData( cur );

    if ( newNode->type == LEFT_PAREN )
        p_count++;
    else if ( newNode->type == RIGHT_PAREN ) {
        p_count--;
        if ( p_count == 0 ) // 做完一個 S_exp
            return newNode;
    } // else if
    else {
        if ( p_count == 0 )
            return NULL;
    } // else 

    newNode->left = CloneTree( cur->left, p_count );
    newNode->right = CloneTree( cur->right, p_count );

    return newNode;
} // Execuate::CloneTree()

Node Execuate::GetNodeData( Node cur ) {
    Node newNode = new Token;

    newNode->left = NULL;
    newNode->right = NULL;
    newNode->type = cur->type;
    newNode->name = cur->name;

    return newNode;
} // Execuate::GetNodeData()

Node Execuate::GetBind( Node cur ) {
    Node clone = NULL;
    int p_count = 0;
    Terminal tmp;

    if ( cur -> type == LEFT_PAREN )
        clone = CloneTree( cur, p_count );
    else {
        tmp.mName = cur -> name;
        tmp.mType = cur -> type;
        clone = mParser.SetLeaf( tmp );
    } // else

    return clone;
} // Execuate::GetBind()

Node Execuate::NewBind( Node bind, string name, int type ) {
    Node walk = mBindRoot;

    if ( mBindRoot == NULL ) { // first bind
        mBindRoot = new Token;

        // symbol data
        mBindRoot -> name = name;
        if ( IsSystemFunc( bind -> name ) )
            mBindRoot -> type = INTER_FUNCTION;
        else
            mBindRoot -> type = type;

        mBindRoot -> left = NULL;

        // bind data
        mBindRoot -> right = bind;
        bind = NULL;

        walk = mBindRoot;
        return walk;
    } // if

    while ( walk -> left != NULL )
        walk = walk -> left;

    // go to new symbol
    walk -> left = new Token;
    walk = walk -> left;

    // assign data
    walk -> name = name;
    if ( IsSystemFunc( bind -> name ) )
        walk -> type = INTER_FUNCTION;
    else
        walk -> type = type;

    walk -> left = NULL;

    // bind data
    walk -> right = bind;
    bind = NULL;

    return walk;
} // Execuate::NewBind()

void Execuate::CheckPureList( Node cur, Node root ) {
    Node clone = NULL;
    int count = 0;
    clone = CloneTree( root, count ); // 先記錄 等下有錯可以丟

    while ( cur != NULL ) {
        if ( cur->type == INITIAL || cur->type == DOT_PAREN )
            ; // do nothing go next
        else if ( cur->type == LEFT_PAREN ) {
            JumpToNextArgu( cur );
        } // else if
        else if ( cur->type == DOT )
            throw new Exception( NON_LIST, clone );

        cur = cur->right;
    } // while 

    mParser.DeleteTree(clone);
} // Execuate::CheckPureList()

void Execuate::PrintEval( Node cur, int type ) {
    if ( type == DF )
        cout << cur->name << " defined" << endl;
    else if ( type == NORMAL )
        PrintTree( cur, type );
} // Execuate::PrintEval()

void Execuate::PrintTree( Node cur, int printType ) {
    if ( cur != NULL ) {
        if ( cur->type != DOT_PAREN && cur->type != INITIAL ) {
            if (cur->type == LEFT_PAREN) {
                HowToPrint( cur->name, cur->type, mNumOfSpace, mPreTokenIsRightParen, printType );
                mPreTokenIsRightParen = true;
                mNumOfSpace = mNumOfSpace + 2;
            } // if
            else if ( cur->type == RIGHT_PAREN ) {
                mNumOfSpace = mNumOfSpace - 2;
                HowToPrint( cur->name, cur->type, mNumOfSpace, mPreTokenIsRightParen, printType );
                mPreTokenIsRightParen = false;
            } // else if
            else {
                HowToPrint( cur->name, cur->type, mNumOfSpace, mPreTokenIsRightParen, printType );
                mPreTokenIsRightParen = false;
            } // else
        } // if

        PrintTree( cur->left, printType );
        PrintTree( cur->right, printType );
    } // if
} // Parser::PrintTree()

void Execuate::TypeChange( Node & cur ) {
    if ( cur != NULL ) {
        if ( cur -> type == SYMBOL )
            cur -> type = ATOM;

        TypeChange( cur -> left );
        TypeChange( cur -> right );
    } // if
} // Execuate::TypeChange()

void Execuate::HowToPrint( string name, int type, int space, bool isRightParen, int printType ) {
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
    else if ( type == SYMBOL ) {
        if ( printType == NORMAL ) {
            if ( IsSystemFunc( name ) )
                name = "#<procedure " + name + ">";
        } // if

        if ( isRightParen )
            cout << name;
        else
            CountSpace( name, type, space );

        cout << endl;
    } // else if
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

void Execuate::CountSpace( string name, int type, int space ) {
    stringstream ss;
    int i = 0;
    float f = 0;

    for ( int j = 0 ; j < space ; j ++ )
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

void Execuate::JumpToNextArgu( Node & cur ) {
    int p_count = 1;
    while ( p_count != 0 ) {
        cur = cur -> right;
        if ( cur -> left -> type == RIGHT_PAREN )
            p_count--;
        else if ( cur -> type == LEFT_PAREN )
            p_count ++;
    } // while
} // Execuate::JumpToNextArgu()

bool Execuate::CompareTwoTree( Node root1, Node root2 ) {
    stringstream ss1, ss2;
    int i_num1 = 0, i_num2 = 0;
    float f_num1 = 0, f_num2 = 0;

    // Check if both the trees are empty
    if ( root1 == NULL && root2 == NULL )
        return true;

    // If any one of the tree is non-empty
    // and other is empty, return false
    else if ( root1 != NULL && root2 == NULL )
        return false;
    else if ( root1 == NULL && root2 != NULL )
        return false;
    else {
        // Check if current data of both trees equal
        // and recursively check for left and right subtrees
        if ( IsDataSame( root1, root2 ) && CompareTwoTree( root1 -> left, root2 -> left ) && CompareTwoTree( root1 -> right, root2 -> right ) )
            return true;
        else
            return false;
    } // else
} // Execuate::CompareTwoTree()

bool Execuate::IsDataSame( Node root1, Node root2 ) {
    stringstream ss1, ss2;
    int i_num1 = 0, i_num2 = 0;
    float f_num1 = 0, f_num2 = 0;

    if ( root1 -> type == root2 -> type ) {
        if ( root1 -> type == SYMBOL ) {
            if ( root1 -> name == root2 -> name )
                return true;
            else
                return false;
        } // if
        else if ( root1 -> type == INT ) {
            // str to int
            ss1 << root1->name;
            ss2 << root2->name;

            // assign
            ss1 >> i_num1;
            ss2 >> i_num2;

            if ( i_num1 == i_num2 )
                return true;
            else
                return false;
        } // else if
        else if ( root1 -> type == FLOAT ) {
            // str to int
            ss1 << root1 -> name;
            ss2 << root2 -> name;

            // assign
            ss1 >> f_num1;
            ss2 >> f_num2;

            if ( f_num1 == f_num2 )
                return true;
            else
                return false;
        } // else if
        else if ( root1 -> type == STRING ) {
            if ( root1 -> name == root2 -> name )
                return true;
            else
                return false;
        } // else if
        else // LP RP TRUE NILS
            return true;
    } // if
    else
        return false;

    return false;
} // Execuate::IsDataSame()

bool Execuate::IsSystemFunc( string name ) {
    // check it is a system function
    if ( name.compare( "cons" ) == 0 || name.compare( "list" ) == 0 || name.compare( "quote" ) == 0 ||
         name.compare( "define" ) == 0 || name.compare( "car" ) == 0 || name.compare( "cdr" ) == 0 ||
         name.compare( "atom?" ) == 0 || name.compare( "pair?" ) == 0 || name.compare( "list?" ) == 0 ||
         name.compare( "null?" ) == 0 || name.compare( "integer?" ) == 0 || name.compare( "real?" ) == 0 ||
         name.compare( "number?" ) == 0 || name.compare( "string?" ) == 0 || name.compare( "boolean?" ) == 0 ||
         name.compare( "symbol?" ) == 0 || name.compare( "+" ) == 0 || name.compare( "-" ) == 0 ||
         name.compare( "*" ) == 0 || name.compare( "/" ) == 0 || name.compare( "not" ) == 0 ||
         name.compare( "and" ) == 0 || name.compare( "or" ) == 0 || name.compare( ">" ) == 0 ||
         name.compare( "<" ) == 0 || name.compare( ">=" ) == 0 || name.compare( "<=" ) == 0 ||
         name.compare( "=" ) == 0 || name.compare( "string-append" ) == 0 || name.compare( "string>?" ) == 0 ||
         name.compare( "string<?" ) == 0 || name.compare( "string=?" ) == 0 || name.compare( "eqv?" ) == 0 ||
         name.compare( "equal?" ) == 0 || name.compare( "begin" ) == 0 || name.compare( "if" ) == 0 ||
         name.compare( "cond" ) == 0 || name.compare( "clean-environment" ) == 0 || name.compare( "'" ) == 0 ||
         name.compare( "exit" ) == 0 )
        return true;

    return false;
} // Execuate::IsSystemFunc()

bool Execuate::IsBinding( string name, Node & cur ) {
    Node head = mBindRoot;
    while ( head != NULL && head -> name.compare( name ) != 0 ) {
        head = head->left;
    } // while

    if ( head == NULL )
        return false;
    else
        cur = head;

    return true;
} // Execuate::IsBinding()

bool Execuate::IsThereDot( Node cur ) {
    // like CheckPureList, but do not throw error
    int count = 0;

    while ( cur != NULL ) {
        if ( cur -> type == INITIAL || cur -> type == DOT_PAREN )
            ; // do nothing go next
        else if ( cur -> type == LEFT_PAREN ) {
            count ++;
            cur = cur -> right;
            while ( count != 0 ) {
                if ( cur -> left -> type == RIGHT_PAREN )
                    count--;
                else if ( cur -> type == LEFT_PAREN )
                    count ++;

                if ( count != 0 )
                    cur = cur -> right;
            } // while
            // count == 0
        } // else if
        else if ( cur -> type == DOT )
            return true;

        cur = cur -> right;
    } // while 

    return false;
} // Execuate::IsThereDot()

int Execuate::CheckNumOfArgument( Node cur ) {
    int argu = 0;

    while ( cur != NULL ) {
        if ( cur -> type == INITIAL ) {
            if ( cur -> left -> type == SYMBOL || cur -> left -> type == NILS || cur -> left -> type == STRING ||
                 cur -> left -> type == TRUE || cur -> left -> type == INT || cur -> left -> type == FLOAT )
                argu ++;
            else if ( cur -> left -> type == RIGHT_PAREN )
                return argu;
        } // if
        else if ( cur -> type == LEFT_PAREN ) {
            argu ++;

            JumpToNextArgu( cur );
        } // else if 

        cur = cur -> right;
    } // while

    return argu;
} // Execuate::CheckNumOfArgument()

float Execuate::StringToNum( string str ) {
    stringstream ss;
    float num = 0;

    ss << str;
    ss >> num;
    return num;
} // Execuate::StringToNum()

string Execuate::GetStr( string input ) {
    string str = "";

    // 取 "" 中的東西
    for ( int i = 1 ; i < input.size() - 1 ; i ++ )
        str = str + input[i];

    return str;
} // Execuate::GetStr()