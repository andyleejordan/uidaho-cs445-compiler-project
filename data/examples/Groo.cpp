#include <iostream>
using namespace std;
#include "Groo.h"

Groo::Groo()
{
   myval = 42;
}
int Groo::getmyval()
{
   return myval;
}
void Groo::dostuff()
{
   cout << "yup I did stuff" << endl;
}
