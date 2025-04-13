const alphabetContainer = document.getElementById("alphabetButtons");
var answerDisplay = document.getElementById("hold");
var answer = "";
var life = 10;
var wordDisplay = [];
var winningCheck = "";
const buttonReset = document.getElementById("reset");
const livesDisplay = document.getElementById("mylives");
var stickmanCanvas = document.getElementById("stickman");
var context = stickmanCanvas.getContext("2d");

//generate alphabet buttons
function generateAlphabet() {
    var buttonsHTML = "ABCDEFGHIJKLMNOPQRSTUVWXYZ".split("")
      .map(
        (letter) =>
          `<button
           class = "alphabetButtonJS" 
           id="${letter}"
           >
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
    "TORCH"
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

function init() {
    answer = "";
    life = 10;
    wordDisplay = [];
    context.clearRect(0, 0, 400, 400);
    canvas();
    livesDisplay.innerHTML = 'You have 10 lives left!';
    setAnswer();
    alphabetContainer.innerHTML = generateAlphabet();
    alphabetContainer.addEventListener("click", handleLetterClick);
}

window.onload = init();

//reset (play again)
buttonReset.addEventListener("click", init);

//guess click
function guess(event) {
    // Disable the button so that it can only be clicked once
    event.target.disabled = true;

    const guessWord = event.target.id;
    const answerArray = answer.split("");
    var counter = 0;
    if (answer === winningCheck) {
      livesDisplay.innerHTML = `YOU WIN!`;
      return;
    } else {
      if (life > 0) {
        for (var j = 0; j < answer.length; j++) {
          if (guessWord === answerArray[j]) {
            wordDisplay[j] = guessWord;
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
        return;
      }
      console.log(wordDisplay);
      if (answer === winningCheck) {
        livesDisplay.innerHTML = `YOU WIN!`;
        return;
      }
    }
  }
  
  alphabetContainer.addEventListener("click", guess);


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
  
  function head() {
    myStickman = document.getElementById("stickman");
    context = myStickman.getContext("2d");
    context.beginPath();
    context.arc(60, 25, 10, 0, Math.PI * 2, true);
    context.stroke();
  }
  
  function draw($pathFromx, $pathFromy, $pathTox, $pathToy) {
    context.moveTo($pathFromx, $pathFromy);
    context.lineTo($pathTox, $pathToy);
    context.stroke();
  }
  
  function frame1() {
    draw(0, 150, 150, 150);
  }
  
  function frame2() {
    draw(10, 0, 10, 600);
  }
  
  function frame3() {
    draw(0, 5, 70, 5);
  }
  
  function frame4() {
    draw(60, 5, 60, 15);
  }
  
  function torso() {
    draw(60, 36, 60, 70);
  }
  
  function rightArm() {
    draw(60, 46, 90, 55);
  }
  
  function leftArm() {
    draw(60, 46, 30, 55);
  }
  
  function rightLeg() {
    draw(60, 70, 90, 110);
  }
  
  function leftLeg() {
    draw(60, 70, 30, 110);
  }
  
  var drawArray = [
    rightLeg,
    leftLeg,
    rightArm,
    leftArm,
    torso,
    head,
    frame4,
    frame3,
    frame2,
    frame1
  ];