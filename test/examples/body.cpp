#include "body.h"

body::body()
{
  x = y = z = 0.0;
  vx = vy = vz = 0.0;
  ax = ay = az = 0.0;
}

void body::update()
{
	x += vx;
	y += vy;
	z += vz;
	vx += ax;
	vy += ay;
	vz += az;
}
