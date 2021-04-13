# GhostRacer

Project applying OOP on a game. Project specifications attached, based on UCLA CS-32 Project (https://web.archive.org/web/20210210163124/http://web.cs.ucla.edu/classes/winter21/cs32/)

- I created the Actors.h, Actors.cpp, StudentWorld.h, StudentWorld.cpp files to manage objects within the game. 
- All objects are from a base Actors class, with a virtual/abstract function to doSomething().
- All objects are initiated and destroyed within the StudentWorld class, which manages all objects within the class.
- Reduces iteration within the ZombieCab actor initialization and actions with a private member to hold coordinate data. This goes outside of project specifications, but reduces iterations by O(C), where C is the number of cabs on the screen.
- Main red car (Ghost Racer) can change its speed, shoot holy water, and collect blue souls to win the game. Holy water can destroy zombie cabs, zombie pedestrians. Hitting a human pedestrian ends the game.

Build must retarget solution to SDK Version 10.0.17763.0

### Demo
![ghostracer_demo](https://user-images.githubusercontent.com/77988513/114477759-04b31680-9bba-11eb-897b-a882571c4248.gif)

### TLDR for project specifications:

The course provided all files except Actor.h, Actor.cpp, StudentWorld.h, StudentWorld.cpp, and all objects and classes can be found in those four files.

- Create a class StudentWorld to keep track of all objects in the game (GhostRacer, obstacles, pedestrians, projectiles, borders, etc...).

  - StudentWorld essentially orchestrates all gameplay, including initialization, managing objects during each tick/frame, destroying objects during gameplay, and destroying gameplay on new levels/game over.
  - init() initializes the world on game start
  - move() is called at 20fps, and manages the movement of all objects. It also introduces new objects and delete removed objects
  - cleanUp() is called when losing a life or progressing to the new level

- Create actor (object) classes for all objects in the game (GhostRacer, obstacles, pedestrians, projectiles, borders, etc...)
  - Actors are individual objects derived from GraphObject class (provided for graphical representation on the screen)
  - All actors have their own methods for construction, destruction, movement, and interface with other actors, including:
    - Collision with other actors
    - Update scores, health, etc...
