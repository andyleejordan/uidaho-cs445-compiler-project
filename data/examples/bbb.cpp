#include <iostream>
using namespace std;
void f(int);

int y;

int main()
{
   int x = 6;
   y = 6;
   f(x);
   cout << "x should be 7; it is " << x << endl;
}
