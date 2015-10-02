# bitbox-snake
remake of the classic game, with dueling possible


## controls

dpad is all you need to get moving, but if you want fine grained control...

start pauses the game.  pressing buttons while paused, besides start, will modify the game.
* up/down:  speeds up/down the snakes
* right/left:  adds more/less food to the screen
* Y:  changes topography between torus and edges-will-kill-you.
* X:  toggles single-player only mode
* R/L:  increases/decreases starting snake size
* select:  takes a picture of the current screen, saves it to snakeXXX.ppm.

if you want to see the variables you are modifying, you can do so by pressing R+start.
this must be done when the game is running (i.e. the game is unpaused).  once you do this,
however, you lose your saved progress, and the game restarts (after unpausing).  (this is because the screen
itself encodes the game state, so displaying other information will corrupt the game state.)

## screenshots

![option screen](https://raw.githubusercontent.com/lowagner/bitbox-modes/master/snake986.png)

the option screen...

![in game action](https://raw.githubusercontent.com/lowagner/bitbox-modes/master/snake383.png)

![in game action](https://raw.githubusercontent.com/lowagner/bitbox-modes/master/snake593.png)

![in game action](https://raw.githubusercontent.com/lowagner/bitbox-modes/master/snake747.png)
