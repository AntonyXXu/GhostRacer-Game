# GhostRacer

UCLA project applying OOP on a game

### TLDR for project specifications:

The course provided all files except Actor.h, Actor.cpp, StudentWorld.h, StudentWorld.cpp.

#### Primary Requirements:

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
