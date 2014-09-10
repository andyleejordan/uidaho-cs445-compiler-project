/* crew.h - Copyright 2014 Andrew Schwartzmeyer
 * Header file for Crew class
 */

#include <string>

class Crew {
private:
  // properties
  string name = "PlanetExpress";
  int thirst;			// Fry's thirst level
  int work;			// Leela's unfulfilled work orders
  int horde;			// Bender's horde of treasure
  int hunger;			// Zoidberg's hunger level
  int beauty;			// Amy's prettiness
  int receipts;			// Hermes's backlog of receipts to process
public:
  Crew();
  bool stable;			// Crew's ability to continue co-existing
  void print_menu();		// Print the game menu
  bool check();			// Check crew's stability
  int score();			// Calculate total score
  void drink();			// Fry drinks Slurm
  void deliver();		// Leela pilots deliveries
  void steal();			// Bender steals things
  void eat();			// Zoidberg eats everything
  void primp();			// Amy primps herself
  void account();		// Hermes does accounting
  void increase_thirst();	// Increases Fry's thirst automatically
  void increase_hunger();	// Increases Zoidberg's hunger automatically
  void decrease_beauty();	// Decreses Amy's beauty automatically
  void load(); // Loads a crew / pet
  void save (); // Saves a crew / pet
};
