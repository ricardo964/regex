#ifndef __PARSER__
#define __PARSER__

#include <iostream>
#include <queue>
#include <stack>
#include "lexer.hpp"

class Parser
{
public:
    static std::queue<Token> shunting_yard(std::vector<Token> tokens);
};

#endif // __PARSER__