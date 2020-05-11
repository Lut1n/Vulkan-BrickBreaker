#include "common.h"
#include "app/application.h"

//--------------------------------------------------------------
int main()
{
    MiniVkApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}









