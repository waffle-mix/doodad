# doodad
lil shit UCI chess engine.

This is my first ever chess engine. It was originally coded in Python, but I've converted it to C++ due to peer pressure from the Stockfish dc.

I also plan to implement somewhat important chess features such as castling and en passant in the future, as well as iterative deepening.

BTW, my movegen is perfectly fine.



# doodad supports the following few UCI commands:
uci

isready

ucinewgame

position startpos moves *moves*

position fen *fen* moves *moves*

go depth *depth*

quit



# I've also added a few extra debugging commands of my own:
print board

print moves

go perft *depth*

go eval *depth*



# other notes:
I must admit, this engine is absolutely awful. It is still horrendously slow and its evaluation is just complete shit. Its search is almost certainly bugged, and I bet there are countless other bugs I haven't discovered yet. It doesn't even use bitboards (sacrilegious, I know). I just wish I had that voodoo dark magic that real engine devs have.
