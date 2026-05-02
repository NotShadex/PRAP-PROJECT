#pragma once

class ScoreManager 
{
private:
    static inline int score = 0; // did "inline" so there is no need to create a .cpp file
    static inline int bestScore = 0;
public:
    static void Add(int amount) { 
        score += amount;  
        if (bestScore < score) bestScore = score;
    }
    static int Get() { return score; }
    static int GetBest() { return bestScore; }
    static int SetBest(int amount) { bestScore = amount; }
    static void Reset() { score = 0; }
};