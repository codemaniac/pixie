#pragma once

#include "engine.h"
#include "pch.h"

namespace tejas {

    namespace uci {

        class UCIHandler {
           private:
            Engine engine;

           private:
            void handleUCI();
            void handleIsReady();
            void handleUCINewgame();
            void handleDisplay();
            void handlePosition(std::string& input);

           public:
            UCIHandler(const Engine& engine);
            void start();
        };

    }

}
