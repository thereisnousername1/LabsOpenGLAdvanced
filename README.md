# LabsOpenGLAdvanced
 
Prerequisites to open this project correctly: Visual Studio 2019 or above, GLFW, OpenGL32.lib needed! Please also check linker.txt for more information.

-----------------------------------------------------------------------------------------------------------------------------------------------

Building GLFW with CMake 
1. First, we add a folder called “OpenGL” in C drive’s public user folder (C:\users\public\OpenGL\).
   Inside this folder we create 2 folders “lib” and “include”. 
2. GLFW – we need to create a custom library for our system. 
3. Download GLFW (source package) and unpack the folder. 
4. Download CMake binary and install it on your machine. Run CMake once installed. 
5. We use CMake to generate a project that will build the library for us,
   by running CMake on the root folder of the downloaded GLFW. 
6. Choose the “Where to build the binaries” – Create a new folder called “build” inside the root folder GLFW. 
7. Hit configure in CMake and let it compile.
   Keep all the settings as they are and once finished compiling hit Generate.
   This should generate a visual studio solution (GLFW.sln) in the new build folder. 
8. Open Visual studio project by clicking on the solution of the newly generated project. 
9. We run the generated project and create the library; you can find the glfw3.lib in build/src/Debug 
10. We add the GLFW library that we created to the “lib” folder in Dependencies. 
11. We add the content of the “include” folder (a folder called GLFW) from the downloaded GLFW to the C:/users/public/OpenGL/include.

-----------------------------------------------------------------------------------------------------------------------------------------------

Linking 
Next we need to link the GLFW library with your project. In order to link a library to a project, 
first we need to tell the project where it can find the library and second we link it.

1. First let’s tell the project where the library is. 
2. Right click on the project name(e.g. Lab 0 - Initialization) in the solution explorer and go to VC++ Directories.
   Make sure you select the ”Library Directories”: 

Here, we specify the path for our dependencies (both “lib” and “include”). 
Click on the arrow on the right and a new window will open.

In the highlighted section (the blue bit) add the path to your “Dependency” folder that you can find in C:/users/public/OpenGL/lib. 
After you added the path, you should have something filled to the Library Directories.

Now highlight the “Include Directories” and do the same steps from points 1 & 2 under Linking, for the “include” folder. 
 
Now that we have told the project where the library is, we need to link the two. 
If the project properties are not open, please open it. Please select Linker/Input/Additional Dependencies from the left tab menu

Just like before, add the GLFW library (make sure you get the name right) “glfw3.lib”. 
And on a new line add the OpenGl library that already exists on your machine “opengl32.lib”. 
Now you should have something filled to the Linker/Input/Additional Dependencies space.

Hit Ok and close the project properties window. 
Last thing is to test if the path is correct for our library and if we can use it. 
Add the following code at the top of your main.cpp file. Now build the solution. 
If you’ve done it right, you shouldn’t have any errors. 
If you still have errors, check all the dependencies. 
Do the next steps (adding GLAD), GLFW needs Glad to run before it. 

-----------------------------------------------------------------------------------------------------------------------------------------------

GLAD 
Next step is to add GLAD to our project. Generate GLAD dependencies for our system 
1. Open the web browser tool and generate the dependencies. Make sure you pick C++ 
and OpenGL 4.6 and make sure the profile is set to “Core”. Tick the option “Generate 
a loader” 
2. Unpack the folder and copy the content of the GLAD “include” folder and paste it into 
C:/users/public/OpenGL/include. (The content of GLAD “include” should have 2 
folders “glad” and “KHR”). 
3. Add the “glad.c” file to your project. Copy the file in your project (put it in the same 
folder where your main.cpp file is). As you can see, even though you copied the file 
in the project folder it doesn’t show in the project. You need to add it to the project, 
right click on the “Source Files” and add an existing item. Pick the glad.c file in the 
project folder and add it to the Project. 
Now you should see the file next to your main.cpp file. 
Let’s test the project so far. Add the following line at the top of the main.cpp file. Make sure 
you have the correct order. Glad needs to be before GLFW: 
If you’ve done everything ok. You shouldn’t get any errors when building the project. 
