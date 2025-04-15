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