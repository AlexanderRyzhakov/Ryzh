//============================================================================
// Name        : Transport.cpp
// Author      : Ryzhakov
// Description : 10 sprint
//============================================================================

#include <iostream>
#include <sstream>

#include "json_reader.h"
#include "request_handler.h"

using namespace std;
using namespace transport_catalogue;

int main() {

    router::TransportCatalogue catalogue;
    map_renderer::MapRenderer render(catalogue);
    RequestHandler handler(catalogue, render);

    RunJsonIO(handler, cin);

    return 0;
}

