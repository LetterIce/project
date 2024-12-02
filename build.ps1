ccpsx -Xo0x80010000 -Wall -O2 SpaceShooter.c -o SpaceShooter.cpe
cpe2x /ce SpaceShooter.cpe
.\mkpsxiso\mkpsxiso.exe -o game.iso -y .\cuesheet.xml

