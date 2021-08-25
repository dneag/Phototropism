
This program is a plug-in for Autodesk Maya.  It is designed to test a simulation of plants' decision to grow towards light, or, more accurately in this case, 
away from shade.  It creates a 3D cubic grid, with each unit storing a density level.  Block points can then be added to the grid, increasing the density of the 
units they fall in.  Plants within the grid can scan a neaby area and find any units with density greater than 0.  If a unit has any density, subtract the vector
between them from a vector that represents light influence.  The total difference will be this vector.  The program can display the grid, block points, and two vectors
which represent the current direction of the plant, and the direction of light influence.
