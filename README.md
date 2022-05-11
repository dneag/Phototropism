
This program is a plug-in for Autodesk Maya.  It is designed to test a simulation of plants' decision to grow towards light, or, more accurately in this case, 
away from shade.  It creates a 3D cubic grid, with each unit storing a density level.  Block points can then be added to the grid, increasing the density of the 
units they fall in, which in turn affects the direction of the vectors stored in units below.  Plants on the grid can see these vectors and use them to determine
which direction they should grow.

Program demo here:  https://dneag.github.io/phot_demo

To run...

  - have Autodesk Maya 2018 installed (other versions may well work, but I cannot gaurantee).

  - download the file Phototropism.mll

  - place the file in the correct directory...for Windows users, I can summarize:

  - the .mll file can be placed anywhere - then with Maya open, from the top bar go to Windows -> Settings/Preferences -> Plug-in Manager
  - within the plug-in manager window, click the browse button on the bottom left, navigate to the .mll file, then make sure the 'loaded' checkbox is checked for it
  - (a similar process should work for other operating systems. If it doesn't, see the link below)
  - open Maya's script editor: Windows -> General Editors -> Script Editor. Then, on the bottom half of the window, make sure you have a MEL tab open

  - enter the following line of code:

    testBPG
  
  - execute it by clicking the 'Execute' button along the top of the script editor

This will execute the command registered by the plug-in

