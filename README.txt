Dayana Gonzalez Cruz & Yulissa Valencia
CST-310: Graphics
Project 8: Simple Demo Scene
Nov. 17th, 2024
TR1100A Prof. Citro

Running the Project in Linux Terminal

1. Ensure you have a Linux terminal instance open (e.g., WSL, Ubuntu).
   
2. Verify the Project File is in the Current Directory  
   - Use the `ls` command to list files in the current directory:
     ```bash
     ls
     ```
   - Check that your project file (`domino_simulation.cpp`) is listed in the output. If it’s not, navigate to the correct directory or move the file into the current directory.

3. Compile the Domino Simulation

   - Use the following command to compile the project:
     ```bash
     g++ domino_simulation.cpp -o domino_simulation -lGL -lGLEW -lglfw -lglut -lGLU
     ```

4. Run the Simulation

   - Execute the compiled file to launch the domino simulation:
     ```bash
     ./domino_simulation
     ```

   - A window should pop up displaying the domino simulation.

5. Interact with the Simulation

   - Allow the dominos to finish arranging into a circle and stop bouncing.
   - Use a **left click** on your mouse/trackpad to start the falling animation of the dominos.

6. Exit the Simulation

   - Use `Ctrl + C` in the terminal to close the program if needed.
