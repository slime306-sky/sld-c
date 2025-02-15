This is my journey learning c/cpp with sdl

day 1 : 9-2-2025 :-

setup sdl (tutorial : https://youtu.be/Fy_4ACtmt7A?si=n_YbOqwWc5NLfGmo)

how bacis works 

made bauncy ball (tutorial : https://youtu.be/27iEILBpI04?si=cnEWkJg7MJ2XNqVd) 

https://github.com/user-attachments/assets/894ad354-e34a-4bca-a384-309e2f7866ca

file name : bouncy_ball.c 

day 2 : 10-2-2025 :-

made chess board (tutorial : https://youtu.be/U4ogK0MIzqk?si=tV_Uw3M2kXzetQjQ&t=27)
file name : chessboard.cpp

learned how to move object with mouse

move.cpp

learned how image is loaded

moveimage.cpp

day - 3 : 12-02-2025 :-

add pieces in chess board

![chessBoardWithPieces](https://github.com/user-attachments/assets/9b58a20d-85e3-4cd8-8230-9cd5841acf42)

chessBoardWithPieces.cpp

day - 4 : 13-02-2025 :-

pieces can move now 

pieces also can capture other pieces but there is no concept of legal moves and no chance wise play

https://github.com/user-attachments/assets/4b70acd4-50d8-47ca-a2fc-05b4e2f62153

chessPiecesCanMove.cpp

day - 5 : 14-02-2025 :-

pieces can still move but they have bit of broken animation

https://github.com/user-attachments/assets/5ba4935f-e4e3-4794-b7e8-ae45eb66e38b

chessPiecesMovesWithBrokenAnimation.cpp

time : 19:40

just solved some logic for fixing that broken animation
there was some issue's like i can capture my own pieces and when i click piece two time it gets removed
solution was i have to store old piece postion and check when i release it which color piece can when it in valid restore old postion 

https://github.com/user-attachments/assets/b3acdf50-3aa6-434e-90c1-c2cb41058977

chessPiecesMovesWithAnimation.cpp

day - 6 : 15-02-2025 :-

today i added notation for chess move 
castling notation is not add yet 

when i was adding notation there was problem like when i move piece to the same square it was counting as move and displaying the notation and notation were wrong

problem :

i cant flip notation nor board

https://github.com/user-attachments/assets/fe9919ea-3f88-4317-a04b-40f7a5e99e9d

chessWithNotation.cpp

