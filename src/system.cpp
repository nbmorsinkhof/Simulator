#include <iostream>
#include <chrono>
#include <thread>
#include "system.hpp"

//constructor
System::System(SystemObjects* s) : SystemObjects_(s){
    set_state(IDLE);
    init();
}

//methods
void System::init(){
    SystemObjects_->link_subsystems();
}

void System::run(){
    while (true){
        switch(state_){
            case IDLE:{
                SystemObjects_->system_interface.state = IDLE;

                set_state(RUN);
            }
            case RUN:{
                    SystemObjects_->system_interface.state = RUN;
                    SystemObjects_->simulator.update_state();
                    if(SystemObjects_->system_interface.bStop){
                        SystemObjects_->system_interface.bStop = false;
                        set_state(STOP);
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    break;
                }
                
            case STOP:{
                    SystemObjects_->system_interface.state = STOP;
                    if(SystemObjects_->system_interface.bStart){
                        SystemObjects_->system_interface.bStart = false;
                        set_state(RUN);
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    break;
                }
                
            case END:
                SystemObjects_->system_interface.state;
                return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    
}
    //accesors
void System::set_state(SystemState s){
    state_ = s;
}

    //mutators
const SystemState System::get_state(){
    return state_;
}