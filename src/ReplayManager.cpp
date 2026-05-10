#include "ReplayManager.h"
#include <fstream>

void Replay::SaveFrame(glm::vec2 position, int playerSpriteIndex) {
    ReplayFrame frame;
    frame.x = position.x;
    frame.y = position.y;
    frame.spriteIndex = playerSpriteIndex;
    recordingArr.push_back(frame);
}

void Replay::SaveReplay() {
    std::ofstream file("replay.bin", std::ios::binary | std::ios::trunc);
    if (file.is_open() && !recordingArr.empty()) {
        file.write((char*)recordingArr.data(), recordingArr.size() * sizeof(ReplayFrame));
        file.close();
    }
}

void Replay::LoadReplay()
{
    replayArr.clear();
    currentFrame = 0;
    std::ifstream file("replay.bin", std::ios::binary);
    if (file.is_open()) {
        ReplayFrame temp;
        while (file.read((char*)&temp, sizeof(ReplayFrame))) {
            replayArr.push_back(temp);
        }
        file.close();
    }
}

void Replay::ClearRecording() {
    recordingArr.clear();
    currentFrame = 0;
}