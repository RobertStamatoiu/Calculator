/**
 * @file calculator.hpp
 * @brief A self contained header containing the implementation of the calculate function, meant to take an arbitrary string and calculate the result, and return it as a string
 * @include <cmath>, <algorithm>, <string>, <vector>, <optional>, <stdexcept>
 * @author Stamatoiu Robert Nicolas
 */

#include <cmath>
#include <algorithm>
#include <cctype>
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
            if(raw.empty()) return false;
            bool foundDecimalPoint = false;
            bool seenDigit = false;

            for(size_t i = 0; i < raw.size(); ++i){
                const char c = raw[i];
                if(std::isdigit(static_cast<unsigned char>(c))) {
                    seenDigit = true;
                    continue;
                }
                if(c == '.') {
                    if(foundDecimalPoint) return false;
                    foundDecimalPoint = true;
                    continue;
                }
                return false;
            }
            return seenDigit;
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

        auto commit_number = [&](){
            if(!current.empty()){
                if(!detail::IsNumberLiteral(current))
                    throw std::invalid_argument("Invalid number literal: \"" + current + "\"");
                result.push_back(Token{TokenType::NumberLiteral, current});
                current.clear();
            }
        };

        for(const char& character : raw){
            if(std::isspace(static_cast<unsigned char>(character))) {
                commit_number();
                continue;
            }
            if(std::isdigit(static_cast<unsigned char>(character)) || character == '.'){
                current += character;
                continue;
            }
            if(character == '+') {
                commit_number();
                result.push_back(Token{TokenType::OperatorPlus});
            }
            else if (character == '-'){
                commit_number();
                result.push_back(Token{TokenType::OperatorMinus});
            }
            else if (character == '*'){
                commit_number();
                result.push_back(Token{TokenType::OperatorMultiply});
            }
            else if (character == '/'){
                commit_number();
                result.push_back(Token{TokenType::OperatorDivide});
            }
            else if (character == '^'){
                commit_number();
                result.push_back(Token{TokenType::OperatorExponentiate});
            }
            else if (character == '('){
                commit_number();
                result.push_back(Token{TokenType::OpenParenthesis});
            }
            else if (character == ')'){
                commit_number();
                result.push_back(Token{TokenType::CloseParenthesis});
            }
            else {
                throw std::invalid_argument("Unsupported character \"" + std::string(1, character) + "\"");
            }
        }

        commit_number();
        return result;
    }

    class Node{
    public:
        Token token;
        Node* LeftChild = nullptr;
        Node* RightChild = nullptr;
        Node(Token _token = Token{}, Node* left = nullptr, Node* right = nullptr): token(_token), LeftChild(left), RightChild(right) {}
    };

    /**
     * `BuildAST` builds an Abstract Syntax Tree out of the tokens from the previous step, and validates the tokens
     * @param tokens a vector of tokens to build an AST out of
     * @return a pointer to the head of the tree
     * @throws `std::invalid_argument` if it couldn't validate the tokens
     */
    Node* BuildAST(std::vector<Token> tokens){
        if(tokens.empty())
            throw std::invalid_argument("Expression is empty");

        struct Parser {
            const std::vector<Token>& tokens;
            size_t index = 0;

            explicit Parser(const std::vector<Token>& _tokens) : tokens(_tokens) {}

            Node* parseExpression() {
                Node* left = parseTerm();
                while(index < tokens.size() &&
                      (tokens[index].type == TokenType::OperatorPlus ||
                       tokens[index].type == TokenType::OperatorMinus)) {
                    Token op = tokens[index++];
                    Node* right = parseTerm();
                    Node* node = new Node{op};
                    node->LeftChild = left;
                    node->RightChild = right;
                    left = node;
                }
                return left;
            }

            Node* parseTerm() {
                Node* left = parsePower();
                while(index < tokens.size() &&
                      (tokens[index].type == TokenType::OperatorMultiply ||
                       tokens[index].type == TokenType::OperatorDivide)) {
                    Token op = tokens[index++];
                    Node* right = parsePower();
                    Node* node = new Node{op};
                    node->LeftChild = left;
                    node->RightChild = right;
                    left = node;
                }
                return left;
            }

            Node* parsePower() {
                Node* left = parseUnary();
                if(index < tokens.size() && tokens[index].type == TokenType::OperatorExponentiate) {
                    Token op = tokens[index++];
                    Node* right = parsePower();
                    Node* node = new Node{op};
                    node->LeftChild = left;
                    node->RightChild = right;
                    return node;
                }
                return left;
            }

            Node* parseUnary() {
                if(index < tokens.size() && tokens[index].type == TokenType::OperatorPlus) {
                    ++index;
                    return parseUnary();
                }
                if(index < tokens.size() && tokens[index].type == TokenType::OperatorMinus) {
                    ++index;
                    Node* operand = parseUnary();
                    Node* negate = new Node{Token{TokenType::OperatorMultiply}};
                    negate->LeftChild = new Node{Token{TokenType::NumberLiteral, "-1"}};
                    negate->RightChild = operand;
                    return negate;
                }
                return parsePrimary();
            }

            Node* parsePrimary() {
                if(index >= tokens.size())
                    throw std::invalid_argument("Unexpected end of expression");

                Token token = tokens[index++];
                if(token.type == TokenType::NumberLiteral)
                    return new Node{token};

                if(token.type == TokenType::OpenParenthesis) {
                    Node* inner = parseExpression();
                    if(index >= tokens.size() || tokens[index].type != TokenType::CloseParenthesis)
                        throw std::invalid_argument("Mismatched parentheses");
                    ++index;
                    return inner;
                }

                throw std::invalid_argument("Expected a value");
            }
        };

        Parser parser(tokens);
        Node* root = parser.parseExpression();
        if(parser.index != tokens.size())
            throw std::invalid_argument("Unexpected trailing token");
        return root;
    }


    /**
     * `Evaluate` evaluates the AST by recursively going down it
     * @param head the head of the tree
     * @returns the result of the expression, as `double`
     * @throws `std::invalid_argument` if it cannot evaluate a branch
     */
    double Evaluate(Node* head){
        if(head == nullptr)
            throw std::invalid_argument("Empty AST node");

        if(head->token.type == TokenType::OperatorPlus)
            return Evaluate(head->LeftChild) + Evaluate(head->RightChild);
        else if (head->token.type == TokenType::OperatorMinus)
            return Evaluate(head->LeftChild) - Evaluate(head->RightChild);
        else if (head->token.type == TokenType::OperatorMultiply)
            return Evaluate(head->LeftChild) * Evaluate(head->RightChild);
        else if (head->token.type == TokenType::OperatorDivide){
            double right = Evaluate(head->RightChild);
            if(std::abs(right) <= 1e-12)
                throw std::invalid_argument("division by zero detected!");
            return Evaluate(head->LeftChild) / right;
        } else if (head->token.type == TokenType::OperatorExponentiate)
            return std::powl(Evaluate(head->LeftChild), Evaluate(head->RightChild));
        else if (head->token.type == TokenType::NumberLiteral){
            if(!head->token.data.has_value())
                throw std::invalid_argument("Number literal is missing data");
            return std::stod(head->token.data.value());
        }
        else throw std::invalid_argument("unrecognised token");
    }


    /**
     * Bundles all higher calculations into one function call
     * @param expression the expression, passed a string
     * @returns the resullt of the expression, of type `double`
     * @throws `std::invalid_argument` in various cases, all that can be found in the documentation of `Evaluate`, `BuildAST` and `tokenise`
     */
    double Calculate(std::string expression){
        return Evaluate(BuildAST(tokenise(expression)));
    }
}