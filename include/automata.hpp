#ifndef __AUTOMATA__
#define __AUTOMATA__

#include <iostream>
#include <queue>
#include <stack>
#include <set>
#include <unordered_map>
#include "lexer.hpp"

#define EPSILON -1

class TransitionKey
{
public:
    int state;
    char input;

    TransitionKey(int _state, char _input);
    bool operator==(const TransitionKey &other) const;
};

struct TransitionKeyHash
{
    size_t operator()(const TransitionKey &k) const;
};

class ENonDeterministicFiniteAutomata
{
public:
    std::set<int> states;
    std::unordered_map<TransitionKey, std::set<int>, TransitionKeyHash> transition_table;
    int start_state;
    int end_state;

    ENonDeterministicFiniteAutomata();
    ENonDeterministicFiniteAutomata(char symbol);
    // union
    ENonDeterministicFiniteAutomata operator+(const ENonDeterministicFiniteAutomata &k) const;
    // concatenation
    ENonDeterministicFiniteAutomata operator-(const ENonDeterministicFiniteAutomata &k) const;
    // kleene clouse
    ENonDeterministicFiniteAutomata operator!() const;
    std::set<int> move(int state, char input);
    std::set<int> eclosures(int state);
    bool eval(std::string plaintext);

private:
    void defaultAutomata();
};


ENonDeterministicFiniteAutomata build(std::queue<Token> queue);

#endif // __AUTOMATA__