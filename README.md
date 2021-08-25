
This program is a plug-in for Autodesk Maya.  It is designed to test a simulation of plants' decision to grow towards light, or, more accurately in this case, 
away from shade.  It creates a 3D cubic grid, with each unit storing a density level.  Block points can then be added to the grid, increasing the density of the 
units they fall in.  Plants within the grid can scan a neaby area and find any units with density greater than 0.  If a unit has any density, subtract the vector
between them from another vector.  The total difference vector after checking all neaby units will represent the direction of light influence.  The program can 
display the grid, block points, and two vectors which represent the current direction of the plant, and the direction of light influence.

To run...

  - have Autodesk Maya 2018 installed (other versions may well work, but I cannot gaurantee).

  - download the file Phototropism.mll

  - place the file in the correct directory...for Windows users, I can summarize:

  - the .mll file can be placed anywhere - then with Maya open, from the top bar go to Windows -> Settings/Preferences -> Plug-in Manager
  - within the plug-in manager window, click the browse button on the bottom left, navigate to the .mll file, then make sure the 'loaded' checkbox is checked for it
  - (a similar process should work for other operating systems. If it doesn't, see the link below)
  - open Maya's script editor: Windows -> General Editors -> Script Editor. Then, on the bottom half of the window, make sure you have a MEL tab open

  - enter the following line of code:

    testGrow
  
  - execute it by clicking the 'Execute' button along the top of the script editor

This will execute the command registered by the plug-in
