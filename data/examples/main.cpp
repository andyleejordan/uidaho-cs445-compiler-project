/* assign5.cpp - Copyright 2014 Andrew Schwartzmeyer
 * main source file for Spring 2014's Assignment 5
 */

#include <iostream>

#include "crew.h"

using namespace std;

int play(Crew PlanetExpress);

int main() {
  // Make the Crew object
  Crew PlanetExpress;
  int score = 0;
  // start the game
  cout << "Welcome to the Game of Futurama!\n\n"
       << "You control the crew of the Planet Express and must keep them\n"
       << "stable enough to continue delivering packages across the universe.\n"
       << "If they can no longer work together, the Professor\'s business\n"
       << "disentegrates and the crew must go their separate ways.\n"
       << "Probably to get new career chips or something.\n\n"
       << "Good luck, and have fun! - Andrew Schwartzmeyer\n\n"
       << "Press enter to continue..." << endl;
  cin.ignore();
  // play the game
  score = play(PlanetExpress);
  // exit game
  cout << "Good game! Your score was: " << score << endl;
  return 0;
}

int play(Crew PlanetExpress) {
  // play game while stable
  while (PlanetExpress.stable) {
    int choice;
    PlanetExpress.print_menu();
    cin >> choice;
    // implement game logic: use switch(choice) to call correct member
    // functions, or set stable = false to exit. You may want to use
    // the default case to validate the input choice and repeatedly
    // ask until it is valid.

    // at the end of each turn, check stability of crew (if not exiting)
    if (PlanetExpress.stable) PlanetExpress.stable = PlanetExpress.check();
  }
  // return the game's score
  return PlanetExpress.score();
}
