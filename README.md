# Words Without Friends

**Words Without Friends** is a word game written in C that runs as a local HTTP server. It allows users to interact with the game through their web browser at `http://localhost:8000/words`.

## Description

The program selects a random "master word" from a dictionary and generates all valid sub-words that can be created from its letters. Users can then play the game by visiting a URL, where they input guesses and receive feedback via the browser.

## Features

- Built-in HTTP server running on `localhost:8000`
- Dictionary loading from `2of12.txt`
- Random selection of a master word
- Sub-word validation
- Dynamic memory management in C
- File I/O and string manipulation
- Terminal and browser-based interaction

## How to Compile and Run

1. Open your terminal in the project directory.

2. Compile the program using:

   ```bash
   gcc WWF.c -o WWF
   
3. Run the program:
   ```bash
   ./WWF

4. Open your browser and go to:
   http://localhost:8000/words


