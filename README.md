# kociemba
rubik's cube kocimeba solution implement


1.encode rules
a.position
Choose an orientation as your personal
in WCA cube competition ,use green as front ,white as top
then encode blocks : see <block.h>
b.
how to understand block orientation?
For edges: false state is 1,true state is 0;
For cornors :
if state is 1 ,it means this block Counterclockwise one time
if state is 2 ,it means this block Counterclockwise two times
if state is 0,the orientation is true
 
How to use :
To use my sample , follow you shuffle steps 
./main U2 L2 R2 …

With this way ,you have to make  clear disturb steps
If you want solve a cube you just shuffle  at will 
You have to record your cube status


Scan your cube and  record every block’s position and orientation 
Follow this rules and you can make a legal “cube_t” to search solution



2.how to record a shuffled  cube?

a.position
when you clear you personal cube orientation,
every clock’s serial number has clear,then check every position, 
record which block occupy this position
there is not a special skill

b.orientation
if block position is true ,it is easy to make sure it’s oriention 
if not,how to be sure it ?

first , we define a six face as three groups
UD/FB/LR 
UD is high level group
FB is middle level group
LR is low level group 

for edges blocks :
It has two faces,for any edges block ,if the higher level color on it  at high
level face,it has true orientation ,if not ,it’s false

for cornor blocks:
we focus on high level color(every cornor block has all level color),
when high level color at high level face state is 0
then
still follow the rules:
if state is 1 ,it means this block Counterclockwise one time
if state is 2 ,it means this block Counterclockwise two times
if state is 0,the orientation is true


 it’s easy to sure orientation state.