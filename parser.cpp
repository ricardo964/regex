#include <iostream>
#include <stack>
#include "include/parser.hpp"

std::queue<Token> Parser::shunting_yard(std::vector<Token> tokens)
{
    std::stack<Token> stack;
    std::queue<Token> queue;
    int index = 0;

    while (index < tokens.size())
    {

        if (tokens[index].type == SYMBOL)
        {
            queue.push(tokens[index]);
            index++;
            continue;
        }

        if (
            tokens[index].type == UNION or
            tokens[index].type == STAR or
            tokens[index].type == CONCATENATION)
        {
            if (stack.empty())
                stack.push(tokens[index]);
            else
            {
                while (!stack.empty())
                {
                    Token token = stack.top();
                    if (token.type > tokens[index].type)
                    {
                        queue.push(token);
                        stack.pop();
                    }
                    else
                        break;
                }

                stack.push(tokens[index]);
            }

            index++;
            continue;
        }

        // if (tokens[index].type == LPARAM)
        // {
        // }
    }

    while (!stack.empty())
    {
        Token token = stack.top();
        queue.push(token);
        stack.pop();
    }

    return queue;
}