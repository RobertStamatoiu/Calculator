# Calculator

A small calculator project built with C++ and a simple browser front end. The backend evaluates arithmetic expressions from an HTTP endpoint, and the frontend sends expressions to the server for calculation.

## Features

- Expression parsing and evaluation in C++
- Supports arithmetic operators: `+`, `-`, `*`, `/`, `^`
- Parentheses and unary signs
- Simple HTTP API endpoint for calculator requests
- Browser UI with keypad-style layout

## Project structure

- `src/main.cpp` - HTTP server entry point
- `lib/calculator.hpp` - expression parsing and evaluation logic
- `lib/httplib.h` - vendored HTTP library used by the server
- `index.html` - calculator page
- `src/script.js` - JavaScript client code
- `src/style.css` - styling for the UI

## Building

This project does not use CMake anymore, so there are no build steps to run from this repository root.

Compile the server directly with your C++ compiler if needed, for example:

```powershell
g++ src\main.cpp -std=c++20 -I. -o calculator.exe
```

## Running the app

1. Start the calculator server:

```powershell
.\calculator.exe
```

2. Open `index.html` in a browser.

3. The frontend sends POST requests to:

```text
http://localhost:8080/calculate
```

Example request body:

```text
expr=2+3*4
```

## API

The server exposes a `POST /calculate` endpoint.

Request:

```http
POST /calculate
Content-Type: application/x-www-form-urlencoded

expr=2+3*4
```

Response:

```json
{"result": 14}
```

## Notes

- The project is intended as a lightweight calculator demo.
- The backend uses the `srn::Calculate` function from `lib/calculator.hpp` to parse and evaluate expressions.
- The included HTTP library is vendored under `lib/httplib.h`.
