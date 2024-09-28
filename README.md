# Poeltl-Solver

Poeltl-Solver is a command-line application that helps you solve the popular NBA guessing game "Poeltl." The game involves guessing an NBA player based on a series of clues like height, age, position, team, conference, and division.

This project uses C++ with REST API integration for fetching real-time NBA player data and provides filtered guesses based on user feedback on each attempt.

## Features

- Fetch real-time NBA player data from the [SportsData.io API](https://sportsdata.io).
- Allows the user to filter players based on:
  - Team
  - Position
  - Height (in centimeters)
  - Age
  - Jersey number
  - Conference and Division
- Offers feedback-driven filtering of possible players.

## Technologies Used

- C++ Standard Library
- [nlohmann/json](https://github.com/nlohmann/json) for JSON parsing
- [httplib](https://github.com/yhirose/cpp-httplib) for HTTP client functionality
- SportsData.io API for real-time NBA player data

## Installation

1. Clone the repository:

   ```bash
   -git clone https://github.com/Shikhararora19/Poeltl-Solver.git
   -Install the dependencies listed in the requirements.txt

2. Running the script:
   - Compile the poeltl_solver application g++ -std=c++17 -pthread -o poeltl_solver poetl.cpp
   - Finally give command ./poeltl_solver to run the script


