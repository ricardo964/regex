#include "include/automata.hpp"

TransitionKey::TransitionKey(int _state, char _input) : state(_state), input(_input) {};

bool TransitionKey::operator==(const TransitionKey &other) const
{
    return this->state == other.state && this->input == other.state;
}

size_t TransitionKeyHash::operator()(const TransitionKey &k) const
{
    return std::hash<int>()(k.state) ^ std::hash<int>()(k.input);
}

void ENonDeterministicFiniteAutomata::defaultAutomata()
    {
        
        static int state = 0;

        this->start_state = state;
        this->end_state = state + 1;
        this->states.insert(this->start_state);
        this->states.insert(this->end_state);

        state = state + 2;
    }

ENonDeterministicFiniteAutomata::ENonDeterministicFiniteAutomata()
    {
        this->defaultAutomata();
    }

ENonDeterministicFiniteAutomata::ENonDeterministicFiniteAutomata(char symbol)
    {
        this->defaultAutomata();
        this->transition_table[TransitionKey(this->start_state, symbol)].insert(this->end_state);
    }

ENonDeterministicFiniteAutomata ENonDeterministicFiniteAutomata::operator+(const ENonDeterministicFiniteAutomata &k) const
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

ENonDeterministicFiniteAutomata ENonDeterministicFiniteAutomata::operator-(const ENonDeterministicFiniteAutomata &k) const
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


 ENonDeterministicFiniteAutomata ENonDeterministicFiniteAutomata::operator!() const
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

std::set<int> ENonDeterministicFiniteAutomata::move(int state, char input)
    {
        return this->transition_table[TransitionKey(state, input)];
    }

std::set<int> ENonDeterministicFiniteAutomata::eclosures(int state)
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

bool ENonDeterministicFiniteAutomata::eval(std::string plaintext)
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