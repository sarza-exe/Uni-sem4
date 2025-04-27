/*jslint browser:true */
/*global console */

const alphabetContainer = document.getElementById("alphabetButtons");
var answerDisplay = document.getElementById("hold");
var answer = "";
var life = 10;
var wordDisplay = [];
var winningCheck = "";
var guessedLetters = [];
const buttonReset = document.getElementById("reset");
const livesDisplay = document.getElementById("mylives");
var stickmanCanvas = document.getElementById("stickman");
var context = stickmanCanvas.getContext("2d");
var loadState = true;

//generate alphabet buttons
function generateAlphabet() {
    var buttonsHTML = "ABCDEFGHIJKLMNOPQRSTUVWXYZ".split("")
      .map(
        (letter) =>
          `<button class = "alphabetButtonJS" id="${letter}">
          ${letter}
          </button>`
      ).join("");
  
    return buttonsHTML;
  }

function handleLetterClick(event) {
    const isButton = event.target.nodeName === "BUTTON";
    if(isButton) {
        const buttonId = document.getElementById(event.target.id);
        buttonId.classList.add("selected");
    }
    return;
}

const words = [
    "RAINBOW",
    "ALIEN",
    "SPHINX",
    "TORCH",
    "DOOR",
    "HALF-LIFE",
    "ORANGES",
    "LADYBUG",
    "GIRL",
    "SIX-PACK"
];

function setAnswer() {
    const wordIndex = Math.floor(Math.random() * words.length);
    const chosenWord = words[wordIndex];

    answer = chosenWord;
    answerDisplay.innerHTML = generateAnswerDisplay(chosenWord);
}

function generateAnswerDisplay(word) {
    var wordArray = word.split("");
    for( var i = 0; i < wordArray.length; i++){
        if (wordArray[i] !== "-") {
            wordDisplay.push("_");
        } else {
            wordDisplay.push("-");
        }
    }
    return wordDisplay.join(" ");
}


function loadGameState() {
  const savedState = localStorage.getItem('hangmanGame');
  console.log(savedState);
  if (savedState) {
    const gameState = JSON.parse(savedState);
    life = gameState.life;
    wordDisplay = gameState.wordDisplay;
    answer = gameState.answer;
    guessedLetters = gameState.guessedLetters;
    
    // update interface
    loadStateAnimate();
    answerDisplay.innerHTML = wordDisplay.join(" ");
    livesDisplay.innerHTML = life > 1 ? `You have ${life} lives!` : `You have ${life} life!`;
    if(answer === wordDisplay.join("")) livesDisplay.innerHTML = "You Win!";

    // select and disable gueassed letters
    guessedLetters.forEach(letter => {
      const button = document.getElementById(letter);
      if (button) {
        button.disabled = true;
        button.classList.add("selected");
      }
    });
  }
}

function init() {
    answer = "";
    life = 10;
    wordDisplay = [];
    winningCheck = "";
    context.clearRect(0, 0, 400, 400);
    canvas();
    livesDisplay.innerHTML = 'You have 10 lives left!';
    setAnswer();
    alphabetContainer.innerHTML = generateAlphabet();
    alphabetContainer.addEventListener("click", handleLetterClick);
    if(loadState) loadGameState();
    loadState = true;
}

window.onload = init();

//reset (play again)
buttonReset.addEventListener("click", () => {
  loadState = false;
  guessedLetters = [];
  localStorage.removeItem("hangmanGame");
  init();
} );

function saveGameState(){
  const gameState = {
    life: life,
    wordDisplay: wordDisplay,
    answer: answer,
    guessedLetters: guessedLetters  // przykładowo tablica klikniętych liter
  };
  
  // Zapisywanie stanu gry
  localStorage.setItem('hangmanGame', JSON.stringify(gameState));
}


//guess click
function guess(event) {
    // Only proceed if a button was clicked
    if (event.target.nodeName !== "BUTTON") {
      return;
    }

    // Disable the button so that it can only be clicked once
    event.target.disabled = true;

    const guessWord = event.target.id;
    const answerArray = answer.split("");
    var counter = 0;
    console.log(answer);
    console.log(winningCheck);
    if (answer === winningCheck) {
      livesDisplay.innerHTML = `YOU WIN!`;
      saveGameState(); //Wwwwwwin
      return;
    } else {
      guessedLetters.push(guessWord);
      if (life > 0) {
        for (var j = 0; j < answer.length; j++) {
          if (guessWord === answerArray[j]) {
            wordDisplay[j] = guessWord;
            localStorage.setItem("worddisplay", wordDisplay);
            console.log(guessWord);
            answerDisplay.innerHTML = wordDisplay.join(" ");
            winningCheck = wordDisplay.join("");
            counter += 1;
          }
        }
        if (counter === 0) {
          life -= 1;
          counter = 0;
          animate();
        } else {
          counter = 0;
        }
        if (life > 1) {
          livesDisplay.innerHTML = `You have ${life} lives!`;
        } else if (life === 1) {
          livesDisplay.innerHTML = `You have ${life} life!`;
        } else {
          livesDisplay.innerHTML = `GAME OVER!</p><p>The word was ${answer}</p>`;
        }
      } else {
        saveGameState();
        return;
      }
      console.log(wordDisplay);
      if (answer === winningCheck) {
        livesDisplay.innerHTML = `YOU WIN!`;
        saveGameState();
        return;
      }
    }
    saveGameState();
  }
  
  alphabetContainer.addEventListener("click", guess);

  
  
function loadStateAnimate() {
  for (var i = 9; i >= life; i--)
    drawArray[i]();
}

// Hangman
function animate() {
  drawArray[life]();
  //console.log(drawArray[life]);
}

function canvas() {
  myStickman = document.getElementById("stickman");
  context = myStickman.getContext("2d");
  context.beginPath();
  context.strokeStyle = "#fff";
  context.lineWidth = 2;
}
  
