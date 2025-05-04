# Ada Travelers Simulation Overview

## Travelers Array Initialization  
```ada
Travel_Tasks : array (…) of access General_Traveler_Task_Type
```

 Main allocates one task object per trap, legal traveler, and wild traveler, then calls each one’s Init entry (supplying an ID, seed & symbol). No explicit interfaces—any task matching the entry signatures “implements” the protocol.

## Printer task
Printer waits for exactly one Report(…) from each legal & wild traveler (in any order), prints their traces, then sets Finish := True and does the same for all trap tasks. Acts like two “phases” of collection & printing, synchronized via entries.

## Tile as protected server
```ada
protected type Tile is
  entry Enter(Id; Response);
  entry Leave;
end Tile;
```

Each board cell offers Enter (condition: initilized and non-legal) and Leave (condition: initialized).

On Enter:
  - If tile **unoccuppied**: occupy & Response := Success.
  - If tile **occuppied by Wild** and Enter was called by Legal: wild is nudged—tries relocating for up to 4 directions (using a nonblocking select … else; fail immediately if blocked); if successful, wild’s Relocate entry is called, then the legal takes the cell.
  - If tile is **occupied by Trap**: forwarded to trap task via its Its_A_Trap entry.
  - Otherwise: immediate Response := Fail.

## Traveler tasks (Legal, Wild, Trap)

Legal:

1. Picks a random start cell (probing with nonblocking Enter until success or Trap).
2. Takes N random steps (with a deadlock timeout), each step tracing time/pos/symbol; on Success or Trap it issues Leave on the old cell.
3. On completion sends its trace list to Printer.

Wild:
1. Delays until its random “appearance” time, then behaves like a single-step traveler but lives only until a “disappearance” time or until it enters a Trap, responds to external Relocate calls to move & record trace, then eventually leaves.

Trap:
1. Picks a random start cell, then loops waiting on Its_A_Trap from incoming travelers.
2. On catch: if legal ⇒ convert symbol to lowercase; if wild ⇒ try to nudge it off-board; record a trace, delay, reset symbol, record again; repeat until Finish.