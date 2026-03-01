# Intro

A love-letter to Chip's Challenge for Windows.

# Design

Let's start with the...

## Screen

What needs to be seen on the screen?
* Chip's Challenge is played on a map grid sized up to X by X tiles, with a visible window of 9x9 tiles.
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