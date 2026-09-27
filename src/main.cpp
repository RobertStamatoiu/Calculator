#include <iostream>

#include "..\lib\calculator.hpp"
#include "..\lib\httplib.h"

/**
 * licences to httplib.h, vendored from github, https://github.com/yhirose/cpp-httplib
 */

int main() {
    httplib::Server Server;
    Server.Post("/calculate", [](const httplib::Request& request, httplib::Response &response){
        std::string expression = request.get_param_value("expr");
        double res = srn::Calculate(expression);
        response.set_content("{\"result\": "+ std::to_string(res) + "}", "application/json");
    });
    std::cout << "Server runnign on http://localhost:8080\n";
    Server.listen("localhost", 8080);
}
