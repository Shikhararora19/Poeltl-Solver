#include <thread>
#include <mutex>
#include <condition_variable>
#include <utility>  // If not already present
#include <iostream>
#include <fstream>
#include "json.hpp"  // Use the json.hpp from nlohmann
#include "httplib.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <limits> // For std::numeric_limits

using json = nlohmann::json;

// Structure to store NBA player data
struct Player {
    std::string firstName;
    std::string lastName;
    std::string team;
    std::string position;
    std::string conference;
    std::string division;
    int height;
    int age;
    int jerseyNumber;
};

// Define the team information map
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> team_info = {
    {"ATL", {{"Conference", "Eastern"}, {"Division", "Southeast"}}},
    {"BOS", {{"Conference", "Eastern"}, {"Division", "Atlantic"}}},
    {"BKN", {{"Conference", "Eastern"}, {"Division", "Atlantic"}}},
    {"CHA", {{"Conference", "Eastern"}, {"Division", "Southeast"}}},
    {"CHI", {{"Conference", "Eastern"}, {"Division", "Central"}}},
    {"CLE", {{"Conference", "Eastern"}, {"Division", "Central"}}},
    {"DAL", {{"Conference", "Western"}, {"Division", "Southwest"}}},
    {"DEN", {{"Conference", "Western"}, {"Division", "Northwest"}}},
    {"DET", {{"Conference", "Eastern"}, {"Division", "Central"}}},
    {"GSW", {{"Conference", "Western"}, {"Division", "Pacific"}}},
    {"HOU", {{"Conference", "Western"}, {"Division", "Southwest"}}},
    {"IND", {{"Conference", "Eastern"}, {"Division", "Central"}}},
    {"LAC", {{"Conference", "Western"}, {"Division", "Pacific"}}},
    {"LAL", {{"Conference", "Western"}, {"Division", "Pacific"}}},
    {"MEM", {{"Conference", "Western"}, {"Division", "Southwest"}}},
    {"MIA", {{"Conference", "Eastern"}, {"Division", "Southeast"}}},
    {"MIL", {{"Conference", "Eastern"}, {"Division", "Central"}}},
    {"MIN", {{"Conference", "Western"}, {"Division", "Northwest"}}},
    {"NOP", {{"Conference", "Western"}, {"Division", "Southwest"}}},
    {"NYK", {{"Conference", "Eastern"}, {"Division", "Atlantic"}}},
    {"OKC", {{"Conference", "Western"}, {"Division", "Northwest"}}},
    {"ORL", {{"Conference", "Eastern"}, {"Division", "Southeast"}}},
    {"PHI", {{"Conference", "Eastern"}, {"Division", "Atlantic"}}},
    {"PHO", {{"Conference", "Western"}, {"Division", "Pacific"}}},
    {"POR", {{"Conference", "Western"}, {"Division", "Northwest"}}},
    {"SAC", {{"Conference", "Western"}, {"Division", "Pacific"}}},
    {"SAS", {{"Conference", "Western"}, {"Division", "Southwest"}}},
    {"TOR", {{"Conference", "Eastern"}, {"Division", "Atlantic"}}},
    {"UTA", {{"Conference", "Western"}, {"Division", "Northwest"}}},
    {"WAS", {{"Conference", "Eastern"}, {"Division", "Southeast"}}}
};


// Function to fetch player data from SportsData.io API
std::string fetchPlayerData() {
    httplib::Client client("api.sportsdata.io");

    std::string endpoint = "/v3/nba/scores/json/PlayersActiveBasic?key=25a2492d13894327a60e4b8acd6ffb90";
    auto res = client.Get(endpoint.c_str());

    if (res && res->status == 200) {
        return res->body;
    } else {
        std::cerr << "Failed to fetch player data. Status code: " << res->status << std::endl;
        return "";
    }
}

// Parse the JSON and load the players into a vector of Player objects
std::vector<Player> parsePlayerData(const std::string& jsonData) {
    std::vector<Player> players;
    json j = json::parse(jsonData);

    for (const auto& item : j) {
        Player player;
        player.firstName = item["FirstName"];
        player.lastName = item["LastName"];
        player.team = item["Team"];
        player.position = item["Position"];
        if (!item["Height"].is_null()) {
        int heightInInches = item["Height"];
        player.height = heightInInches * 2.54;
        } else {
            player.height = 0; // Default value if height is null
        }

        // Check if "BirthDate" is null or contains a valid date
        if (!item["BirthDate"].is_null() && item["BirthDate"].is_string()) {
            // Calculate age based on birth year (assuming BirthDate is in the format YYYY-MM-DD)
            player.age = 2024 - std::stoi(item["BirthDate"].get<std::string>().substr(0, 4));
        } else {
            player.age = 0; // Default value if birth date is null
        }

        // Check if "Jersey" is null or a valid number
        if (!item["Jersey"].is_null()) {
            player.jerseyNumber = item["Jersey"];
        } else {
            player.jerseyNumber = 0; // Default value if jersey number is null
        }
        // Get the conference and division from team_info based on the team
        if (team_info.find(player.team) != team_info.end()) {
            player.conference = team_info[player.team]["Conference"];
            player.division = team_info[player.team]["Division"];
        }

        players.push_back(player);
    }

    return players;
}

