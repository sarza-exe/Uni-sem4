with Ada.Text_IO;          use Ada.Text_IO;
with Ada.Numerics.Float_Random; use Ada.Numerics.Float_Random;
with Random_Seeds;           use Random_Seeds;
with Ada.Real_Time;          use Ada.Real_Time;

procedure Mutex_Template is

   -- Configuration constants
   Nr_Of_Processes : constant Integer := 15;
   Min_Steps       : constant Integer := 50;
   Max_Steps       : constant Integer := 100;
   Min_Delay       : constant Duration := 0.01;
   Max_Delay       : constant Duration := 0.05;

   -- Process states
   type Process_State is (Local_Section, Entry_Protocol, Critical_Section, Exit_Protocol);

   Board_Width  : constant Integer := Nr_Of_Processes;
   Board_Height : constant Integer := Process_State'Pos(Process_State'Last) + 1;

   Start_Time : Time := Clock;

   Seeds : Seed_Array_Type(1 .. Nr_Of_Processes) := Make_Seeds(Nr_Of_Processes);

   -- Position and trace types
   type Position_Type is record
      X : Integer range 0 .. Board_Width - 1;
      Y : Integer range 0 .. Board_Height - 1;
   end record;

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

   type Boolean_Array is array(0 .. Nr_Of_Processes - 1) of Boolean;
   type Integer_Array is array(0 .. Nr_Of_Processes - 1) of Integer;

   protected Bakery_State is
      entry Set_Choosing(Id : Integer; Value : Boolean);
      entry Set_Number(Id : Integer; Ticket : Integer);
      function Max_Number return Integer;
      function Choosing(Id : Integer) return Boolean;
      function Ticket(Id : Integer) return Integer;
      function Global_Max return Integer;
   private
      Choosing_Array : Boolean_Array := (others => False);
      Ticket_Array   : Integer_Array := (others => 0);
      Max_Ticket     : Integer := 0;
   end Bakery_State;

   protected body Bakery_State is
      entry Set_Choosing(Id : Integer; Value : Boolean) when True is
      begin
         Choosing_Array(Id) := Value;
      end Set_Choosing;

      entry Set_Number(Id : Integer; Ticket : Integer) when True is
      begin
         Ticket_Array(Id) := Ticket;
         if Ticket > Max_Ticket then
            Max_Ticket := Ticket;
         end if;
      end Set_Number;

      function Max_Number return Integer is
         Max_Val : Integer := 0;
      begin
         for J in Ticket_Array'Range loop
            if Ticket_Array(J) > Max_Val then
               Max_Val := Ticket_Array(J);
            end if;
         end loop;
         return Max_Val;
      end Max_Number;

      function Choosing(Id : Integer) return Boolean is
      begin
         return Choosing_Array(Id);
      end Choosing;

      function Ticket(Id : Integer) return Integer is
      begin
         return Ticket_Array(Id);
      end Ticket;

      function Global_Max return Integer is
      begin
         return Max_Ticket;
      end Global_Max;
   end Bakery_State;

   -- Printer task for display
   task Printer is
      entry Report(Traces : Traces_Sequence_Type);
   end Printer;

   task body Printer is
   begin
      for I in 1 .. Nr_Of_Processes loop
         accept Report(Traces : Traces_Sequence_Type) do
            null;
         end Report;
         -- Print individual traces
         for J in 0 .. Traces.Last loop
            Put_Line(Duration'Image(Traces.Trace_Array(J).Time_Stamp)
                     & " " & Integer'Image(Traces.Trace_Array(J).Id)
                     & " " & Integer'Image(Traces.Trace_Array(J).Position.X)
                     & " " & Integer'Image(Traces.Trace_Array(J).Position.Y)
                     & " " & Traces.Trace_Array(J).Symbol);
         end loop;
      end loop;

      -- Print display parameters line
      Put("-1 " & Integer'Image(Nr_Of_Processes) & " "
          & Integer'Image(Board_Width) & " " & Integer'Image(Board_Height) & " ");
      for S in Process_State'Range loop
         Put(S'Image & ";");
      end loop;
      Put_Line("MAX_TICKET=" & Integer'Image(Bakery_State.Global_Max) & ";");
   end Printer;

   -- Process task type
   task type Process_Task_Type is
      entry Init(Id : Integer; Seed : Integer; Symbol : Character);
      entry Start;
   end Process_Task_Type;

   type Process is record
      Id       : Integer;
      Symbol   : Character;
      Position : Position_Type;
   end record;

   task body Process_Task_Type is
      G          : Generator;
      Time_Stamp : Duration;
      Nr_of_Steps: Integer;
      Traces     : Traces_Sequence_Type;

      procedure Store_Trace is
      begin
         Traces.Last := Traces.Last + 1;
         Traces.Trace_Array(Traces.Last) := (
            Time_Stamp => Time_Stamp,
            Id         => Process.Id,
            Position   => Process.Position,
            Symbol     => Process.Symbol);
      end Store_Trace;

      procedure Change_State(State : Process_State) is
      begin
         Time_Stamp := To_Duration(Clock - Start_Time);
         Process.Position.Y := Process_State'Pos(State);
         Store_Trace;
      end Change_State;

   begin  -- Process body
      accept Init(Id : Integer; Seed : Integer; Symbol : Character) do
         Reset(G, Seed);
         Process.Id     := Id;
         Process.Symbol := Symbol;
         Process.Position := (X => Id, Y => Process_State'Pos(Local_Section));
         Nr_of_Steps := Min_Steps + Integer(Float(Max_Steps - Min_Steps) * Random(G));
         Time_Stamp  := To_Duration(Clock - Start_Time);
         Store_Trace;
      end Init;

      accept Start;

      for Step in 0 .. Nr_of_Steps-1 loop
         -- Local Section
         delay Min_Delay + (Max_Delay - Min_Delay) * Duration(Random(G));
         Change_State(Entry_Protocol);

         -- Bakery Entry Protocol
         Bakery_State.Set_Choosing(Process.Id, True);
         declare
            My_Ticket : constant Integer := Bakery_State.Max_Number + 1;
         begin
            Bakery_State.Set_Ticket(Process.Id, My_Ticket);
         end;
         Bakery_State.Set_Choosing(Process.Id, False);

         -- Wait for turn
         for J in 0 .. Nr_Of_Processes-1 loop
            if J /= Process.Id then
               -- wait until J has chosen
               while Bakery_State.Choosing(J) loop
                  null;
               end loop;
               -- wait if J has lower ticket or same ticket and lower Id
               while Bakery_State.Ticket(J) /= 0 and then
                     (Bakery_State.Ticket(J) < Bakery_State.Ticket(Process.Id) or
                      (Bakery_State.Ticket(J) = Bakery_State.Ticket(Process.Id) and then J < Process.Id)) loop
                  null;
               end loop;
            end if;
         end loop;

         Change_State(Critical_Section);

         -- Critical Section
         delay Min_Delay + (Max_Delay - Min_Delay) * Duration(Random(G));

         Change_State(Exit_Protocol);
         -- Exit Protocol
         Bakery_State.Set_Ticket(Process.Id, 0);

         Change_State(Local_Section);
      end loop;

      Printer.Report(Traces);
   end Process_Task_Type;

   -- Main execution
   Process_Tasks : array(0 .. Nr_Of_Processes-1) of Process_Task_Type;
   Symbol        : Character := 'A';
begin
   for I in Process_Tasks'Range loop
      Process_Tasks(I).Init(I, Seeds(I+1), Symbol);
      Symbol := Character'Succ(Symbol);
   end loop;
   for I in Process_Tasks'Range loop
      Process_Tasks(I).Start;
   end loop;
end Mutex_Template;