# Puyo-Platform

Puyo platform is a server intended to expose the game of puyo as an HTTP API in
order to propose Puyo Puyo as a coding contest.

Contestants have to

    1. Create their game on /new and name it
    2. Play on /turn with the action they wish to do and the game's name

Those endpoints have an HTML browser friendly interface to manually dabble with
it, but also have a parser-friendly output if queried with the header `Accept:
text/plain`

Both of those plain text endpoints will answer with either

`ERROR` if something went wrong (most likely an invalid game ID), `LOST` if
you've lost the game, or an ASCII representation of the game following this
format

```
x_position_of_main_puyo  y_pos_of_main_puyo second_puyo_location
main_puyo_type second_puyo_type
game
```

where

    * `0 <= x_position_of_main_puyo < 6`. Leftmost is 0, rightmost is 5
    * `0 <= y_pos_of_main_puyo < 16`. Bottom is 0, top is 15
    * `second_puyo_location` is `UP`, `RIGHT`, `DOWN` or `LEFT`
    * `main_puyo_type` and `second_puyo_type` are puyo character codes
    * `game` is an ASCII drawing of the game, a grid of 16 lines and 6 columns
    with delimiting `|` and `=` containing:

        * ` ` for an empty location
        * `R` for a red puyo
        * `G` for a green puyo
        * `Y` for a yellow puyo
        * `B` for a blue puyo
        * `#` for a rock

For instance, here is a possible output with an extra `0` to denote the main
puyo's location which is not present is the real output

```
2 14 UP
R G
|      |
|  0   |
|      |
|      |
|      |
|      |
|      |
|      |
|      |
|      |
|      |
|      |
|      |
|   B  |
|   G  |
|  RGR |
========
```

The player can choose one of those actions at every turn:

    * `LEFT`: move left the main puyo
    * `RIGHT`: move right the main puyo
    * `DOWN`: move down the main puyo
    * `ROTR`: rotate the second puyo clockwise around the main puyo
    * `ROTL`: rotate the second puyo counter-clockwise around the main puyo

Independently of the player's choices, every three steps the puyos will move
down.


