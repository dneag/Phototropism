
This program is a plug-in for Autodesk Maya.  It is designed to test a simulation of plants' decision to grow towards light, or, more accurately in this case, 
away from shade.  It creates a 3D cubic grid, with each unit storing a density level.  Block points can then be added to the grid, increasing the density of the 
units they fall in.  Plants within the grid can scan a neaby area and find any units with density greater than 0.  If a unit has any density, subtract the vector
between them from another vector.  The total difference vector after checking all neaby units will represent the direction of light influence.  The program can 
display the grid, block points, and two vectors which represent the current direction of the plant, and the direction of light influence.
