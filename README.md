
# ConsoleGameofLife

This program runs the John Conway's game of life in your console.  

## Features

- File treatment: Allow you to load a text file with specific syntax to init the
simulation.  
- Speed control: You can change the rate (time intervall between two screen refreshs)
from 10 to 200 ms.  
- Stability detection: This is an option that when set **On**, stops the game when the world  
becomes stable; that means, when all shapes are either immuable or periodically stable  
(they constantly change, but they fall in the same state each period of time, like an ideal spring oscillation).  
This feature detects oscillations of period 5 and lower.  
- Glider gun: Allow you to start the game with a glider gun in the bottom-left corner.  

## Build/Setup

Download the repository and put its content in a folder named *ConsoleGameofLife*
or something similar, then run "make" from this location.  
The executable, *cgol.exe*, is generated in the ./bin subfolder.  
You could consider adding its path to your PATH variable to run it from anywhere.  
Although this program is not relly useful, I wanted it to share the spirit of the GNU coreutils.
