#include <iostream>

using namespace std;
int
main ()
{
   char a[5][9] = { "every", "good", "bear", "deserves", "fudge" };
   char *p = &(a[2][1]);
   cout << "a: " << p << endl;
// semantic error: can't assign an array-at-a-time
   char b[5][9];
//   b[0] = "every"; b[1] = "good"; b[2] = "bear";
//   b[3] = "deserves"; b[4] = "fudge";
// warning, non-standard but works
   b[0] = { 'e','v','e','r','y' };
   cout << "b: " << b[0] << endl;
   char b0[5][9] = {'e','v','e','r','y',
                    'g','o','o','d',
		    'b','e','a','r',
		    'd','e','s','e','r','v','e','s',
		    'f','u','d','g','e'};
   p = &(b0[2][1]);
   cout << "b0: " << p << endl;
   char b1[5][9] = { {'e','v','e','r','y'},
                    {'g','o','o','d'},
		    {'b','e','a','r'},
		    {'d','e','s','e','r','v','e','s'},
		    {'f','u','d','g','e'}};
   p = &(b1[2][1]);
   cout << "b1: " << p << endl;
   char b2[5][9] = {{'e','v','e','r','y','\0'},
                    {'g','o','o','d','\0'},
		    {'b','e','a','r','\0'},
		    {'d','e','s','e','r','v','e','s','\0'},
		    {'f','u','d','g','e','\0'}};
   p = &(b2[2][1]);
   cout << "b2: " << p << endl;
}
