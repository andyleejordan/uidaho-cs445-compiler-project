/* 
 * File:   pet.h
 * Author: Terence
 *
 * Created on October 14, 2011, 11:46 AM
 */
#include<string>
using namespace std;

class pet {
public:
    pet();
    pet(pet& orig);
    ~pet();
    void print(int);
    void set_type(string);
    void set_name(string);
    void feed();
    void play_fetch();
    int get_health();
    void save();
    void load();

private:
    string type_of_pet;
    int hunger;
    int happy;
    int health;
    string name;
};
