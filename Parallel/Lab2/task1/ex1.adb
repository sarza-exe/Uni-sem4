with Ada.Text_IO; use Ada.Text_IO;
with Ada.Numerics.Float_Random; use Ada.Numerics.Float_Random;
with Random_Seeds; use Random_Seeds;
with Ada.Real_Time; use Ada.Real_Time;

procedure Ex1 is

   -- Travelers moving on the board
   Nr_Of_Travelers : constant Integer := 15;
   Min_Steps       : constant Integer := 10;
   Max_Steps       : constant Integer := 100;
   Min_Delay       : constant Duration := 0.01; -- seconds
   Max_Delay       : constant Duration := 0.05;

   -- 2D Board with torus topology
   Board_Width  : constant Integer := 15;
   Board_Height : constant Integer := 15;

   -- Timing
   Start_Time : Time := Clock; -- global starting time

   -- Random seeds for the tasks' random number generators
   Seeds : Seed_Array_Type(1 .. Nr_Of_Travelers) := Make_Seeds(Nr_Of_Travelers);

   -- Types, procedures and functions

   -- Positions on the board
   type Position_Type is record
      X : Integer range 0 .. Board_Width - 1;
      Y : Integer range 0 .. Board_Height - 1;
   end record;


   -- traces of travelers
   type Trace_Type is record
      Time_Stamp : Duration;
      Id         : Integer;
      Position   : Position_Type;
      Symbol     : Character;
   end record;

   type Trace_Array_type is array(0 .. Max_Steps) of Trace_Type;

   type Traces_Sequence_Type is record
      Last        : Integer := -1;
      Trace_Array : Trace_Array_type;
   end record;

   procedure Print_Trace(Trace : Trace_Type) is
      Symbol : String := (' ', Trace.Symbol);
   begin
      Put_Line(
         Duration'Image(Trace.Time_Stamp) & " " & --'Image converts value to string
         Integer'Image(Trace.Id) & " " &
         Integer'Image(Trace.Position.X) & " " &
         Integer'Image(Trace.Position.Y) & " " &
         (' ', Trace.Symbol) -- print as string to avoid: '
      );
   end Print_Trace;

   procedure Print_Traces(Traces : Traces_Sequence_Type) is
   begin
      for I in 0 .. Traces.Last loop -- [0,... , Traces.Last]
         Print_Trace(Traces.Trace_Array(I));
      end loop;
   end Print_Traces;

   protected type Counter_Type is
      procedure Increment;
      procedure Decrement;
      function Get_Count return Natural;
   private
      Count : Natural := Nr_Of_Travelers;
   end Counter_Type;

   protected body Counter_Type is
      procedure Increment is
      begin
         Count := Count + 1;
      end Increment;

      procedure Decrement is
      begin
         if Count > 0 then
            Count := Count - 1;
         end if;
      end Decrement;

      function Get_Count return Natural is
      begin
         return Count;
      end Get_Count;
   end Counter_Type;

   Agent_Counter : Counter_Type; -- Globalna instancja licznika
   
   -- wild tenant
   type Wild_Tenant_Type is record
      Id       : Integer;
      Symbol   : Character;
      Position : Position_Type;
   end record;

   task type Wild_Tenant_Task_Type (Initial_X, Initial_Y : Integer; Life_Time : Duration);
   task body Wild_Tenant_Task_Type is
      procedure Store_Trace is
      begin
         Traces.Last := Traces.Last + 1;
         Traces.Trace_Array(Traces.Last) := (
            Time_Stamp => Time_Stamp,
            Id         => Wild_Tenant.Id,
            Position   => Wild_Tenant.Position,
            Symbol     => Wild_Tenant.Symbol
         );
      end Store_Trace;
   begin
         -- Inicjalizacja pozycji przy starcie zadania
      Wild_Tenant.Position.X := Initial_X;
      Wild_Tenant.Position.Y := Initial_Y;
      Store_Trace;  -- rejestracja początkowego stanu

      -- Oprócz głównego czasu życia, lokator może reagować na zmianę pozycji.
      declare
         Next_Change : Boolean := False;
      begin
         -- Uruchamiamy równolegle timer wygaszenia
         delay Life_Time;
      exception
         when others =>
            null;
      end;

      -- Główna pętla zadania, która pozostaje otwarta na wywołania entry.
      loop
         select
            -- Entry obsługująca zmianę pozycji Wild Tenant.
            accept Change_Position (New_X, New_Y : in Integer) do
               Wild_Tenant.Position.X := New_X;
               Wild_Tenant.Position.Y := New_Y;
               -- Każda zmiana pozycji skutkuje rejestracją śladu.
               Store_Trace;
            end Change_Position;
         or
            delay 0.0;  -- opcjonalnie, by nie blokować zadania, gdy nie przychodzi żadne żądanie
         end select;

         exit when false; -- lub w jakiś sposób zakończ zadanie, np. po przekroczeniu czasu życia
      end loop;
   end Wild_Tenant_Task_Type;

   task type Tile is
      entry Init(X : Integer; Y : Integer; Id : Integer);
      entry Try_Acquire(Success : out Boolean); -- Procedure doesn't block. Used for initial position
      entry Acquire(Success : out Boolean);       -- Entry will block until tile becomes unoccupied
      entry Release;
      entry Create_Wild_Tenant;
      entry Stop;
   end Tile;

   task body Tile is
      Occupied : Boolean := False;
      Exit_Task : Boolean := False;
      Wild_Tenant : Wild_Tenant_Type;
      Traces     : Traces_Sequence_Type;
      G          : Generator;

   begin
      -- [accept Init do <code> end Init] synchronizes with main.
      -- So main stops until accept do... end block finished
      accept Init(X : Integer; Y : Integer; Id : Integer) do
         Wild_Tenant.Position.X := X;
         Wild_Tenant.Position.Y := Y;
         Wild_Tenant.Id := Id;
         Reset(G, -Id);
         Wild_Tenant.Symbol := Character'Val(Character'Pos('0') + Integer(Float'Floor(10.0 * Random(G))));
         --Time_Stamp := To_Duration(Clock - Start_Time); -- reads global clock
      end Init;

      loop
         select
            accept Try_Acquire (Success : out Boolean) do
               if not Occupied then
                  Occupied := True;
                  Success := True;
               else
                  Success := False;
               end if;
            end Try_Acquire;
         or
            accept Acquire (Success : out Boolean) do
               if not Occupied then
                   Occupied := True;
                   Success := True;
                else
                   select
                        accept Release;
                        Occupied := True;
                        Success := True;
                   or
                         delay Max_Delay;
                         Success := False;
                   end select;
                end if;
            end Acquire;
         or
            accept Release do
               Occupied := False;
            end Release;
         or
            accept Create_Wild_Tenant do
               -- Utworzenie nowego zadania dzikiego lokatora
               --if Wild_Tenant = null and Occupied = False then
               --   Wild_Tenant := new Wild_Tenant_Task_Type'(Initial_X => Get_Tile_X,
               --                                             Initial_Y => Get_Tile_Y,
               --                                             Life_Time => 3 * Max_Delay);
               --end if;
               Put_Line ("XD");
            end Create_Wild_Tenant;
         or
            accept Stop do
               Exit_Task := True;
            end Stop;
         end select;
         exit when Exit_Task;
      end loop;
   end Tile;

   Board : array(0 .. Board_Width - 1, 0 .. Board_Height - 1) of Tile;

   -- task Printer collects and prints reports of traces
   task Printer is
      entry Report(Traces : Traces_Sequence_Type);
   end Printer;

   task body Printer is
   begin
      while Agent_Counter.Get_Count > 0 loop
         accept Report(Traces : Traces_Sequence_Type) do -- it's not in order. It just waits until every thread has requested print
               Print_Traces(Traces);
         end Report;
         Agent_Counter.Decrement;
      end loop;

      for I in Board'Range(1) loop
         for J in Board'Range(2) loop
            Board(I, J).Stop;
         end loop;
      end loop;
   end Printer;

   -- travelers
  type Traveler_Type is record
    Id: Integer;
    Symbol: Character;
    Position: Position_Type;    
  end record;

   -- task type is a type of task that must be declared
   -- task would just start. https://learn.adacore.com/courses/intro-to-ada/chapters/tasking.html#tasks
  task type Traveler_Task_Type is	
    entry Init(Id: Integer; Seed: Integer; Symbol: Character);
    entry Start;
  end Traveler_Task_Type;	

  task body Traveler_Task_Type is
    G : Generator;
    Traveler : Traveler_Type;
    Time_Stamp : Duration;
    Nr_of_Steps: Integer;
    Traces: Traces_Sequence_Type; 

    procedure Store_Trace is
    begin  
      Traces.Last := Traces.Last + 1;
      Traces.Trace_Array( Traces.Last ) := ( 
          Time_Stamp => Time_Stamp,
          Id => Traveler.Id,
          Position => Traveler.Position,
          Symbol => Traveler.Symbol
        );
    end Store_Trace;

  begin
    -- [accept Init do <code> end Init] synchronizes with main. 
    -- So main stops until accept do... end block finished
    accept Init(Id: Integer; Seed: Integer; Symbol: Character) do
      Reset(G, Seed); 
      Traveler.Id := Id;
      Traveler.Symbol := Symbol;
      -- Choose random position on board until you find one that's unoccupied
      declare
        Success : Boolean;
        Pos     : Position_Type;
      begin
        loop
            Pos.X := Integer( Float'Floor( Float( Board_Width )  * Random(G) ) );
            Pos.Y := Integer( Float'Floor( Float( Board_Height ) * Random(G) ) );
            Board(Pos.X, Pos.Y).Try_Acquire(Success);  -- Try to acquire the cell at this position.
            if Success then
              Traveler.Position := Pos; -- Successfully acquired the cell.
              exit;  -- exit the loop once a free cell is found.
            end if;
        end loop;
      end;
      -- Store starting position
      Store_Trace; 
      Nr_of_Steps := Min_Steps + Integer( Float(Max_Steps - Min_Steps) * Random(G));
      -- Time_Stamp of initialization
      Time_Stamp := To_Duration ( Clock - Start_Time ); -- reads global clock
    end Init;
    
    -- wait for initialisations of the remaining tasks:
    -- It ensures every Init block in other tasks is finished
    accept Start do
      null;
    end Start;

    -- it first makes all the steps noting it in Store_Trace
    -- then sends traces to printer.report
    Deadlock_Check:
    for Step in 0 .. Nr_of_Steps loop
      delay Min_Delay+(Max_Delay-Min_Delay)*Duration(Random(G));
      -- do action ...
      declare
        Success : Boolean;
        New_Pos : Position_Type;
        N       : Integer;
      begin
        N := Integer( Float'Floor(4.0 * Random(G)) );   
        New_Pos.Y := Traveler.Position.Y;
        New_Pos.X := Traveler.Position.X; 
        case N is
        when 0 =>
          New_Pos.Y := ( Traveler.Position.Y + Board_Height - 1 ) mod Board_Height;
        when 1 =>
          New_Pos.Y := ( Traveler.Position.Y + 1 ) mod Board_Height;
        when 2 =>
          New_Pos.X := ( Traveler.Position.X + Board_Width - 1 ) mod Board_Width;
        when 3 =>
          New_Pos.X := ( Traveler.Position.X + 1 ) mod Board_Width;
        when others =>
          Put_Line( " ?????????????? " & Integer'Image( N ) );
        end case;

         -- Try to acquire the tile
         Board(New_Pos.X, New_Pos.Y).Acquire(Success);
         if Success then
              -- Release the tile when the traveler is leaving
              Board(Traveler.Position.X, Traveler.Position.Y).Release;
              -- Commit the move
              Traveler.Position := New_Pos;
         else
            -- Change symbol to lowercase and exit loop
            Traveler.Symbol := Character'Val(Character'Pos(Traveler.Symbol) + 32);
            Store_Trace;
            exit Deadlock_Check;
         end if;

      end;
      Store_Trace;
      Time_Stamp := To_Duration ( Clock - Start_Time );
    end loop Deadlock_Check;
    Printer.Report( Traces );
  end Traveler_Task_Type;

   -- local for main task
   Travel_Tasks : array(0 .. Nr_Of_Travelers - 1) of Traveler_Task_Type; -- for tests
   Symbol       : Character := 'A';
begin -- beginning of MAIN

   -- Prit the line with the parameters needed for display script:
   Put_Line(
      "-1 " &
      Integer'Image(Nr_Of_Travelers) & " " &
      Integer'Image(Board_Width) & " " &
      Integer'Image(Board_Height)
   );

   for I in Board'Range(1) loop
         for J in Board'Range(2) loop
            Board(I, J).Init(I, J, -(I * Board_Width + J + 1)); -- ID ranges from -1 to -225
         end loop;
      end loop;

   -- init travelers tasks
   for I in Travel_Tasks'Range loop
      Travel_Tasks(I).Init(I, Seeds(I + 1), Symbol); -- `Seeds(I+1)` is still here :-(
      Symbol := Character'Succ(Symbol);             -- Succ returns next character
   end loop;

   -- start travelers tasks
   for I in Travel_Tasks'Range loop
      Travel_Tasks(I).Start;
   end loop;

end Ex1;