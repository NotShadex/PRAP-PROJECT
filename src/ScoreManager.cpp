#include "ScoreManager.h"
#include <fstream>
#include <algorithm>
#include <vector>
#include <cstring>


void ScoreManager::SaveFinalScore(const std::string& playerName) {
    std::vector<ScoreEntry> entries = LoadLeaderboard();

    // make a new input field
    ScoreEntry newEntry;
    strncpy(newEntry.name, playerName.c_str(), 19);
    newEntry.name[19] = '\0';
    newEntry.score = bestScore;
    entries.push_back(newEntry);

    std::sort(entries.begin(), entries.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
    });

    if (entries.size() > 5) entries.resize(5); // only check top 5

    std::ofstream file("leaderboard.bin", std::ios::binary | std::ios::trunc); // write back to binary file
    if (file.is_open()) {
        for (const auto& e : entries) {
            file.write((char*)&e, sizeof(ScoreEntry));
        }
        file.close();
    }
}

std::vector<ScoreEntry> ScoreManager::LoadLeaderboard() {
    std::vector<ScoreEntry> entries;
    std::ifstream file("leaderboard.bin", std::ios::binary);
    
    if (file.is_open()) {
        ScoreEntry temp;
        while (file.read((char*)&temp, sizeof(ScoreEntry))) {
            entries.push_back(temp);
        }
        file.close();
    }
    return entries;
}