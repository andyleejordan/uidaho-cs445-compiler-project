/* 
 * File:   ant.cpp
 * Author: Joshua Rubini
 * 
 * Created on October 25, 2011, 10:42 AM
 */

#include "ant.h"

/* ant::ant()  Default constructor
 * Initializes ant to location (0,0), facing north
 */
ant::ant() {
    x = 0;
    y = 0;
    energy = 0;
    dir = '^';
}

// Accessors: return the requested data member

int ant::GetX() {
    return x;
}

int ant::GetY() {
    return y;
}

int ant::GetEnergy() {
    return energy;
}

char ant::Print() {
    return dir;
}

/* ant::ChangeEnergy(int) -- Mutator:
 * @param n: amount to add to energy
 * Adds the specified amount to energy
 * (to subtract energy, pass in a negative value)  
 */
void ant::ChangeEnergy(int n) {
    energy+=n;
}

//------------------- ant Methods ----------------------------

/* Note on coordinate system:
 * This implementation assumes a set of "normal" Euclidean axes. 
 * In particular, moving "north" moves the ant "up" the screen, and
 * increases its y-coordinate by 1, while moving "south" does the
 * opposite.  Similarly, moving "east" moves the ant to the right,
 * and increases its x-coordinate by 1.  Therefore, when printing,
 * the row and column indices would look like this:
 * 
 *        +Y
 * (MAX_Y-1) |   |   |   |   |   |   |
 *           +---+---+---+---+---+---+---
 * (MAX_Y-2) |   |   |   |   |   |   |
 *           +---+---+---+---+---+---+---
 *     ...   |///|///|///|///|///|   |
 *           +---+---+---+---+---+---+---
 *     ...   |///|///|///|///|///|   |
 *           +---+---+---+---+---+---+---
 *         2 |0,2|1,2|2,2|///|///|   |
 *           +---+---+---+---+---+---+---
 *         1 |0,1|1,1|2,1|///|///|   |
 *           +---+---+---+---+---+---+---
 *         0 |0,0|1,0|2,0|///|///|   |
 *        ---+---+---+---+---+---+---+--- 
 *      -x   | 0   1   2  ... ...  *   **  +x
 *        -y
 * 
 *    * = MAX_X-2
 *   ** = MAX_X-1
 * 
 * In order to achieve this using normal streaming output commands
 * (cout << .... << endl; etc.) you will need to count _down_ the y/row
 * values.  Otherwise, when moving in the y direction, your ant will
 * appear to move "backwards".
 * 
 */

/* Note on implementation:
 * All movement methods use a simple switch statement off of the current
 * direction.  While not necessarily the most concise or general solution,
 * it has the advantage of being fairly easy to understand and shows how
 * similar the logic for each method is, so it is the method I chose.
 */


/* ant::Left() -- Method:
 * Changes the direction the ant is facing based upon its current
 * facing and energy.  Ants require 1 energy to turn left.  If
 * insufficient energy is available, direction is unchanged and the
 * method returns false, indicating it failed to turn.  Otherwise,
 * energy is decremented, the direction is updated to the new, correct
 * facing, and a value of true is returned to indicate success.
 */
bool ant::Left() {
    bool ret_val = false;
    if (energy > 0) {
        energy--;
        switch (dir) {
            case '^':
                dir = '<';
                break;
            case '>':
                dir = '^';
                break;
            case 'v':
                dir = '>';
                break;
            case '<':
                dir = 'v';
                break;
                
        }
        ret_val = true;
    }
    return ret_val;
}

/* ant::Right() -- Method:
 * Changes the direction the ant is facing based upon its current
 * facing and energy.  Ants require 1 energy to turn right.  If
 * insufficient energy is available, direction is unchanged and the
 * method returns false, indicating it failed to turn.  Otherwise,
 * energy is decremented, the direction is updated to the new, correct
 * facing, and a value of true is returned to indicate success.
 */
bool ant::Right() {
    bool ret_val = false;
    if (energy > 0) {
        energy--;
        switch (dir) {
            case '^':
                dir = '>';
                break;
            case '>':
                dir = 'v';
                break;
            case 'v':
                dir = '<';
                break;
            case '<':
                dir = '^';
                break;
                
        }
        ret_val = true;
    }
    return ret_val;
}

/* ant::Forward() -- Method:
 * Changes the location of the ant based upon its current
 * facing and energy.  Ants require 2 energy to move forward.  If
 * insufficient energy is available, location is unchanged and the
 * method returns false, indicating it failed to move.  Otherwise,
 * energy is decremented by 2, the location is updated to the new,
 * correct location, and a value of true is returned to indicate success.
 */
bool ant::Forward() {
    bool ret_val = false;
    if (energy > 1) {
        energy -= 2;
        switch (dir) {
            case '^':
                y++;
                break;
            case '>':
                x++;
                break;
            case 'v':
                y--;
                break;
            case '<':
                x--;
                break;
                
        }
        ret_val = true;
    }    
    return ret_val;
}

