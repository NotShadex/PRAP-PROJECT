#pragma once
#include <vector>
#include <glm/glm.hpp>

struct ReplayFrame {
    float x, y;
    int spriteIndex;
};

class Replay {
public:
    void SaveFrame(glm::vec2 position, int playerSpriteIndex);
    void SaveReplay();
    void LoadReplay();
    void ClearRecording();
public:
    std::vector<ReplayFrame> recordingArr;
    std::vector<ReplayFrame> replayArr;
    int currentFrame;
};