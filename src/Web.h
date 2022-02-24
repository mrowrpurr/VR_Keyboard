#pragma once

#include "Web/controllers/HelloController.hpp"
#include "Web/AppComponent.hpp"

#include "oatpp/network/Server.hpp"

#include <iostream>
#include <stdint.h>

namespace Web {
    void routes(std::shared_ptr<oatpp::web::server::HttpRouter>& router) {
        router->addController(std::make_shared<HelloController>());
    }

    void run() {
        oatpp::base::Environment::init();
        AppComponent components;
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
        routes(router);
        OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);
        OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);
        oatpp::network::Server server(connectionProvider, connectionHandler);
        logger::info("MyFirstSkse: Begin web server on port 8000");
        server.run();
        logger::info("MyFirstSkse: End web server run");
        oatpp::base::Environment::destroy();
    }
}

// OATPP_LOGI("My First SKSE web app", "Server running on port %s", connectionProvider->getProperty("port").getData());
