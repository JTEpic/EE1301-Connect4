// global variables
var deviceID = "";
//expires:
var accessToken = "";
var baseURL = "https://api.particle.io/v1/devices/"

document.getElementById("data1").innerHTML = "REFRESH";
const TRIES_UNTIL_STOP = 5;

const xLength=7,yLength=6;
//create array and set all to 0, (0=open, 1=player 1(red), 2=player 2(blue)), 6x7
//let board=Array(yLength).fill(Array(xLength).fill(0));
let board = Array(yLength).fill().map(() => Array(xLength).fill(0));
let currentPlayer = 1; //1=player 1(red)(cloud), 2=player 2(blue)(photon)
let gameOver = false;

const boardElement = document.getElementById('board');
const statusElement = document.getElementById('status');

createBoard();
const interval = setInterval(function(){refresh()},1500);

function createBoard() {
  boardElement.innerHTML = '';
  for (let row = 0; row < yLength; row++) {
      for (let col = 0; col < xLength; col++) {
          const cell = document.createElement('div');
          cell.classList.add('cell');
          cell.dataset.row = row;
          cell.dataset.col = col;
          cell.addEventListener('click', handleClick);
          boardElement.appendChild(cell);
      }
  }
  updateBoardUI();
  //calls photon function
  //event.preventDefault();
  var functionName = "resetBoard"; // local variable functionName
  //empty temp necessary?
  const temp = "";
  $.ajax({
    url: 'https://api.particle.io/v1/devices/' + deviceID + '/' + functionName,
    method: "POST",
    headers: { "Authorization": "Bearer " + accessToken },
    data: { arg: temp },
    success: () => console.log("Reset Board"),
    error: () => console.log("Fail Send")
  });
}

function handleClick(e) {
  //if (gameOver) return;
  if (gameOver||currentPlayer==2) return;
  
  const col = parseInt(e.target.dataset.col);
  const row = getLowestEmptyRow(col);
  
  if (row !== -1) {
    //send coord to board
    sendBoard(row,col);

    // Update board array
    board[row][col] = 1;
    // Update UI
    updateBoardUI();

    // Check for win
    if (checkWin(row, col)) {
      console.log("Game Over");
      statusElement.textContent = `${currentPlayer === 1 ? 'Cloud (Red' : 'Photon (Blue'}) wins!`;
      gameOver = true;
      sendWin();
      return;
    }

    // Switch player
    currentPlayer=2;
    //currentPlayer = currentPlayer === 1 ? 2 : 1;
    statusElement.textContent = `${currentPlayer === 1 ? 'Cloud (Red' : 'Photon (Blue'})'s turn`;
  }
}

function getLowestEmptyRow(col) {
  for (let row = yLength-1; row >= 0; row--) {
  //for(let row=0;row<yLength;row++){
      if (board[row][col] === 0){
        console.log(row+","+col);
        return row;
      }
  }
  console.log("Row = -1");
  return -1;
}

//send coord to board
let triesSend=0;
function sendBoard(row,col){
  //event.preventDefault();
  var functionName = "setBoard"; // local variable functionName
  const temp=row+","+col;
  $.ajax({
    url: 'https://api.particle.io/v1/devices/' + deviceID + '/' + functionName,
    method: "POST",
    headers: { "Authorization": "Bearer " + accessToken },
    data: { arg: temp },
    success: function(resp){
      console.log("Sent Data");
      triesSend=0;
    },
    error: function(resp){
      console.log("Fail Send");
      triesSend++;
      if(triesSend<TRIES_UNTIL_STOP)
        sendBoard(row,col);
      else
        triesSend=0;
    }
  });
}

//breaks if not 6x7 and only checks currentPlayer
function checkWin(row, col) {
  const directions = [
      [0, 1],  // horizontal
      [1, 0],  // vertical
      [1, 1],  // diagonal /
      [1, -1]  // diagonal \
  ];
  
  for (let [dr, dc] of directions) {
      let count = 1;
      
      // Check in positive direction
      for (let i = 1; i < 4; i++) {
          const r = row + dr * i;
          const c = col + dc * i;
          if (r < 0 || r >= 6 || c < 0 || c >= 7 || board[r][c] !== currentPlayer) break;
          count++;
      }
      
      // Check in negative direction
      for (let i = 1; i < 4; i++) {
          const r = row - dr * i;
          const c = col - dc * i;
          if (r < 0 || r >= 6 || c < 0 || c >= 7 || board[r][c] !== currentPlayer) break;
          count++;
      }
      
      if (count >= 4) return true;
  }
  return false;
}

