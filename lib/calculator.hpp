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

    
}