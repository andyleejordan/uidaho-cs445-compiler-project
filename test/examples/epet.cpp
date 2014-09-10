#include <iostream>
#include <string>
using namespace std;

// declaration of the pet class
class pet {
  private:
     int hunger;	// private data member
     int happy;		// private data member
     string name;	// private data member
  public:
      pet(); // constructor
      void play();	// public member function
      void feed();	// public member function
      void print();	// public member function
      int check_health();// public member function
 };


 int main()
 {
    pet pet1;
    int choice ;
    int health_check;
    do {
       pet1.print();
       cout << "What would you like to do with your pet?\n";
       cout << " Play (1) \n Feed (2) \n Exit (0) \n";
       cin >> choice;
       switch (choice) {
       case 1:
          pet1.play();
          break;
       case 2:
          pet1.feed();
          break;
          }
       health_check = pet1.check_health();
    } while (choice != 0 && health_check != 1);
    cin.ignore();
    cout << "Press enter to exit." << endl;
    cin.ignore();
    return 0;
  }


/* Constructor, creates a new pet with starting values. */
pet::pet(){
     hunger = 50;
     happy = 50;
     cout << "Pet's name? (One word) ";
     cin >> name;
}
/* Member function play(), allows playing with a pet. */
void pet::play()
{
   int choice;
   cout << "What should we play?\n";
   cout << " Fetch (1) \n Roll over (2) \n";
   cin >> choice;
   switch(choice){
   case 1:
        happy += 10;
        hunger += 5;
        break;
   case 2:
        happy += 5;
        hunger += 1;
        break;
   default:
        cout << "Not a valid choice." << endl;
   }
}
/* Member function feed(), allows the user to feed a pet. */
void pet::feed(){
   cout << "\nMMM, Yummy!\n";
   hunger -= 5;
}
/* Member function print(), prints information about a pet. */
void pet::print(){
   cout << "\nYour pet " << name << " is " << endl;
   cout << "Happy: " << happy << endl;
   cout << "Hungry: " << hunger << endl;
}
/* Member function check_health(), checks the health of a pet. */
int pet::check_health(){
   if (hunger >= 100) {
      cout << "\nYour pet has starved.\n";
      return 1;
   }
   if(happy <= 0){
      cout << "\nYour pet has died of a broken heart.\n";
      return 1;
   }
   return 0;
}
