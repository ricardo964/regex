#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <string>
#include <set>
#include <unordered_map>

#define EPSILON -1

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

    Token(TokenType _type, std::string _lexeme) : type(_type), lexeme(_lexeme) {}
};

class AstNode
{
public:
    Token token;
    AstNode *leftNode;
    AstNode *rightNode;

    AstNode(Token _token, AstNode *_leftNode, AstNode *_rightNode) : token(_token), leftNode(_leftNode), rightNode(_rightNode) {};
};

std::vector<Token> _lexer(std::string source)
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

std::queue<Token> shuntingYard(std::vector<Token> tokens)
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

class TransitionKey
{
public:
    int state;
    char input;

    TransitionKey(int _state, char _input) : state(_state), input(_input) {}

    bool operator==(const TransitionKey &other) const
    {
        return this->state == other.state && this->input == other.state;
    }
};

struct TransitionKeyHash
{
    size_t operator()(const TransitionKey &k) const
    {
        return std::hash<int>()(k.state) ^ std::hash<int>()(k.input);
    }
};

int state = 0;
class ENonDeterministicFiniteAutomata
{
public:
    std::set<int> states;
    std::unordered_map<TransitionKey, std::set<int>, TransitionKeyHash> transition_table;
    int start_state;
    int end_state;

    void defaultAutomata()
    {
        this->start_state = state;
        this->end_state = state + 1;
        this->states.insert(this->start_state);
        this->states.insert(this->end_state);

        state = state + 2;
    }

    ENonDeterministicFiniteAutomata()
    {
        this->defaultAutomata();
    }

    ENonDeterministicFiniteAutomata(char symbol)
    {
        this->defaultAutomata();
        this->transition_table[TransitionKey(this->start_state, symbol)].insert(this->end_state);
    }

    // union
    ENonDeterministicFiniteAutomata operator+(const ENonDeterministicFiniteAutomata &k) const
    {
        ENonDeterministicFiniteAutomata automata;

        // state union create a private function
        for (int s : this->states)
            automata.states.insert(s);
        for (int s : k.states)
            automata.states.insert(s);

        // constrain of union
        automata.transition_table[TransitionKey(automata.start_state, EPSILON)].insert(this->start_state);
        automata.transition_table[TransitionKey(automata.start_state, EPSILON)].insert(k.start_state);
        automata.transition_table[TransitionKey(this->end_state, EPSILON)].insert(automata.end_state);
        automata.transition_table[TransitionKey(k.end_state, EPSILON)].insert(automata.end_state);

        // copy create a private function
        for (const auto &[state, input] : this->transition_table)
            for (const auto s : input)
                automata.transition_table[state].insert(s);

        for (const auto &[state, input] : k.transition_table)
            for (const auto s : input)
                automata.transition_table[state].insert(s);

        return automata;
    }

    // concatenation
    ENonDeterministicFiniteAutomata operator-(const ENonDeterministicFiniteAutomata &k) const
    {
        ENonDeterministicFiniteAutomata automata;

        // state union create a private function
        for (int s : this->states)
            automata.states.insert(s);
        for (int s : k.states)
            automata.states.insert(s);

        // constrain of union
        automata.transition_table[TransitionKey(automata.start_state, EPSILON)].insert(this->start_state);
        automata.transition_table[TransitionKey(this->end_state, EPSILON)].insert(k.start_state);
        automata.transition_table[TransitionKey(k.end_state, EPSILON)].insert(automata.end_state);

        // copy create a private function
        for (const auto &[state, input] : this->transition_table)
            for (const auto s : input)
                automata.transition_table[state].insert(s);

        for (const auto &[state, input] : k.transition_table)
            for (const auto s : input)
                automata.transition_table[state].insert(s);

        return automata;
    }

    // kleene clouse
    ENonDeterministicFiniteAutomata operator!() const
    {
        ENonDeterministicFiniteAutomata automata;

        for (int s : this->states)
            automata.states.insert(s);

        automata.transition_table[TransitionKey(automata.start_state, EPSILON)].insert(this->start_state);
        automata.transition_table[TransitionKey(this->end_state, EPSILON)].insert(automata.end_state);
        automata.transition_table[TransitionKey(automata.start_state, EPSILON)].insert(automata.end_state);
        automata.transition_table[TransitionKey(this->end_state, EPSILON)].insert(this->start_state);

        for (const auto &[state, input] : this->transition_table)
            for (const auto s : input)
                automata.transition_table[state].insert(s);

        return automata;
    }

    std::set<int> move(int state, char input)
    {
        return this->transition_table[TransitionKey(state, input)];
    }

    std::set<int> eclosures(int state)
    {
        std::set<int> visited;
        std::queue<int> queue;

        visited.insert(state);
        queue.push(state);

        while (!queue.empty())
        {
            int current_state = queue.front();
            queue.pop();

            for (int next : this->move(current_state, EPSILON))
            {
                if (!visited.count(next))
                {
                    visited.insert(next);
                    queue.push(next);
                }
            }
        }

        return visited;
    }

    bool eval(std::string plaintext)
    {
        std::set<int> current_states;

        current_states = this->eclosures(this->start_state);
        for (char c : plaintext)
        {
            std::set<int> temp;
            for (int state : current_states)
            {
                for (int next_state : this->move(state, c))
                    temp.insert(next_state);

                for (int e_state : this->eclosures(state))
                {
                    for (int next_state : this->move(e_state, c))
                        temp.insert(next_state);
                }
            }

            current_states = temp;
        }

        std::set<int> temp;
        for (int state : current_states)
        {
            for (int e_state : this->eclosures(state))
                temp.insert(e_state);
        }

        for (int state : temp)
            current_states.insert(state);

        if (current_states.find(this->end_state) == current_states.end())
            return false;
        else
            return true;
    }
};

ENonDeterministicFiniteAutomata build(std::queue<Token> queue)
{
    ENonDeterministicFiniteAutomata automata;
    std::stack<ENonDeterministicFiniteAutomata> stack;

    while (!queue.empty())
    {
        Token token = queue.front();
        queue.pop();

        switch (token.type)
        {
        case UNION:
        {
            ENonDeterministicFiniteAutomata right = stack.top();
            stack.pop();
            ENonDeterministicFiniteAutomata left = stack.top();
            stack.pop();
            stack.push(left + right);

            break;
        }

        case CONCATENATION:
        {
            ENonDeterministicFiniteAutomata right = stack.top();
            stack.pop();
            ENonDeterministicFiniteAutomata left = stack.top();
            stack.pop();
            stack.push(left - right);

            break;
        }

        case STAR:
        {
            ENonDeterministicFiniteAutomata _new = stack.top();
            stack.pop();
            stack.push(!_new);

            break;
        }

        default:
            ENonDeterministicFiniteAutomata new_automata(token.lexeme[0]);
            stack.push(new_automata);
        }
    }

    automata = stack.top();
    stack.pop();

    return automata;
}

int main()
{
    std::string buf;

    while (true)
    {
        std::cout << ">>";
        std::getline(std::cin, buf);

        if (buf.empty())
            break;

        std::vector<Token> tokens = _lexer(buf);
        std::queue<Token> queue = shuntingYard(tokens);
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
