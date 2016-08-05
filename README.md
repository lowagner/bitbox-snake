# bitbox-snake
remake of the classic game, with dueling possible


## controls

dpad is all you need to get moving, but if you want fine grained control...

start pauses the game.  pressing buttons while paused, besides start, will modify the game.
* up/down:  speeds up/down the snakes
* right/left:  adds more/less food to the screen
* Y:  changes topography between torus and edges-will-kill-you.
* X:  toggles single-player only mode
* B:  changes gun mode.  fire a bullet with B in game if you toggle guns on.
  * guns 0:  no guns
  * guns 1-2-3:  bullets shoot slowly-fast-faster
* R/L:  increases/decreases starting snake size
* select:  takes a picture of the current screen, saves it to SNAKEXXX.PPM.

if you want to see the variables you are modifying, you can do so by pressing R+start.

in game, once you hit R+start, you lose your saved progress, and the game restarts 
(after unpausing).  (this is because the screen itself encodes the game state, so 
displaying other information will corrupt the game state.)

## screenshots

![option screen](https://raw.githubusercontent.com/lowagner/bitbox-snake/master/snake986.png)

the option/intro screen above, and other in-game screenshots below:

![in game action](https://raw.githubusercontent.com/lowagner/bitbox-snake/master/snake383.png)

![in game action](https://raw.githubusercontent.com/lowagner/bitbox-snake/master/snake593.png)

![in game action](https://raw.githubusercontent.com/lowagner/bitbox-snake/master/snake747.png)

![in game action](https://raw.githubusercontent.com/lowagner/bitbox-snake/master/snake001.png)

![in game action](https://raw.githubusercontent.com/lowagner/bitbox-snake/master/snake885.png)

## Super secret chiptune editor

Press L+start to get to the chiptune editor.  See
[the 16 wiki](https://github.com/makapuf/bitbox/wiki/Palette-16-Tile-Editor)
for commands and how to create a song.
