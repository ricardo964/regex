#include "include/lexer.hpp"

Token::Token(TokenType _type, std::string _lexeme) : type(_type), lexeme(_lexeme) {}

std::vector<Token> Token::tokenizer(std::string source)
{
    std::vector<Token> tokens;
    int index = 0;

    for (int index = 0; index < source.length(); index++)
    {

        switch (source[index])
        {
        case '|':
            tokens.push_back(
                Token(UNION, source.substr(index, 1)));
            break;

        case '*':
            tokens.push_back(
                Token(STAR, source.substr(index, 1)));
            break;

        case '(':
            tokens.push_back(
                Token(RPARAM, source.substr(index, 1)));
            break;

        case ')':
            tokens.push_back(
                Token(LPARAM, source.substr(index, 1)));
            break;

        default:
            tokens.push_back(
                Token(SYMBOL, source.substr(index, 1)));

            if (index + 1 < source.length())
                if (source[index + 1] != '|' and source[index + 1] != '*')
                    tokens.push_back(
                        Token(CONCATENATION, std::string(".")));
        }
    }

    return tokens;
}