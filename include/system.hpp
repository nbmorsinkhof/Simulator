#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <stdbool.h>

#include "graphics.hpp"
#include "simulator.hpp"
#include "state_space.hpp"

class StateSpacePendulum;

enum SystemState{
    IDLE,
    RUN,
    STOP,
    END
};

struct SystemInterface{
    bool bStart = false;
    bool bStop = false;
    SystemState state = IDLE;
};

struct SystemObjects{
    SystemInterface& system_interface;
    Simulator& simulator;
    StateSpacePendulum& state_space;

    void link_subsystems(){
        simulator.set_system(this);
        state_space.set_system(this);
    }
};

class System{
public:
    //constuctor
    explicit System(SystemObjects*);

    //methods
    void init();
    void run();

    void set_state(SystemState s);

    const SystemState get_state();

private:
    SystemState state_;
    
    SystemObjects* SystemObjects_;
};

#endif