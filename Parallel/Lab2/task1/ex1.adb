with Ada.Text_IO; use Ada.Text_IO;
with Ada.Numerics.Float_Random; use Ada.Numerics.Float_Random;
with Ada.Wide_Characters.Handling;
with Random_Seeds; use Random_Seeds;
with Ada.Real_Time; use Ada.Real_Time;
with Ada.Characters.Handling; use Ada.Characters.Handling;

procedure Ex1 is

-- Travelers moving on the board
  Nr_Of_Travelers : constant Integer := 15;
  Nr_Of_Wild_Travelers : constant Integer := 15;

  Min_Steps : constant Integer := 10;
  Max_Steps : constant Integer := 100;

  Min_Delay : constant Duration := 0.01;
  Max_Delay : constant Duration := 0.05;

-- 2D Board with torus topology

  Board_Width : constant Integer := 15;
  Board_Height : constant Integer := 15;

-- Timing
  Start_Time : Time := Clock;

-- Random seeds for the tasks' random number generators
  Seeds : Seed_Array_Type(1 .. Nr_Of_Travelers + Nr_Of_Wild_Travelers) := Make_Seeds(Nr_Of_Travelers + Nr_Of_Wild_Travelers);

-- Positions on the board
  type Position_Type is record
    X : Integer range 0 .. Board_Width;
    Y : Integer range 0 .. Board_Height;
  end record;

  procedure Move_Down(Position : in out Position_Type) is
  begin
    Position.Y := (Position.Y + 1) mod Board_Height;
  end Move_Down;

  procedure Move_Up(Position : in out Position_Type) is
  begin
    Position.Y := (Position.Y + Board_Height - 1) mod Board_Height;
  end Move_Up;

  procedure Move_Right(Position : in out Position_Type) is
  begin
    Position.X := (Position.X + 1) mod Board_Width;
  end Move_Right;

  procedure Move_Left(Position : in out Position_Type) is
  begin
    Position.X := (Position.X + Board_Width - 1) mod Board_Width;
  end Move_Left;

  procedure Move_Direction(Position : in out Position_Type; Direction : Integer) is
  begin
    case Direction is
      when 0 => Move_Up(Position);
      when 1 => Move_Down(Position);
      when 2 => Move_Left(Position);
      when 3 => Move_Right(Position);
      when others => Put_Line(" ?????????????? " & Integer'Image(Direction));
    end case;
  end Move_Direction;

-- traces of travelers
  type Trace_Type is record
    Time_Stamp : Duration;
    Id         : Integer;
    Position   : Position_Type;
    Symbol     : Character;
  end record;

  type Trace_Array_Type is array (0 .. Max_Steps) of Trace_Type;

  type Traces_Sequence_Type is record
    Last        : Integer := -1;
    Trace_Array : Trace_Array_Type;
  end record;

  -- Printer
  procedure Print_Trace(Trace : Trace_Type) is
  begin
    Put_Line(Duration'Image(Trace.Time_Stamp) & " " &
             Integer'Image(Trace.Id) & " " &
             Integer'Image(Trace.Position.X) & " " &
             Integer'Image(Trace.Position.Y) & ( ' ', Trace.Symbol ));
  end Print_Trace;

  procedure Print_Traces(Traces : Traces_Sequence_Type) is
  begin
    for I in 0 .. Traces.Last loop -- [0,... , Traces.Last]
      Print_Trace(Traces.Trace_Array(I));
    end loop;
  end Print_Traces;

-- task Printer collects and prints reports of traces
  task Printer is
    entry Report(Traces : Traces_Sequence_Type);
  end Printer;

  task body Printer is
  begin
    for I in 1 .. Nr_Of_Travelers + Nr_Of_Wild_Travelers loop
      accept Report(Traces : Traces_Sequence_Type) do -- it's not in order. It just waits until every thread has requested print
        Print_Traces(Traces);
      end Report;
    end loop;
  end Printer;

-- Travelers
  type Traveler_Variant is (Legal, Wild, None);

  type Traveler_Type is record
    Id       : Integer;
    Symbol   : Character;
    Position : Position_Type;
  end record;

-- Task type is a type of task that must be declared
-- Task would just start.
  task type Traveler_Task_Type is
    entry Init(Id : Integer; Seed : Integer; Symbol : Character);
    entry Start;
  end Traveler_Task_Type;

  task type Wild_Traveler_Task_Type is
    entry Init(Id : Integer; Seed : Integer; Symbol : Character);
    entry Start;
    entry Relocate(New_Position : Position_Type);
  end Wild_Traveler_Task_Type;

  type General_Traveler_Task_Type (Variant : Traveler_Variant) is record
    case Variant is
      when Legal =>
        Traveler_Task : Traveler_Task_Type;
      when Wild =>
        Wild_Traveler_Task : Wild_Traveler_Task_Type;
      when None =>
        null;
    end case;
  end record;

  protected type Tile is
    -- Entry will block until 'when' conditions are met
    entry Init(New_Position : Position_Type);
    entry Enter(New_Traveler : access General_Traveler_Task_Type; Success : out Boolean);
    entry Leave;
  private
    Inited : Boolean := False;
    Traveler : access General_Traveler_Task_Type;
    Position : Position_Type;
  end Tile;

  -- Global objects
  Board : array (0 .. Board_Width - 1, 0 .. Board_Height - 1) of Tile;
  -- Travel_Tasks allow entries in Tile to be called on any type of traveler
  -- access T type is equivalent to pointers
  Travel_Tasks : array (0 .. Nr_Of_Travelers + Nr_Of_Wild_Travelers - 1) of access General_Traveler_Task_Type;
  -- for removing travelers from board and initiating empty tiles
  Null_Task : access General_Traveler_Task_Type := new General_Traveler_Task_Type(Variant => None);
  
-- Task bodies
  protected body Tile is
    entry Init(New_Position : Position_Type) when not Inited is
    begin
      Position := New_Position;
      Traveler := Null_Task; -- initiate tiles with no travelers
      Inited := True;
    end Init;

    -- locks for legal travelers
    -- returns bool for wild travelers | tile must be initialized and traveler is none or wild
    -- Enter changes tile's travelers variable but if that happens we also have to change pos of travelers
    entry Enter(New_Traveler : access General_Traveler_Task_Type; Success : out Boolean) when Inited and Traveler.Variant /= Legal is
    begin
      -- if no one here, assign traveler
      if Traveler.Variant = None then
        Traveler := New_Traveler;
        Success := True;

      -- if wild here, try to move him
      elsif Traveler.Variant = Wild and New_Traveler.Variant = Legal then
        declare
          New_Position : Position_Type;
        begin
          for N in 0 .. 3 loop -- all directions
            New_Position := Position;
            Move_Direction(New_Position, N);
            select
              -- try to enter
              Board(New_Position.X, New_Position.Y).Enter(Traveler, Success);
              if Success then -- if not success keep trying
                exit;
              end if;
            else -- blocks if legal there - cant move
              Success := False;
            end select;
          end loop;

          if Success then -- signal wild and assign new traveler here
            Traveler.Wild_Traveler_Task.Relocate(New_Position);
            Traveler := New_Traveler;
          end if;
        end;

      -- any other cases - refuse enter
      else
        Success := False;
      end if;
    end Enter;

    -- used by travelers to leave the tile and by wildings to die
    entry Leave when Inited is
    begin
      Traveler := Null_Task;
    end Leave;
  end Tile;

  task body Traveler_Task_Type is
    G             : Generator;
    Traveler      : Traveler_Type;
    Time_Stamp    : Duration;
    Nr_of_Steps   : Integer;
    Traces        : Traces_Sequence_Type;

    procedure Store_Trace is
    begin
      Traces.Last := Traces.Last + 1;
      Traces.Trace_Array(Traces.Last) := (
        Time_Stamp => Time_Stamp,
        Id         => Traveler.Id,
        Position   => Traveler.Position,
        Symbol     => Traveler.Symbol
      );
    end Store_Trace;

    procedure Make_Step(Position : in out Position_Type) is
      N : Integer;
    begin
      N := Integer(Float'Floor(4.0 * Random(G)));
      Move_Direction(Position, N);
    end Make_Step;

  New_Position  : Position_Type;
  Success       : Boolean;
  Deadlock      : Boolean;
  begin
-- [accept Init do <code> end Init] synchronizes with main. 
-- So main stops until accept do... end block finished
    accept Init(Id : Integer; Seed : Integer; Symbol : Character) do
      Reset(G, Seed);
      Nr_of_Steps := Min_Steps + Integer(Float(Max_Steps - Min_Steps) * Random(G));
      Traveler.Id := Id;
      Traveler.Symbol := Symbol;

      -- try to move in
      Success := False;
      while not Success loop
        Traveler.Position := ( -- random position
          X => Integer(Float'Floor(Float(Board_Width) * Random(G))),
          Y => Integer(Float'Floor(Float(Board_Height) * Random(G)))
        );
        select
          Board(Traveler.Position.X, Traveler.Position.Y).Enter(Travel_Tasks(Traveler.Id), Success);
        else
          null;
        end select;
      end loop;
      Time_Stamp := To_Duration(Clock - Start_Time);
      -- Store starting position
      Store_Trace;
    end Init;

-- wait for initialisations of the remaining tasks:
-- It ensures every Init block in other tasks is finished
    accept Start do
      null;
    end Start;

-- it first makes all the steps noting it in Store_Trace
-- then sends traces to printer.report
    Deadlock := False;
    for Step in 0 .. Nr_of_Steps loop
      delay Min_Delay + (Max_Delay - Min_Delay) * Duration(Random(G));

      -- try to move
      Success := False;
      Deadlock := False;
      while not Success loop
        New_Position := Traveler.Position;
        Make_Step(New_Position);
        select
          -- Enter is used on the tile traveler wants to move to. So if there is another traveler it blocks
          Board(New_Position.X, New_Position.Y).Enter(Travel_Tasks(Traveler.Id), Success);
        or
          delay 6 * Max_Delay; -- deadlock
          Deadlock := True;
          exit;
        end select;
      end loop;

      if Deadlock then
        Traveler.Symbol := To_Lower(Traveler.Symbol);
        Time_Stamp := To_Duration(Clock - Start_Time);
        Store_Trace;
        exit;
      else
        Board(Traveler.Position.X, Traveler.Position.Y).Leave;
        Traveler.Position := New_Position;
        Time_Stamp := To_Duration(Clock - Start_Time);
        Store_Trace;
      end if;
    end loop;

    Printer.Report(Traces);
  end Traveler_Task_Type;

  task body Wild_Traveler_Task_Type is
    G              : Generator;
    Traveler       : Traveler_Type;
    Time_Stamp     : Duration;
    Traces         : Traces_Sequence_Type;
    Time_Appear    : Duration;
    Time_Disappear : Duration;

    procedure Store_Trace is
    begin
      Traces.Last := Traces.Last + 1;
      Traces.Trace_Array(Traces.Last) := (
        Time_Stamp => Time_Stamp,
        Id         => Traveler.Id,
        Position   => Traveler.Position,
        Symbol     => Traveler.Symbol
      );
    end Store_Trace;

  Success        : Boolean;
  begin
    accept Init(Id : Integer; Seed : Integer; Symbol : Character) do
      Reset(G, Seed);
      Traveler.Id := Id;
      Traveler.Symbol := Symbol;
      -- Time_Appear is anything in range [0, Max_Delay*Max_Steps)
      Time_Appear := (Max_Delay * Max_Steps) * Duration(Random(G));
      Time_Disappear := Time_Appear + (Max_Delay * Max_Steps - Time_Appear) * Duration(Random(G));
    end Init;

    accept Start do
      null;
    end Start;

    delay Time_Appear;

   -- choose initial position
    Success := False;
    while not Success loop
      Traveler.Position := ( -- random initial position
        X => Integer(Float'Floor(Float(Board_Width) * Random(G))),
        Y => Integer(Float'Floor(Float(Board_Height) * Random(G)))
      );
      select
        Board(Traveler.Position.X, Traveler.Position.Y).Enter(Travel_Tasks(Traveler.Id), Success);
      else
        null;
      end select;
    end loop;
    Time_Stamp := To_Duration(Clock - Start_Time);
    Store_Trace;

   -- accept relocation from legal travelers
    loop
      select
        -- forceful relocation
        accept Relocate(New_Position : Position_Type) do
          Traveler.Position := New_Position;
          Time_Stamp := To_Duration(Clock - Start_Time);
          Store_Trace;
        end Relocate;
      or
        delay 0.1; -- keep checking time
        if To_Duration(Clock - Start_Time) >= Time_Disappear then
          Board(Traveler.Position.X, Traveler.Position.Y).Leave;
          Traveler.Position := (X => Board_Width, Y => Board_Height);
          Time_Stamp := To_Duration(Clock - Start_Time);
          Store_Trace;
          exit;
        end if;
      end select;
    end loop;

    Printer.Report(Traces);
  end Wild_Traveler_Task_Type;


-- Local for main task

  Symbol : Character;
  Id : Integer;
begin -- beginning of MAIN

  -- Print the line with the parameters needed for display script:
  Put_Line(
    "-1 " &
    Integer'Image(Nr_Of_Travelers + Nr_Of_Wild_Travelers) & " " &
    Integer'Image(Board_Width) & " " &
    Integer'Image(Board_Height)
  );

  -- Initialize board's tiles
  for I in 0 .. Board_Width - 1 loop
    for J in 0 .. Board_Height - 1 loop
      Board(I, J).Init((X => I, Y => J));
    end loop;
  end loop;

-- init travelers tasks
  Id := 0;
  Symbol := 'A';
  for I in 0 .. Nr_Of_Travelers - 1 loop
    Travel_Tasks(Id) := new General_Traveler_Task_Type(Variant => Legal);
    Travel_Tasks(Id).Traveler_Task.Init(Id, Seeds(Id + 1), Symbol);
    Symbol := Character'Succ(Symbol);
    Id := Id + 1;
  end loop;

  -- init wild travelers
  Symbol := '0';
  for I in 0 .. Nr_Of_Wild_Travelers - 1 loop
    Travel_Tasks(Id) := new General_Traveler_Task_Type(Variant => Wild);
    Travel_Tasks(Id).Wild_Traveler_Task.Init(Id, Seeds(Id + 1), Symbol);
    -- wild travelers have symbols from '0' to '9' so it's kinda like modulo
    if Symbol = '9' then
      Symbol := '0';
    else
      Symbol := Character'Succ(Symbol);
    end if;
    Id := Id + 1;
  end loop;

   -- start tarvelers tasks
  Id := 0;
  for I in 0 .. Nr_Of_Travelers - 1 loop
    Travel_Tasks(Id).Traveler_Task.Start;
    Id := Id + 1;
  end loop;

   -- start wild tarvelers tasks
  for I in 0 .. Nr_Of_Wild_Travelers - 1 loop
    Travel_Tasks(Id).Wild_Traveler_Task.Start;
    Id := Id + 1;
  end loop;

end Ex1;