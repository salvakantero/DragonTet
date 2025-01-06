
////////////////////////////////
DragonTet v1.0 salvaKantero 2025
////////////////////////////////

Compatible with:
- Dragon 32/64
- Tandy Color Computer 1/2/3


Package Contents:
- dtetdr.cas: Cassette file for loading on Dragon 32/64/200/Tano emulators.
- dtetcc.cas: Cassette file for loading on Tandy Coco 1/2/3 emulators.
- dtetdr.bin: Binary file for running on Dragon 32/64/200/Tano emulators.
- dtetcc.bin: Binary file for running on Tandy Coco 1/2/3 emulators.
- dtetdr.wav: Audio file for recording onto a real cassette compatible with Dragon 32/64/200/Tano.
- dtetcc.wav: Audio file for recording onto a real cassette compatible with Tandy Coco 1/2/3.
- dtet.c: Source code (includes compilation instructions).
- dtet.png: Cassette cover.
- Readme.txt/Leeme.txt: This file. Instructions and additional information.
- License.txt: Legal information about the licence.
- Bin2cas.pl/Bin2cas.txt: Perl script for generating custom .cas files.


The objective of DragonTet is to move, rotate, and place the falling pieces to form complete horizontal lines on the game board.
Each time you complete a line, it disappears, freeing up space on the board and increasing your score.
By completing a certain number of lines, the game level increases. With each level, the falling speed of the pieces accelerates, making the game more challenging.
If the pieces stack up to the top of the board and you can no longer place them, the game is over!

Single-Player Mode:
  Compete to achieve the highest score possible.
  From level 4 onwards, every 12 pieces played will generate trap lines or blocks.

Two-Player Mode (Optional):
  Each player has their own board.
  The winner is the one who survives the longest or achieves the highest score.
  From level 4 onwards, trap blocks and lines are generated on the opponent's board every 3 lines completed.



CONTROLS
========

PLAYER 1:

Rotate piece:
  Press <W>, the up arrow key, or joystick 1 up.

Move piece left:
  Press <A>, the left arrow key, or joystick 1 left.

Soft drop (faster descent):
  Press <S>, the down arrow key, or joystick 1 down.

Move piece right:
  Press <D>, the right arrow key, or joystick 1 right.


PLAYER 2:

Rotate piece:
  Press <I> or joystick 2 up.

Move piece left:
  Press <J> or joystick 2 left.

Soft drop (faster descent):
  Press <K> or joystick 2 down.

Move piece right:
  Press <L> or joystick 2 right.


COMMON CONTROLS:

Pause:
  Press <H> to pause. Any other key to continue.

Cancel:
  Press <X> to exit the game at any time and return to the main menu.



SCORING
=======

1 line:   100 × level number
2 lines:  300 × level number
3 lines:  500 × level number
4 lines:  800 × level number

* Soft dropping a piece awards 1 point per row descended.



TIPS AND TRICKS
===============

Plan Ahead:
  Observe the next piece to plan where to place the current one.

Keep the Board Tidy:
  Try to keep the board as clear as possible to avoid running out of space.

Maximise Your Score:
  Aim to complete multiple lines at the same time for higher scores.



Dragontet is released under GPL v3 for all software components* (see license.txt).
C SOURCE CODE AND RESOURCES AVAILABLE AT https://github.com/salvakantero/Dragontet
