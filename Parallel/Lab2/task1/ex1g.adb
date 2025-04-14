with Ada.Text_IO;           use Ada.Text_IO;
with Ada.Numerics.Float_Random; use Ada.Numerics.Float_Random;
with Random_Seeds;         use Random_Seeds;
with Ada.Real_Time;         use Ada.Real_Time;

procedure Ex1g is

   -------------------------------
   -- Parametry symulacji
   -------------------------------
   Nr_Of_Travelers : constant Integer := 15;
   Min_Steps       : constant Integer := 10;
   Max_Steps       : constant Integer := 100;
   Min_Delay       : constant Duration := 0.01;  -- sekundy
   Max_Delay       : constant Duration := 0.05;

   Board_Width  : constant Integer := 15;
   Board_Height : constant Integer := 15;

   Start_Time : Time := Clock; -- globalny czas startu

   Seeds : Seed_Array_Type(1 .. Nr_Of_Travelers) := Make_Seeds(Nr_Of_Travelers);

   -------------------------------
   -- Typy danych
   -------------------------------
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
      for I in 0 .. Traces.Last loop
         Print_Trace(Traces.Trace_Array(I));
      end loop;
   end Print_Traces;

   -------------------------------
   -- Globalny licznik aktywnych agentów
   -------------------------------
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

   Agent_Counter : Counter_Type;

   -------------------------------
   -- Typ i definicja dzikiego lokatora
   -------------------------------
   type Wild_Tenant_Type is record
      Id       : Integer;
      Symbol   : Character;
      Position : Position_Type;
   end record;

   -------------------------------
   -- Zadanie Tile – reprezentuje jedno pole planszy.
   -- Rozróżniamy stany pola – puste, zajęte przez podróżnika lub przez lokatora.
   -------------------------------
   type Occupant_State is (Empty, Occupied_Traveler, Occupied_Wild);

   task type Tile is
      -- Inicjalizacja pola: zapisanie pozycji (przydane do relokacji) oraz identyfikatora pola.
      entry Init(X : Integer; Y : Integer; Id : Integer);
      entry Try_Acquire(Success : out Boolean);
      -- W entry Acquire dodajemy parametr From, czyli pozycję pola, z którego podróżnik przychodzi.
      entry Acquire(From : in Position_Type; Success : out Boolean);
      entry Release;
      -- Entry dla relokacji lokatora – pozwala przyjąć lokatora przenoszonego z innego pola.
      entry Place_Wild(WT : in Wild_Tenant_Type; Success : out Boolean);
      entry Stop;
   end Tile;

   task body Tile is
      -------------------------
      -- Dane wewnętrzne pola
      -------------------------
      Board_Ref : access Board_Type := null;
      Pos_X, Pos_Y : Integer;
      Field_Id     : Integer;
      -- Stan pola: początkowo puste
      Occupant : Occupant_State := Empty;
      -- Gdy pole jest zajęte przez lokatora, przechowujemy jego dane:
      Current_WT : Wild_Tenant_Type;
      -- Generator do losowych zdarzeń lokalnych (np. spawn lokatora)
      G : Generator;
      -- Flaga zakończenia zadania
      Exit_Task : Boolean := False;
      -- Lokalna kolejka śladów – pole może rejestrować zmianę stanu (lokator spawn, relokacja itp.)
      Traces : Traces_Sequence_Type;
      -- Pomocnicza procedura do zapisu śladu dla lokatora
      procedure Store_Trace(Symbol : in Character; Id : in Integer) is
         Time_Stamp : Duration := To_Duration(Clock - Start_Time);
      begin
         Traces.Last := Traces.Last + 1;
         Traces.Trace_Array(Traces.Last) := (
             Time_Stamp => Time_Stamp,
             Id         => Id,
             Position   => (X => Pos_X, Y => Pos_Y),
             Symbol     => Symbol
         );
      end Store_Trace;

      procedure Initialize(B : access Board_Type) is
      begin
         Board_Ref := B;
      end Initialize;
   begin
      accept Init(X : Integer; Y : Integer; Id : Integer) do
         Pos_X := X;
         Pos_Y := Y;
         Field_Id := Id;
         Reset(G, -Id);  -- inicjalizacja generatora – dla powtarzalności
      end Init;

      -- po Init, inicjalizujemy Board_Ref
      Initialize(Board'Access);  -- dostęp do całej planszy

      -- Główna pętla zadaniowa: oprócz obsługi wejść, losowo pojawia się lokator, jeśli pole jest puste.
      loop
         select
            -- Obsługa wejścia Try_Acquire – nie blokujące, dla ustawienia pozycji początkowej podróżnika.
            accept Try_Acquire(Success : out Boolean) do
               if Occupant = Empty then
                  Occupant := Occupied_Traveler;
                  Success := True;
               else
                  Success := False;
               end if;
            end Try_Acquire;
         or
            -- Obsługa Acquire: podróżnik przychodzi z pola From.
            accept Acquire(From : in Position_Type; Success : out Boolean) do
               if Occupant = Empty then
                  Occupant := Occupied_Traveler;
                  Success := True;
               elsif Occupant = Occupied_Wild then
                  -- Próba relokacji lokatora – przeglądamy sąsiednie pola.
                  declare
                     Relocated : Boolean := False;
                     Candidate : Position_Type;
                     Temp_Success : Boolean;
                  begin
                     -- Lista sąsiadów (torusowa topologia)
                     for lol in 0 .. 3 loop
                        case lol is
                           when 0 =>
                              Candidate.X := (Pos_X + Board_Width - 1) mod Board_Width;
                              Candidate.Y := Pos_Y;
                           when 1 =>
                              Candidate.X := (Pos_X + 1) mod Board_Width;
                              Candidate.Y := Pos_Y;
                           when 2 =>
                              Candidate.X := Pos_X;
                              Candidate.Y := (Pos_Y + Board_Height - 1) mod Board_Height;
                           when 3 =>
                              Candidate.X := Pos_X;
                              Candidate.Y := (Pos_Y + 1) mod Board_Height;
                           when others =>
                              null;
                        end case;
                        -- Nie przyjmujemy pola, z którego przychodzi podróżnik
                        if (Candidate.X = From.X) and (Candidate.Y = From.Y) then
                           null;
                        else
                           -- Próba przeniesienia lokatora do sąsiedniego pola
                           Board_Ref.all(Candidate.X, Candidate.Y).Place_Wild(Current_WT, Temp_Success);
                           if Temp_Success then
                              -- Zapisujemy ślad relokacji lokatora (oznaczamy starą pozycję jako opuszczoną, np. małą literą)
                              Store_Trace(Character'Val(Character'Pos(Current_WT.Symbol) + 32), Current_WT.Id);
                              Relocated := True;
                              exit;
                           end if;
                        end if;
                     end loop;
                     if Relocated then
                        -- Lokator przeniesiony – pole staje się puste i podróżnik może zająć pole.
                        Occupant := Occupied_Traveler;
                        Success := True;
                     else
                        delay Max_Delay;  -- nie udało się przenieść lokatora, podróżnik musi spróbować ponownie
                        Success := False;
                     end if;
                  end;
               else
                  -- Pole zajęte przez innego podróżnika – nie powinno zdarzyć się w Acquire, ale dla bezpieczeństwa:
                  Success := False;
               end if;
            end Acquire;
         or
            -- Obsługa Release – podróżnik opuszcza pole.
            accept Release do
               if Occupant = Occupied_Traveler then
                  Occupant := Empty;
               end if;
            end Release;
         or
            -- Obsługa Place_Wild – przyjmujemy przenoszonego lokatora.
            accept Place_Wild(WT : in Wild_Tenant_Type; Success : out Boolean) do
               if Occupant = Empty then
                  Occupant := Occupied_Wild;
                  Current_WT := WT;
                  -- Zapisz pojawienie się lokatora (ślad)
                  Store_Trace(WT.Symbol, WT.Id);
                  Success := True;
               else
                  Success := False;
               end if;
            end Place_Wild;
         or
            accept Stop do
               Exit_Task := True;
            end Stop;
         or
            -- Gałąź odpowiedzialna za pojawianie się dzikiego lokatora, gdy pole jest wolne.
            when Occupant = Empty =>
               -- Losowy czas do pojawienia się lokatora – np. pomiędzy 0.5 a 2.0 sekundy.
               delay 0.5 + (2.0 - 0.5) * Duration(Random(G));
               if Occupant = Empty then
                  -- Tworzymy lokatora
                  Current_WT.Id := Field_Id; -- przykładowo wykorzystujemy ID pola, ujemne lub dodatnie, według potrzeb
                  Current_WT.Position := (X => Pos_X, Y => Pos_Y);
                  Current_WT.Symbol := Character'Val(Character'Pos('0') + Integer(Float'Floor(10.0 * Random(G))));
                  Occupant := Occupied_Wild;
                  Store_Trace(Current_WT.Symbol, Current_WT.Id);
                  -- Lokator ma ograniczony czas życia – np. 3 sekundy
                  delay 3.0;
                  -- Jeśli lokator nadal występuje (nie został przeniesiony przez podróżnika), usuń go:
                  if Occupant = Occupied_Wild then
                     -- Możemy zapisać ślad zniknięcia (opcjonalnie oznaczając np. '.')
                     Store_Trace('.', Current_WT.Id);
                     Occupant := Empty;
                  end if;
               end if;
         end select;
         exit when Exit_Task;
      end loop;
      -- Przekaż lokalne ślady do Printera przed zakończeniem
      Printer.Report(Traces);
   end Tile;

   -------------------------------
   -- Deklaracja globalnej planszy (macierz zadań Tile)
   -------------------------------
   type Tile; -- forward declaration
   type Tile_Ptr is access all Tile;
   type Board_Type is array (Integer range <>, Integer range <>) of Tile_Ptr;
   Board : Board_Type(0 .. Width - 1, 0 .. Height - 1);





   -------------------------------
   -- Zadanie Printer – zbiera i wypisuje ślady.
   -------------------------------
   task Printer is
      entry Report(Traces : Traces_Sequence_Type);
   end Printer;

   task body Printer is
   begin
      while Agent_Counter.Get_Count > 0 loop
         accept Report(Traces : Traces_Sequence_Type) do
            Print_Traces(Traces);
         end Report;
         Agent_Counter.Decrement;
      end loop;
      -- Zakończ działanie wszystkich pól
      for I in Board'Range(1) loop
         for J in Board'Range(2) loop
            Board(I, J).Stop;
         end loop;
      end loop;
   end Printer;

   -------------------------------
   -- Zadanie Traveler – symuluje podróżnika.
   -------------------------------
   type Traveler_Type is record
      Id : Integer;
      Symbol : Character;
      Position : Position_Type;
   end record;

   task type Traveler_Task_Type is
      entry Init(Id : Integer; Seed : Integer; Symbol : Character);
      entry Start;
   end Traveler_Task_Type;

   task body Traveler_Task_Type is
      G : Generator;
      Traveler : Traveler_Type;
      Time_Stamp : Duration;
      Nr_of_Steps : Integer;
      Traces : Traces_Sequence_Type;

      procedure Store_Trace is
      begin
         Traces.Last := Traces.Last + 1;
         Traces.Trace_Array(Traces.Last) :=
           ( Time_Stamp => To_Duration(Clock - Start_Time),
             Id         => Traveler.Id,
             Position   => Traveler.Position,
             Symbol     => Traveler.Symbol );
      end Store_Trace;
   begin
      accept Init(Id : Integer; Seed : Integer; Symbol : Character) do
         Reset(G, Seed);
         Traveler.Id := Id;
         Traveler.Symbol := Symbol;
         -- Wybieramy losowe pole startowe, próbując je zająć
         declare
            Success : Boolean;
            Pos     : Position_Type;
         begin
            loop
               Pos.X := Integer( Float'Floor( Float(Board_Width) * Random(G) ) );
               Pos.Y := Integer( Float'Floor( Float(Board_Height) * Random(G) ) );
               Board(Pos.X, Pos.Y).Try_Acquire(Success);
               if Success then
                  Traveler.Position := Pos;
                  exit;
               end if;
            end loop;
         end;
         Store_Trace;
         Nr_of_Steps := Min_Steps + Integer( Float(Max_Steps - Min_Steps) * Random(G));
      end Init;

      accept Start do
         null;
      end Start;

      -- Wykonywanie kroków
      Deadlock_Check:
      for Step in 0 .. Nr_of_Steps loop
         delay Min_Delay + (Max_Delay - Min_Delay) * Duration(Random(G));
         declare
            Success : Boolean;
            New_Pos : Position_Type := Traveler.Position;
            N : Integer;
         begin
            N := Integer( Float'Floor(4.0 * Random(G)) );
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
                  null;
            end case;
            -- Próbujemy wejść na nowe pole – przekazujemy również pozycję bieżącą (From)
            Board(New_Pos.X, New_Pos.Y).Acquire(Traveler.Position, Success);
            if Success then
               Board(Traveler.Position.X, Traveler.Position.Y).Release;
               Traveler.Position := New_Pos;
            else
               -- Wykryto potencjalny deadlock – zmieniamy symbol na małą literę, zapisujemy ślad i kończymy
               Traveler.Symbol := Character'Val(Character'Pos(Traveler.Symbol) + 32);
               Store_Trace;
               exit Deadlock_Check;
            end if;
         end;
         Store_Trace;
      end loop Deadlock_Check;
      Printer.Report(Traces);
   end Traveler_Task_Type;

   -------------------------------
   -- Deklaracja zadań travelerów
   -------------------------------
   type Traveler_Array is array (0 .. Nr_Of_Travelers - 1) of Traveler_Task_Type;
   Travel_Tasks : Traveler_Array;
   Symbol       : Character := 'A';

begin
   -- Wypisanie parametrów planszy dla skryptu display-travel.bash
   Put_Line("-1 " &
           Integer'Image(Nr_Of_Travelers) & " " &
           Integer'Image(Board_Width) & " " &
           Integer'Image(Board_Height));

   -- Inicjalizacja pól planszy
   declare
   -- plansza: tablica wskaźników
   Board : Board_Type(0 .. Board_Width - 1, 0 .. Board_Height - 1);
   begin
      -- alokujemy taski i zapisujemy wskaźniki
      for I in Board'Range(1) loop
         for J in Board'Range(2) loop
            Board(I, J) := new Tile;
         end loop;
      end loop;

      -- inicjalizujemy taski z przekazaniem wskaźnika na planszę
      for I in Board'Range(1) loop
         for J in Board'Range(2) loop
            Board(I, J).Init(I, J, -(I * Board_Width + J + 1), Board'Access);
         end loop;
      end loop;
   end;


   -- Inicjalizacja zadań travelerów
   for I in Travel_Tasks'Range loop
      Travel_Tasks(I).Init(I, Seeds(I + 1), Symbol);
      Symbol := Character'Succ(Symbol);
   end loop;

   -- Start zadań travelerów
   for I in Travel_Tasks'Range loop
      Travel_Tasks(I).Start;
   end loop;

end Ex1g;
