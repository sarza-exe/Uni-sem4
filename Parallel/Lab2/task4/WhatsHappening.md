# Go Travelers Simulation Overview

## Travelers Array Initialization  
```go
 var travelers [Nr_Of_Traps + Nr_Of_Travelers + Nr_Of_Wild_Travelers]GeneralTraveler
travelers[id] = &Trap{}
travelers[id] = &Legal{}
travelers[id] = &Wild{}
```
Any type that defines all three methods with exact matching signatures is considered to implement GeneralTraveler interface — explicit declaration is not needed.


## Printer
 Printer is a struct that has two channels. First it gathers all reports (via report channel) from legals and wilds, then signals done channel, prints all trap's reports and again signals done channel


## Tile as Server
In tile stateful goroutines is used so it behaves like server. 

**Entering**:

If entering succeeds we send Success on Request.ResponseChannel.

- n.traveler is traveler currently on a tile
- Request.Traveler is traveler that wants to enter tile

If there is legal on tile we add Request to n.waiting slice.

If there is wild on tile and Request.Traveler is legal we try to move him. We choose direction and try to enter the tile for maximum of 100 ms (then we send fail to ResponseChannel).

If there is trap on the tile we move logic to the trap.

**Leaving**:

First we just set tile's traveler to nil
Then we check if there are EnterChannel requests in n.waiting. If <-request.ResponseChannel return response we continue cuz he moved on. Else we move the request.Traveler to our tile 
make(chan Response, 1) makes channel buffering up to 1 value


## Wild
 Wild just waits to appear then listens for relocation request and desapears after his live is over or he is trapped


## Trap
 Trap listens on TrapChannel. If there is legal we change our symbol to lowercase of their symbol. If there is wild we move them outside of the board and change our symbol to '?'. Then if something other then trap was caught we Store_Trace, wait a bit, change our symbol back to '#' and Store_Trace again.

 If trap want to enter another trap (on initialization) the response is set to fail
