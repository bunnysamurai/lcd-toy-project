# Intro

A love-letter to Chip's Challenge for Windows.

# Design

Let's start with the...

## Gameplay

Chip moves around a map, collecting DIP packaged ICs, and is trying to find the exit to move to the next level.


Along the way, there are obstacles, including:

* Static Terrain
  * Clear
  * Exit
  * Hint
  * Walls
  * Thin Walls
  * Invisible Walls
  * Ice
  * Green Arrow'd
  * Campfire
  * Water
  * Salt n' Peppa Gravel
  * Brown Dirt
  * Thief
  * Monster Maker
  * Brown Block Maker
  * Brown Trap
  * Cyan Teleporter
* Items
  * Chips
  * Keys (red, yellow, green, cyan)
  * Footwear (skate, fire boot, suction shoe, flipper)
* Dynamic Terrain
  * Chip Gate
  * Key Doors (red, yellow, green, cyan)
  * Brown Blocks (moveable)
  * Buttons
    * Green
    * Blue
    * Brown
    * Red
  * Magic Appearing Walls
  * Cyan Walls
  * Bombs
* Monsters
  * Frog Monster
  * Purple Balls
  * Blue Tanks
  * Brown Bacteria
  * Yellow Bugs
  * Cyan Mantas
  * Orange Fire
  * Dark Green Blubs
  * Cyan Stick Balls

Some obstacles can occupy the same space as others (for example, a key is under)

## High level architecture

A super loop with the following tasks:

* Keyboard input
* Entity processing
* Player processing
* Timer processing
* Event processing
* Paint screen
* Other checks (game over?)

All processing updates the "game state".

Painting the screen is done on demand, whenever the "game state" changes.  The entire screen is redrawn.

Side-effects will be registered as events, and an event processing loop will apply said side-effects?

## Entities

A tiled playfield with sprites for moving around.  There are two kinds:

* Sprites that move (Entities)
* Sprites that don't move (Entities)

They can be moved, something *happens* when another entity collides with them, and they can be drawn on the screen.

Moving entities live in a linked list.

Static entities live in a grid that matches the size of the board??

```c++
struct entity
{
public:

    void move(const game_state&);
    void paint();

private:

    grid_point position;
    uint32_t tile_index;
};
```
Entities need to know the following:

* How to update their state
*

## Screen

What needs to be seen on the screen?
* Chip's Challenge is played on a map grid of 32 by 32 tiles, with a visible window of 9x9 tiles.
* There are 3 three-digit numerical displays, for Level, Timer, and Chips.  They are rendered as seven-segment displays... would be cool to replicate that.
* There is a 2x4 grid of items Chip is currently carrying, of the same tile size as the playfield.

Our waveshare jobby is 240 rows by 320 columns.

An example of what will be seen:

```
+-+--+--+--+--+--+--+--+--+--+---------------+
>*|  |  |  |  |  |  |  |  |  |*             *<
>*+--+--+--+--+--+--+--+--+--+*+--+--+--+--+*<
>*|  |  |  |  |  |  |  |  |  |*|  |  |  |  |*<                   
>*+--+--+--+--+--+--+--+--+--+*+--+--+--+--+*<
>*|  |  |  |  |  |  |  |  |  |*                    
>*+--+--+--+--+--+--+--+--+--+*
>*|  |  |  |  |  |  |  |  |  |*                    
>*+--+--+--+--+--+--+--+--+--+*
>*|  |  |  |  |  |  |  |  |  |*                    
>*+--+--+--+--+--+--+--+--+--+*
>*|  |  |  |  |  |  |  |  |  |*                    
>*+--+--+--+--+--+--+--+--+--+*
>*|  |  |  |  |  |  |  |  |  |*                    
>*+--+--+--+--+--+--+--+--+--+*
>*|  |  |  |  |  |  |  |  |  |*                    
>*+--+--+--+--+--+--+--+--+--+*
>*|  |  |  |  |  |  |  |  |  |*                    
>*+--+--+--+--+--+--+--+--+--+*--------------------+
```
If we do 0.5 + 9 + 0.5 + 4 + 0.5 "tiles" across
and 0.5 + 9 + 0.5 "tiles" down

we have a tiled-up screen size of 14.5 tiles wide by 10 tiles high