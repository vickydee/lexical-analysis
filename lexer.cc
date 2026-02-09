#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR",
    // Add labels for new token types here (as string)
    "REALNUM", "BASE08NUM", "BASE16NUM"
};

/*
...
  */

Token LexicalAnalyzer::ScanNumber()
{ 
    int flag8o9 = 0; //saw integer 8/9
    int flag0 = 0; //"0"

    char c;
    
    tmp.lexeme = "";
    tmp.line_no = line_no;

    input.GetChar(c);
    
    // Singular Char Lexemes
    if (isdigit(c)) {
        // flag0 singular char case
        if (c == '0') {
            tmp.lexeme = "0";
            flag0 = 1;
        } 
        else {
            while (!input.EndOfInput() && isdigit(c)) {
                tmp.lexeme += c;
                // flag8 singular char case
                if (c == '8' || c == '9') {
                    flag8o9 = 1;
                }
                input.GetChar(c);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
        }
        if (input.EndOfInput()) {
            tmp.token_type = NUM;
            return tmp;
        }
        // Check for REALNUM, BASE08NUM and BASE16NUM here

        // REALNUM
        
        char la; //lookahead variable
        char f; //lookahead variable for fractional (i.e. after decimal point) component
        input.GetChar(la);
        if(la == '.') {
            string frac = "";
            int hasNonZero = 0;
            if (!input.EndOfInput()) {
                input.GetChar(f);
                while (!input.EndOfInput() && isdigit(f)) {
                frac += f;
                if (f != '0') {
                    hasNonZero = 1;
                }
                input.GetChar(f);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(f);
            } 
        }
        // If no fractional digits at all
        // Tokenize as NUM
        if (frac.size() == 0) {
            input.UngetChar(la);     // put back '.'
            tmp.token_type = NUM;
            return tmp;
        }           
        // Special case: don't make 0.000 a REALNUM
        if (flag0 == 1 && hasNonZero == 0) {
            // put back the digits and the dot for later tokenization
            input.UngetString(frac);
            input.UngetChar(la);
            
            tmp.token_type = NUM;
            return tmp;
        }
        // Otherwise it's REALNUM
        tmp.lexeme += '.';
        tmp.lexeme += frac;
        tmp.token_type = REALNUM;
        return tmp;
    }
            
    /*   input.GetChar();
         scanning in variables () {
            int flagnotzero = 0;
            if (input.getChar(nextC) == '.') {
                while (!input.EndOfInput() && isdigit(c)) {
                    input.GetChar(c);
                    if(c != 0) {
                        flagnotzero = 1;
                        input.UngetChar(c);
                    }
                }
                if (flagnotzero == 0 && flag0 == 1) {
                    tmp.token_type = NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
            }
            input.UngetChar(nextC);
        }
    */        


    // BASE08NUM / BASE16NUM or <digits>x08 / <digits>x16
        
    if (la == 'x') {
        char a, b;
        if (!input.EndOfInput()) {
            input.GetChar(a); 
        } 
        else {
            a = '\0';
        }
        if (!input.EndOfInput()) {
            input.GetChar(b); 
        } 
        else {
            b = '\0';
        }
        // x08
        if (a == '0' && b == '8' && flag8o9 == 0) {
            tmp.lexeme += "x08";
            tmp.token_type = BASE08NUM;
            return tmp;
        }

        // x16
        if (a == '1' && b == '6') {
            tmp.lexeme += "x16";
            tmp.token_type = BASE16NUM;
            return tmp;
        }

        // failed match -> backtrack
        if (b != '\0') input.UngetChar(b);
        if (a != '\0') input.UngetChar(a);
        input.UngetChar(la);
        tmp.token_type = NUM;
        return tmp;
        }
    
        // hex digits A-F appear after initial digits, must end with x16
        auto isHexTail = [](char ch) {
            return isdigit(ch) || (ch >= 'A' && ch <= 'F');
        };
    
        if (la >= 'A' && la <= 'F') {
            string tail = "";
            tail += la;
    
            char h;
            input.GetChar(h);
            while (!input.EndOfInput() && isHexTail(h)) {
                tail += h;
                input.GetChar(h);
            }
            if (!input.EndOfInput()) input.UngetChar(h);
    
            // now expect x16
            char x, one, six;
            if (!input.EndOfInput()) input.GetChar(x); else x = '\0';
            if (!input.EndOfInput()) input.GetChar(one); else one = '\0';
            if (!input.EndOfInput()) input.GetChar(six); else six = '\0';
    
            if (x == 'x' && one == '1' && six == '6') {
                tmp.lexeme += tail;
                tmp.lexeme += "x16";
                tmp.token_type = BASE16NUM;
                return tmp;
            }
    
            // backtrack everything
            if (six != '\0') input.UngetChar(six);
            if (one != '\0') input.UngetChar(one);
            if (x != '\0') input.UngetChar(x);
            input.UngetString(tail);
            tmp.token_type = NUM;
            return tmp;
        }
    
        // ---- Default: not a special number ----
        input.UngetChar(la);
        tmp.token_type = NUM;
        return tmp;
    }
    else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

/*
...
  */