function updateBoardUI() {
  const cells = document.querySelectorAll('.cell');
  cells.forEach(cell => {
      const row = parseInt(cell.dataset.row);
      const col = parseInt(cell.dataset.col);
      cell.classList.remove('red', 'blue');
      if (board[row][col] === 1) cell.classList.add('red');
      if (board[row][col] === 2) cell.classList.add('blue');
  });
}

//refresh data, should be put into a loop according to particle's cloud limits
function refresh(objButton) {
    if(gameOver) return;
    //if(gameOver||currentPlayer==1) return;
    var varName = "grabChip"; // your cloud variable name goes here
    $.ajax({
      url: baseURL + deviceID + '/' + varName,
      headers: { 'Authorization': 'Bearer ' + accessToken },
      method: 'GET',
      success: function (resp) {
        document.getElementById("data1").innerText = resp.result;
        console.log(resp.result);
        const temp=JSON.stringify(resp.result);
        //JSON adds "" to outsides
        if(temp!="\"null\""){
          //see if coord is empty and set
          //find location of comma
          let loc = 0;
          for (let x = 0; x < temp.length; x++){
            if (temp.charAt(x) == ',') {
              loc = x;
              console.log("Loc: "+loc);
              break;
            }
          }
          let row = parseInt(temp.substring(1, loc));
          console.log("Parsed row: "+row);
          let col = parseInt(temp.substring(loc + 1, temp.length));
          console.log("Parsed col: "+col);
          //if board has changed, else same data/no play yet
          if(board[row][col]==0){
            //remove later when player locked
            currentPlayer=2;
            board[row][col] = currentPlayer;
            updateBoardUI();
            // Check for win
            if (checkWin(row, col)) {
              console.log("Game Over");
              statusElement.textContent = `${currentPlayer === 1 ? 'Cloud (Red' : 'Photon (Blue'}) wins!`;
              gameOver = true;
              sendWin();
              return;
            }
            currentPlayer=1;
            statusElement.textContent = `${currentPlayer === 1 ? 'Cloud (Red' : 'Photon (Blue'})'s turn`;
          }
        }
      }
    });
}

//resets board
function resetBoard(objButton) {
  //resets photon, calls photon function
  //event.preventDefault();
  var functionName = "resetBoard"; // local variable functionName
  //empty temp necessary?
  const temp = "";
  $.ajax({
    url: 'https://api.particle.io/v1/devices/' + deviceID + '/' + functionName,
    method: "POST",
    headers: { "Authorization": "Bearer " + accessToken },
    data: { arg: temp },
    success: function(resp){
      console.log("Reset Board");
      //resets site
      for (let row = 0; row < yLength; row++) {
        for (let col = 0; col < xLength; col++) {
          board[row][col]=0;
        }
      }
      updateBoardUI();
      currentPlayer=1;
      statusElement.textContent = `${currentPlayer === 1 ? 'Cloud (Red' : 'Photon (Blue'})'s turn`;
      gameOver=false;
    },
    error: () => console.log("Fail Send")
  });
}

//tells board of win
let triesWin=0;
function sendWin(objButton) {
  //calls photon function
  //event.preventDefault();
  var functionName = "setWin"; // local variable functionName
  const temp = currentPlayer;
  $.ajax({
    url: 'https://api.particle.io/v1/devices/' + deviceID + '/' + functionName,
    method: "POST",
    headers: { "Authorization": "Bearer " + accessToken },
    data: { arg: temp },
    success: function(resp){
      console.log("Sent Win");
      triesWin=0;
    },
    error: function(resp){
      console.log("Fail Send");
      triesWin++;
      if(triesWin<TRIES_UNTIL_STOP)
        sendWin();
      else
        triesWin=0;
    }
  });
}