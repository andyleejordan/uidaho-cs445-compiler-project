class body {
private:
      double x,y,z;
      double mass,
             vx,vy,vz, /* velocities */
	     ax,ay,az; // accelerations
public:
   /* update the body's position for one time step forward */
   void update();
}
