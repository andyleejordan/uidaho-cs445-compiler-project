#include <iostream>
using namespace std;

int f( float val )
{
   return val < 3.0;
}



int main()
{
     float g;
     cout << "Enter a real number: " ;
     cin >> g;
     if ( f(g) )
       cout << "f is " << f(g) << endl;
}
