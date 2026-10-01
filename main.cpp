#include <iostream>
#include <vector>
#include <queue>
#include <string>

#include "include/lexer.hpp"
#include "include/parser.hpp"
#include "include/automata.hpp"

int main()
{
    std::string buf;

    while (true)
    {
        std::cout << ">>";
        std::getline(std::cin, buf);

        if (buf.empty())
            break;

        std::vector<Token> tokens = Token::tokenizer(buf);
        std::queue<Token> queue = Parser::shunting_yard(tokens);
        
        ENonDeterministicFiniteAutomata automata = build(queue);

        // std::cout << "size vector " << queue.size() << std::endl;
        // std::cout << "size stack " << tokens.size() << std::endl;

        // while (!queue.empty())
        // {
        //     auto token = queue.front();
        //     queue.pop();
        //     std::cout << token.lexeme << std::endl;
        // }

        buf.clear();
    }

    return 0;
}