// Function to filter players based on the game feedback
std::vector<Player> filterPlayers(
    const std::vector<Player>& playerList, 
    const Player& guess, 
    const std::string& teamFeedback, 
    const std::string& heightFeedback, 
    const std::string& positionFeedback, 
    const std::string& ageFeedback, 
    const std::string& numberFeedback,
    const std::string& conferenceFeedback,
    const std::string& divisionFeedback) 
{
    std::vector<Player> filteredList;

    for (const Player& player : playerList) {
        bool isValid = true;

        // Filter by team feedback
        if (teamFeedback == "correct" && player.team != guess.team) {
            isValid = false;
        } else if (teamFeedback == "incorrect" && player.team == guess.team) {
            isValid = false;
        }

        // Filter by height feedback
        if (heightFeedback == "correct" && player.height != guess.height) {
            isValid = false;
        } else if (heightFeedback == "higher" && player.height <= guess.height) {
            isValid = false;
        } else if (heightFeedback == "lower" && player.height >= guess.height) {
            isValid = false;
        }

        // Filter by position feedback
        if (positionFeedback == "correct" && player.position != guess.position) {
            isValid = false;
        } else if (positionFeedback == "incorrect" && player.position == guess.position) {
            isValid = false;
        }

        // Filter by age feedback
        if (ageFeedback == "correct" && player.age != guess.age) {
            isValid = false;
        } else if (ageFeedback == "higher" && player.age <= guess.age) {
            isValid = false;
        } else if (ageFeedback == "lower" && player.age >= guess.age) {
            isValid = false;
        }

        // Filter by number feedback, skip if jersey number is zero
        if (player.jerseyNumber != 0) {
            if (numberFeedback == "correct" && player.jerseyNumber != guess.jerseyNumber) {
                isValid = false;
            } else if (numberFeedback == "higher" && player.jerseyNumber <= guess.jerseyNumber) {
                isValid = false;
            } else if (numberFeedback == "lower" && player.jerseyNumber >= guess.jerseyNumber) {
                isValid = false;
            }
        }


        // If the player passes all checks, add to filtered list
        if (isValid) {
            filteredList.push_back(player);
        }
    }

    return filteredList;
}


int main() {
    std::string apiKey = "25a2492d13894327a60e4b8acd6ffb90";  // Replace with your SportsData.io API key

    // Fetch player data from the API
    std::string playerData = fetchPlayerData();

    if (!playerData.empty()) {
        // Parse the player data
        std::vector<Player> players = parsePlayerData(playerData);
        std::cout << "Total players parsed: " << players.size() << std::endl;

        std::vector<Player> filteredPlayers = players;

        while (!filteredPlayers.empty()) {
            std::string playerGuessName;
            std::cout << "Enter your guess (NBA Player's Name): ";
            std::getline(std::cin, playerGuessName);

            // Find the player from the database based on the guess
            Player playerGuess;
            bool playerFound = false;
            for (const Player& player : players) {
                if (player.firstName + " " + player.lastName == playerGuessName) {
                    playerGuess = player;
                    std::cout << "- " << player.firstName << " " << player.lastName 
                            << ", Team: " << player.team 
                            << ", Conference: " << player.conference
                            << ", Division: " << player.division
                            << ", Position: " << player.position 
                            << ", Height: " << player.height 
                            << ", Age: " << player.age 
                            << ", Jersey Number: " << player.jerseyNumber << std::endl;
                    playerFound = true;
                    break;
                }
            }

            if (!playerFound) {
                std::cout << "Player not found in the database!" << std::endl;
                continue;
            }

            // Ask for feedback on each attribute
            std::string teamFeedback, heightFeedback, positionFeedback, ageFeedback, numberFeedback, conferenceFeedback, divisionFeedback;
            std::cout << "Team feedback (correct/incorrect): ";
            std::cin >> teamFeedback;
            std::cout << "Height feedback (correct/higher/lower): ";
            std::cin >> heightFeedback;
            std::cout << "Position feedback (correct/incorrect): ";
            std::cin >> positionFeedback;
            std::cout << "Age feedback (correct/higher/lower): ";
            std::cin >> ageFeedback;
            std::cout << "Number feedback (correct/higher/lower): ";
            std::cin >> numberFeedback;
            std::cout << "Conference feedback (correct/incorrect): ";
            std::cin >> conferenceFeedback;
            std::cout << "Division feedback (correct/incorrect): ";
            std::cin >> divisionFeedback;

            std::cout << "Filtering by player: " << playerGuess.firstName << " " << playerGuess.lastName << std::endl;
            std::cout << "Current filters - Team: " << teamFeedback << ", Conference: " << conferenceFeedback << ", Division: " << divisionFeedback << std::endl;

            // Filter the player list based on the feedback
            filteredPlayers = filterPlayers(filteredPlayers, playerGuess, teamFeedback, heightFeedback, positionFeedback, ageFeedback, numberFeedback, conferenceFeedback, divisionFeedback);

            // Check if there's only one player left
            if (filteredPlayers.size() == 1) {
                std::cout << "The player is: " << filteredPlayers[0].firstName << " " << filteredPlayers[0].lastName << std::endl;
                break;
            }
            else{
                std::cout << "There are " << filteredPlayers.size() << " possible players left:" << std::endl;

    // List the remaining possible players
                for (const Player& player : filteredPlayers) {
                    std::cout << "- " << player.firstName << " " << player.lastName 
                            << ", Team: " << player.team 
                            << ", Conference: " << player.conference
                            << ", Division: " << player.division
                            << ", Position: " << player.position 
                            << ", Height: " << player.height 
                            << ", Age: " << player.age 
                            << ", Jersey Number: " << player.jerseyNumber << std::endl;
                            

                }
            }

            // Clear input buffer
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    return 0;
}
