#pragma once

namespace Events {
    void OnInit(SKSE::MessagingInterface::Message* msg) {
        switch (msg->type) {
            case SKSE::MessagingInterface::kPostLoad: {
                logger::info("MyFirstSkse: Post Load!");
                break;
            }
            case SKSE::MessagingInterface::kDataLoaded: {
                logger::info("MyFirstSkse: Data Loaded!");
                break;
            }
        }
    }
}
