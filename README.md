# BreakOut
Assignment One Readme

Welcome to the Game of Breakout. This game is designed and implemented by Mingkun Ni (ID 20655166). Following information is an explanation about my game so that you can learn more about thie game and win it after reading it.

1. The main program of this game is called "a1". To run this game, you need to offer two command line parameters: FPS and speed. FPS, Frame Per Second in full name, means the number of frames screen refreshes per second. Its range is between 10 and 60. Higher FPS will have better gaming experience :). Speed represents the speed of ball. Its range is from 1 to 10. If you miss FPS parameter, it will be set to default which is 60. For speed parameter, its default value is 5.
2. There exist three pages in this game. They perform different function:
  1) Welcome page
     This page is the first page appears when you just enter the game. It includes some instructions about how to play this game and some greeting from me. You can also select difficulty levels in this page
  2) Playground page
     This page is the main page that you will play. At first, the ball will stay at the middle of the pad. You can shot the ball out by press <space>. You can also move pad to adjust the location you want to launch the ball.
  3) End page
     When the ball touches the bottom of window at Playground page, game will be ended and you will enter the End page. In the end page, you can always press <r> to restart the game or press <q> to quit the game.

2. This game has 4 different modes that you can select:
  1) Easy mode
     You can enter easy mode by press e at the welcome page. In this mode, pad will be double-sized.
  2) Normal mode
     You can enter normal mode by press n at the welcome page. In this mode, pad will be set to normal size.
  3) Hard mode
     This is the mode to test you skill! You can enter hard mode by press h at the Welcome Page. In this mode, pad will be HALF-SIZED!
  4) TA mode 
     To be friendly to TA who may be a "terrible gamer" as described in assignment, I specially designed TA mode for test purpose. In this mode, pad will have extremely large size and you don't need to worry about losing game. Just enjoy it!

3. This game is entirely controlled by keyboard. You can do following operations by pressing corresponding keys in <>:
    <a> move pad leftword
    <d> move pad rightword
    <r> recreate all blockes on Playground Page / retry the game on End Page
    <space> make ball start to move
    <q> quit the game

4. The goal of this game is to break all blockes appear on the screen. You will initially have a block matrix that contains 50 blocks. There are three different ways to arrange blocks and it will randomly pick one when you recreate the matirx by pressing <r>. When you destroy one block, you will receive one score as an award. After you break all blocks, congradulation for you but the game does not end. New blocks will be loaded at the next time you hit the window edge so that you can achieve even higher score! You can always use pad to bounce the ball back up. When the ball reaches the bottom of the window, you lose the game unfortunately. However, you can always press <r> to retry the game at the End Page.

5. UI is designed by me. There are some features I need to mention to you here:
  1) FPS, speed, and your score are recorded at top left corner of the window
  2) The brick will have random generated color everytime you restart the game or recreate block matrix.
  3) Everytime you clear all 50 blockes, your pad will automatically change its color.
  4) When new block matrix is created, it will randomly choose one of three arrange pattern to generate new blocks

As the developer of this game, I truly hope you will enjoy this game and have an excellent experience playing with it.

Name: Mingkun Ni
ID: 20655166
Email: m8ni@uwaterloo.ca
