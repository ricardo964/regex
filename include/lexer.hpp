#ifndef __LEXER__
#define __LEXER__

#include <iostream>
#include <string>
#include <vector>

typedef enum
{
    SYMBOL,
    UNION,
    CONCATENATION,
    STAR,
    LPARAM,
    RPARAM
} TokenType;

class Token
{
public:
    TokenType type;
    std::string lexeme;

    Token(TokenType _type, std::string _lexeme);
    static std::vector<Token> tokenizer(std::string source);
};

#endif // __LEXER__