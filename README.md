# Traffic-light-system
This code is responsible for a traffic system for a street where there are 2 directions for the cars,
either north to south or east to west. The sequence starts with cars north to south are moving (Their traffic light is green)
while the cars of east to west are stopped (their traffic light is red). When 5 seconds elapses, the north to south traffic 
light becomes yellow for 2 more seconds. Then it turns to red, but waits for another 1 second (for safety) and after that the
east to west traffic light becomes green and starts holding the sequence again.

On the other hand there are 2 pedestrians traffic lights (one for each direction). In a certain direction, if the cars
are moving (Green light) then the pedestrians on that direction are stopped (Red light) and vise versa.

If a pedestrain wanted to cross the street and the traffic light of cars is green (which means his traffic light is red),
he can press a button that will make his traffic light green and cars' traffic light red for 2 seconds so that he can cross
the street.
However, there are certain conditions that are followed in the code where the press won't be taken into considiration if:
-The button is pressed before at least 1 second is elapsed from the last time it was released.
-The button kept holding as pressed, it would be taken only as 1 press
-The button is pressed within the 2 seconds of switching

Lastly, a status of the green lights in the system is transmitted through UART0 in order to monitor our system well.


This project is done by:
Islam Ashraf 17P6086
Malak Walid 17P6095
Omar Saad 17P6008
Amira Alaa 17P6036
Alanood Yassin 18P5128
