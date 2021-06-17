Note: This (C++) version of the project is unlikely to be updated. I am working on converting the project to Ruby, and any further updates are likely to be on that version.


# Declination

## What?

A text adventure in the style of Zork, set in a space train

## Usage

cd into declination-cpp/  
`make`  
`./declination.exe`

## Features/expected behavior

'help' gives a list of commands.  
The whole command can be typed, or just the part in parentheses.  
For commands that can take multiple inputs, (e.g. 'take') seperate arguments by space, rather than comma space.

At game start, a random passenger is selected to be a terrorist. The end can change based on whether or not the terrorist is saved.

Saving a passenger increases the player's score. Different endings give different score bonus. The score can only be seen by beating the game.

The game has a turn limit, after which the game ends if not yet beaten.

A debug mode that outputs debug info (parsed input, who the terrorist is, etc)  
A demo mode that only has three rooms

*more*

## Planned Features

Save progress

Random compartment order

Inventory limit (count or weight, not yet decided)

Description for each item

Different terrorist dialogue depending on which passenger is the terrorist  
A method for figuring out who the terrorist is  
Different clues for each possible terrorist  
A way of fighting and defeating the terrorist

Certain passengers stick together (e.g. mother and child)

A command for more detailed room description

Dog as a companion  
Player gives dog a name

Player chooses name for themselves

More compartments

## *Features*  
### "It's not a bug, it's a feature"

'take', 'drop', ect do not work with multi-word items (because spaces seperate different args), so a hyphen is used to seperate words.

'time\_left' is turns, rather than minutes. Works fine as-is, so it won't be fixed unless I find a solution that uses time and works properly over multiple sessions (planning to add save feature later).

Probably typos in prints and comments

## Credit

Story written by Nick Deanda

Credit for code snippets in comments
