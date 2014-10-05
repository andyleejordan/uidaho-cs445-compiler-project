/* 
 * File:   pet.cpp
 * Author: Terence
 * 
 * Created on October 14, 2011, 11:46 AM
 */

#include "pet.h"
#include <iostream>
#include <fstream>
using namespace std;


void pet::print(int choice){
    switch(choice){
        case 1:
            cout << "Hi, I'm a " << type_of_pet <<"." << endl;
            cout << "My name is "  << name << "."  << endl;
            break;
        case 2:
            if(hunger > 60){  // high values mean hungry
                cout << "I'm hungry." << endl;
            }
            if(hunger > 80){ // 
                cout << "I'm starving."  << endl;
            }
            if(happy < 40){
                    cout << "I'm sad."  << endl;
            }
            break;
    }
}

void pet::set_name(string n){
    name = n;
}

void pet::save(){
    ofstream outfile;
    string filename;
    // create a filename from the name and type of pet
    // creates a unique filename for 'each' pet
    filename = name+type_of_pet+".pet";
    outfile.open(filename.c_str());
    outfile << name << " ";
    outfile << type_of_pet << " ";
    outfile << health << " ";
    outfile << happy  << " ";
    outfile << hunger << " ";
    outfile.close();
}

void pet::load(){
    ifstream inputfile;
    string filename;
    cout << "Please enter the filename.";
    cout << "(Format is NameType.pet)";
    cin >> filename;
    inputfile.open(filename.c_str());
    inputfile >> name;
    inputfile >> type_of_pet;
    inputfile >> health;
    inputfile >> happy;
    inputfile >> hunger;
    inputfile.close();
}

void pet::play_fetch(){
    health = health + 5;
    happy = happy + 5;
    hunger = hunger + 20;
    cout << "Wheee!!!" << endl;
}

void pet::feed(){  // void feed();
    cout << "Yum, yum!" << endl;
    hunger = hunger - 10;
}

void pet::set_type(string t){
    type_of_pet = t;
}

int pet::get_health(){
    return health;
}

pet::pet() {
    type_of_pet = "Dog";
    hunger = 20;
    happy = 50;
    health = 50;
    name = "John Doe";
}

pet::pet(pet& orig) {
}

pet::~pet() {
}

