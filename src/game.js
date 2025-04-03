// global variables
var deviceID = "";
//expires:
var accessToken = "";
var baseURL = "https://api.particle.io/v1/devices/"

document.getElementById("data1").innerHTML = "REFRESH";

const xLength=7,yLength=6;
//create array and set all to 0, (0=open, 1=player 1(red), 2=player 2(blue)), 6x7
//let board=Array(yLength).fill(Array(xLength).fill(0));
let board = Array(yLength).fill().map(() => Array(xLength).fill(0));
let currentPlayer = 1; //1=player 1(red), 2=player 2(blue)
let gameOver = false;

const boardElement = document.getElementById('board');
const statusElement = document.getElementById('status');

createBoard();

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
}

function handleClick(e) {
  if (gameOver) return;
  
  const col = parseInt(e.target.dataset.col);
  const row = getLowestEmptyRow(col);
  
  if (row !== -1) {
    //send coord to board
    //event.preventDefault();
    var functionName = "setBoard"; // local variable functionName
    const temp=row+","+col;
    $.ajax({
      url: 'https://api.particle.io/v1/devices/' + deviceID + '/' + functionName,
      method: "POST",
      headers: { "Authorization": "Bearer " + accessToken },
      data: { arg: temp },
      success: () => alert("Message Sent! \n" +
        "(this doesn't mean it worked! \n" +
        "i.e., may not have a valid ID/Token/FunctionName/etc.)")
    });

    // Update board array
    board[row][col] = currentPlayer;
    // Update UI
    updateBoardUI();

    // Check for win
    if (checkWin(row, col)) {
      statusElement.textContent = `Player ${currentPlayer} (${currentPlayer === 1 ? 'Red' : 'Blue'}) wins!`;
      gameOver = true;
      return;
    }

    // Switch player
    currentPlayer = currentPlayer === 1 ? 2 : 1;
    statusElement.textContent = `Player ${currentPlayer} (${currentPlayer === 1 ? 'Red' : 'Blue'})'s turn`;
  }
}

function getLowestEmptyRow(col) {
  for (let row = yLength-1; row >= 0; row--) {
  //for(let row=0;row<yLength;row++){
      if (board[row][col] === 0){
        console.log(col+","+row);
        return row;
      }
  }
  return -1;
}

//breaks if not 6x7
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

function refresh(objButton) {
    var varName = "grabChip"; // your cloud variable name goes here
    $.ajax({
      url: baseURL + deviceID + '/' + varName,
      headers: { 'Authorization': 'Bearer ' + accessToken },
      method: 'GET',
      success: function (resp) {
        document.getElementById("data1").innerText = resp.result;
        const temp=JSON.stringify(resp.result);
        if(temp!="null"){
          //see if coord is empty and set
          //board[row][col] = currentPlayer;
          updateBoardUI();
        }
        
      },
    });
}