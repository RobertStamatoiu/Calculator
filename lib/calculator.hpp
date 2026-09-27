/**
 * @file calculator.hpp
 * @brief A self contained header containing the implementation of the calculate function, meant to take an arbitrary string and calculate the result, and return it as a string
 * @include <cmath>, <algorithm>, <string>, <vector>, <optional>, <stdexcept>
 * @author Stamatoiu Robert Nicolas
 */

#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <optional>
#include <stdexcept>

namespace srn{

    namespace detail{
        /**
         * Checks is a string is a number literal, real or whole number
         * @param raw the raw string of chaarcters
         * @returns `true` if the string is a number literal and `false` otherwise
         */
        bool IsNumberLiteral(std::string raw){
            bool FoundDecimalPoint = false;
            for(const char& c : raw)
                if(std::isdigit(c)) continue;
                else if (c == '.' && !FoundDecimalPoint) {FoundDecimalPoint = true; continue;}
                else if (c == '.' && FoundDecimalPoint) return false;
                else return false;
            return true;
        }
    }

    enum struct TokenType{
        NumberLiteral,
        OperatorPlus,
        OperatorMinus,
        OperatorMultiply,
        OperatorDivide,
        OperatorExponentiate,
        OpenParenthesis,
        CloseParenthesis
    };

    struct Token{
        TokenType type;
        std::optional<std::string> data = std::nullopt;
    };

    /**
     * `tokenise` takes a raw string inpuit and spilts it into semantically meaningful tokens
     * @param raw the raw string of characters
     * @returns A vector of tokens
     * @throws `std::invalid_argument` if it meets an unsupported character
     */
    std::vector<Token> tokenise(std::string raw){
        std::vector<Token> result = {};
        std::string current;
        for(const char& character : raw){
            if(std::isdigit(character)) current += character;
            else if (character == '+') {
                if(!current.empty()) {result.push_back(Token{TokenType::NumberLiteral, current}); current = "";}
                result.push_back(Token{TokenType::OperatorPlus});
            }
            else if (character == '-'){
                if(!current.empty()) {result.push_back(Token{TokenType::NumberLiteral, current}); current = "";}
                result.push_back(Token{TokenType::OperatorMinus});
            }
            else if (character == '*'){
                if(!current.empty()) {result.push_back(Token{TokenType::NumberLiteral, current}); current = "";}
                result.push_back(Token{TokenType::OperatorMultiply});
            }
            else if (character == '/'){
                if(!current.empty()) {result.push_back(Token{TokenType::NumberLiteral, current}); current = "";}
                result.push_back(Token{TokenType::OperatorDivide});
            }
            else if (character == '^'){
                if(!current.empty()) {result.push_back(Token{TokenType::NumberLiteral, current}); current = "";}
                result.push_back(Token{TokenType::OperatorExponentiate});
            }
            else if (character == '('){
                if(!current.empty()) {result.push_back(Token{TokenType::NumberLiteral, current}); current = "";}
                result.push_back(Token{TokenType::OpenParenthesis});
            }
            else if (character == ')'){
                if(!current.empty()) {result.push_back(Token{TokenType::NumberLiteral, current}); current = "";}
                result.push_back(Token{TokenType::CloseParenthesis});
            }
            else throw std::invalid_argument("Unsupported character \"" + current + character +"\"");
        }
    }

    class Node{
    public:
        Token token;
        Node* LeftChild = nullptr;
        Node* RightChild = nullptr;
        Node(Token _token = Token{}, Node* left, Node* right): token(_token), LeftChild(left), RightChild(right) {}
    };

    /**
     * `BuildAST` builds an Abstract Syntax Tree out of the tokens from the previous step, and validates the tokens
     * @param tokens a vector of tokens to build an AST out of
     * @return a pointer to the head of the tree
     * @throws `std::invalid_argument` if it couldn't validate the tokens
     */
    Node* BuildAST(std::vector<Token> tokens){
        Node* result = new Node{};
        for(size_t i = 0; i < tokens.size(); i++){
            Token token = tokens[i];
            if(token.type == TokenType::NumberLiteral) result->LeftChild = new Node{token};
            else if (token.type == TokenType::OpenParenthesis){
                size_t close_index = i+1;
                // we need to find the close parenthesis token
                for(close_index = i + 1; close_index < tokens.size(); close_index++)
                    if(tokens[close_index].type == TokenType::CloseParenthesis)
                        break;
                if(close_index == tokens.size() - 1 && tokens[close_index].type != TokenType::CloseParenthesis){
                    // we didn't find a closing parenthesis
                    throw std::invalid_argument("Invalid parenthesis formation; couldn't find matching closing parenthesis for the open parenthesys at index: " + std::to_string(i));
                }
                // now that we found it, we need to make a replica of the tokens vector and run BuildAST on that
                std::vector<Token> subvector(tokens.begin() + i + 1, tokens.begin() + close_index);
                Node* InnerResult = BuildAST(subvector);
                result->RightChild = InnerResult;
                i = close_index;
            } else {
                // now the token is certainly an operator. So, we will exchange the head with another one, and the current head will become the left operator
                Node* temp = result;
                result = new Node{token};
                result->LeftChild = temp; 
            } 
        }
        return result;
    }


    /**
     * `Evaluate` evaluates the AST by recursively going down it
     * @param head the head of the tree
     * @returns the result of the expression, as `double`
     * @throws `std::invalid_argument` if it cannot evaluate a branch
     */
    double Evaluate(Node* head){
        double result = 0;
        if(head->token.type == TokenType::OperatorPlus)
            return Evaluate(head->LeftChild) + Evaluate(head->RightChild);
        else if (head->token.type == TokenType::OperatorMinus)
            return Evaluate(head->LeftChild) - Evaluate(head->RightChild);
        else if (head->token.type == TokenType::OperatorMultiply)
            return Evaluate(head->LeftChild) * Evaluate(head->RightChild);
        else if (head->token.type == TokenType::OperatorDivide){
            double right = Evaluate(head->RightChild);
            if(right - 1e-9 <= 0){
                throw std::invalid_argument("division by zero detected!");
            }
            return Evaluate(head->LeftChild) / right;
        } else if (head->token.type == TokenType::OperatorExponentiate)
            return std::powl(Evaluate(head->LeftChild), Evaluate(head->RightChild));
        else if (head->token.type == TokenType::NumberLiteral)
            return std::stod(head->token.data.value());
    }
}