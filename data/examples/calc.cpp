
/*
 * A simple calculator program, controlled by a menu and
 *  divided into separate functions
 */
#include<iostream>
using namespace std;
//---------- Function Prototypes -----------
void print_menu();
double get_value();
double divide(double, double);
//-------------- Main -------------------
int main()
{
  double operand1,
        operand2,
	answer;
  int choice, valid_choice;
  do {
    print_menu();
    cin >> choice;

    // assume choice is valid
    valid_choice = 1;

    switch (choice) {
    case 0:
      break;
    case 1:
      // addition
      operand1 = get_value();
      operand2 = get_value();
      answer = operand1 + operand2;
      break;
    case 2:
      // division
      operand1 = get_value();
      operand2 = get_value();
      answer = divide(operand1, operand2);
      break;
    default:
      valid_choice = 0;
      // choice is invalid
      cout << "Invalid Choice." << endl;
    }
    if (valid_choice) {
      // if choice is valid, print the answer
      cout << endl << "Answer = " << answer << endl;
    }
  } while (choice != 0);
  // if the user didn't choose 0, loop back to start
  return 0;
}

//-------------- Functions -------------------
double divide(double dividend, double divisor)
{
  if (divisor == 0.)
    return 0;			// avoids divide by zero errors
  else
    return (dividend / divisor);
}

//----------------- get_value function ----------------
double get_value()
{
  double temp_value;
  cout << "Enter a value: ";
  cin >> temp_value;
  return temp_value;
}

//-------------------- print_menu function -------------
void print_menu()
{
  cout << endl;
  cout << "Add (1)" << endl;
  cout << "Divide (2)" << endl;
  cout << "Exit (0)" << endl;
  cout << "Enter your choice: ";
}
