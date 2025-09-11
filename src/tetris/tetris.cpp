#include "tetris.hpp"

/* Tetris for Windows is 10 x 20 grid points */

/* Tetriminos
 *
 * As we all know, there are seven:
 *
 *    ++++  A
 *
 *    +++   +++  B/C
 *      +   +
 *
 *    ++    ++   D/E
 *     ++  ++
 *
 *     +    ++   F and G
 *    +++   ++
 *
 *
 * I'm going to steal, slightly, from Rheolism, and use a flower pattern for handling rotation
 *
 *         01
 *        AB23
 *        9854
 *         76
 *
 *   A: If at 1256, 9 incr -> AB23 -> 78B0 -> 4589 -> 1256
 *   B: If at B234, 9 incr -> 8B01 -> etc
 *   and so on.
 *
 * However, Tetris for Windows has different behaviour:
 *
 * B/C are in a 3x3 box
 *                              +
 *  Rotates around this one --> *
 *                              + +
 *
 *  So flower pattern is :     012
 *                             7*3
 *                             654
 *  and increment by 2 for each rotation
 *
 */

namespace tetris {
void run() {
    /* game loop:
     *
     * on every game tick:
     *   check for complete lines
     *     clear any if so
     *     move blocks above (if any) down that many rows
     *   move the active tetrimino down if one is active
     *     if moving down will intersect, mark active as dead, then restart game tick
     *   start a new tetrimino if no active tetrimino
     *   check if active tetrimino is above the play area (i.e. row 21)
     *     if so, GAME OVER MAN
     * as fast as possible:
     *   process user input
     *     if left or right, move the active tetrimino
     *     if up, rotate the active tetrimino
     *     if down, move the active tetrimino all the way down, then run the next game tick
     */

    /*
     * rules on processing user input:
     *   take action as soon as a key is pressed
     *   supress any further actions until all keys are released
     *
     * Score... uhh, level * 10 per line?
     *
     * If score gets high enough, increment the level.
     *
     * There are ten levels, where each one shortens the game tick period.
     */
}
} // namespace tetris