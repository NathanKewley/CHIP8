//Object to hold all our game states.
var GameStates = {};

//document.addEventListener("DOMContentLoaded", function(){
function beginEmulation(){
	// Portrait game orientation.
	var width = 320;
	var height = 512;
	
	//lock the orientation (Must be uncommented for firefox)
	//lockedAllowed = window.screen.mozLockOrientation("portrait-primary");
	
	//Set up the game canvas
	game = new Phaser.Game(width, height, Phaser.CANVAS, "game");
	
	//Add the states to the game
	game.state.add('Main', GameStates.Main);
	
	//Start the initial state
	game.state.start('Main');
}
