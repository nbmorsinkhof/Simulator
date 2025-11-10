#include<iostream>
#include<chrono>
#include<thread>
#include<vector>
#include<functional>

#include "state_space.hpp"
#include "simulator.hpp"
#include "graphics.hpp"
#include "system.hpp"

void sim_thread_function(System& system){
    system.run();
}

int main(int argc, char* argv[]){
    std::cout<<"Hello Start simulator"<<std::endl;

    SystemInterface system_interface;
    // Create sate space system of pendulum
    StateSpacePendulum pendulum;
    //Set state of pendulum
    pendulum.set_state(0.6, 0);

    StateSpaceSegway segway;
    segway.set_state(3.14/8, 2);

    // Create simulator and pass pendulum
    Simulator simulator;
    // Create Graphics instance
    Graphics graphics(argc, argv);

    //Create instance of top level system
    SystemObjects system_objects{
       system_interface,
       simulator,
       segway
    };

    System system(&system_objects);

    // Create simulation Thread
    std::thread sim_thread(sim_thread_function, std::ref(system));
  
    // Run graphics and sim_thread
    graphics.set_system(&system_objects);
    graphics.run();
    sim_thread.join();


    return 0;
}