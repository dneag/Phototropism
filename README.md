
This program is designed to test a simulation of plants' decision to grow towards light, or, more accurately in this case, away from shade.  It creates a 3D cubic
grid, with each unit storing a density level.  Block points can then be added to the grid, increasing the density of the units they fall in.  Plants within the grid 
can scan a neaby area and find any units with density greater than 0.  If a unit has any density, subtract the vector to that unit from the vector that represents 
the plant's chosen direction.
