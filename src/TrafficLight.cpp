#include <iostream>
#include <random>
#include "TrafficLight.h"

 //below random generator code taken from https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> distrib(4, 6);

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 


    //perform queue modification under lock
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock,[this]{return !_queue.empty();}); //pass unique lock to condition variable

    //remove last element from queue
    T msg = std::move(_queue.back());
    //I was observing that the vehicles were entering the intersection, even when traffic light was red.
    // I found the following comment in the mentor section of udacity https://knowledge.udacity.com/questions/98313
    //_queue.pop_back();
    _queue.clear();

    return msg; //return the message
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> uLock(_mutex);
    //std::cout << "Message " << msg << " has been sent to the queue" << std::endl; 
    _queue.emplace_back(std::move(msg));
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */

 // Constructor
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

 //Destructor
TrafficLight::~TrafficLight()
{

}


void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {
        if(_messages.receive() == TrafficLightPhase::green){
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}


// // virtual function which is executed in a thread
// void TrafficLight::cycleThroughPhases()
// {
//     // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
//     // and toggles the current phase of the traffic light between red and green and sends an update method 
//     // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
//     // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.

//     //below random generator code taken from https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
//     std::random_device rd;  //Will be used to obtain a seed for the random number engine
//     std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
//     std::uniform_int_distribution<> distrib(4, 6);

//     while(true)
//     {
//         for (int i = 0; i<2;++i)
//         {
            
//             std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(distrib(gen)));
//             std::this_thread::sleep_for(std::chrono::milliseconds(1));
            
//         }

//         //Toggle traffic light
//         //std::unique_lock<std::mutex> lck(_mutex);
//        TrafficLightPhase NewPhase = (_currentPhase == TrafficLightPhase::green) ? TrafficLightPhase::red : TrafficLightPhase::green;
//         _currentPhase = NewPhase;
//         // sends an update method to the message queue using move semantics.
//        _messages.send(std::move(NewPhase));
//         //lck.unlock();
        

//     } 
// }

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.

    
    //code from vehicle::Drive taken as example
    // initalize variables
    double cycleDuration = distrib(gen)*1000; // duration of a single simulation cycle in ms. Multiply by 1000 to make it seconds
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // init stop watch
    lastUpdate = std::chrono::system_clock::now();
    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= cycleDuration)
        {
        
        //Toggle traffic light
       _currentPhase = (_currentPhase == TrafficLightPhase::green) ? TrafficLightPhase::red : TrafficLightPhase::green;
        
        // sends an update method to the message queue using move semantics.
       _messages.send(std::move(_currentPhase));
            
            // reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();
        }
    } // eof simulation loop




}

