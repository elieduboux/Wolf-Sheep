#include "Project_SDL1.h"
#include <stdio.h>
#include <string>
#include <windows.h>
int main(int argc, char* argv[]) {


    std::cout << "Starting up the application" << std::endl;

    if (argc != 4)
    throw std::runtime_error("Need three arguments - "
                                "number of sheep, number of wolves, "
                                "simulation time\n");

    //Initialize SDL , Initialize PNG loading
    init(); 

    std::cout << "Done with initilization" << std::endl;

    auto my_app = application(std::stoul(argv[1]), std::stoul(argv[2]));

    std::cout << "Created window" << std::endl;

    //Debut de la loop
    int retval = my_app.loop(std::stoul(argv[3]));

    std::cout << "Exiting application with code " << retval << std::endl;

    //Nettoyez tous les sous-syst�mes initialis�s.
    SDL_Quit();
    SDL_Delay(4000);
    return retval;
}