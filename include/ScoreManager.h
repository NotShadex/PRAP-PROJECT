#pragma once

class ScoreManager {
    private:
        static inline int score = 0; // did "inline" so there is no need to create a .cpp file
    public:
        static void Add(int amount) { score += amount; }
        static int Get() { return score; }
        static void Reset() { score = 0; }
};