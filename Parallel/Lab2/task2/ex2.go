package main

import (
	"fmt"
	"math"
	"math/rand"
	"strings"
	"sync"
	"sync/atomic"
	"time"
)

// ---------------------------------------------------------------------------
// Ustawienia symulacji
// ---------------------------------------------------------------------------
const Nr_Of_Travelers int = 15

const Min_Steps int = 10
const Max_Steps int = 100

const Min_Delay time.Duration = 10 * time.Millisecond
const Max_Delay time.Duration = 50 * time.Millisecond

const Board_Width int = 15
const Board_Height int = 15

var Start_Time = time.Now()

var Report_Counter atomic.Int32

// ---------------------------------------------------------------------------
// Typy pomocnicze i narzędzia
// ---------------------------------------------------------------------------

// Losowanie nasion
type Seed_Array_Type []int

func MakeSeeds(size int) Seed_Array_Type {
	const SeedRange = 1.0e6
	seeds := make(Seed_Array_Type, size)
	for i := range seeds {
		seeds[i] = int(SeedRange * rand.Float64())
	}
	return seeds
}

var Seeds Seed_Array_Type = MakeSeeds(Nr_Of_Travelers)

// Pozycja na planszy
type Position_Type struct {
	X int
	Y int
}

// Definicje stanów pola
type CellState int

const (
	Empty CellState = iota
	TravelerOccupant
	WildOccupant
)

// ---------------------------------------------------------------------------
// Definicje komunikatów między podróżnikiem a polem
// ---------------------------------------------------------------------------
type CellCommand interface{}

type TravelerEnterRequest struct {
	travelerId int
	symbol     rune
	from       Position_Type // pole, z którego podróżnik nadchodzi
	reply      chan bool
}

type WildRelocateRequest struct {
	reply chan bool
}

type LeaveRequest struct {
	id int
}

// ---------------------------------------------------------------------------
// "Cell server" – gorutyna obsługująca pojedyncze pole
// ---------------------------------------------------------------------------
func cellServer(x, y int, cmd chan CellCommand, boardCells *[Board_Width][Board_Height]chan CellCommand) {
	state := Empty
	var occupantId int = -1
	//var occupantSymbol rune = ' '
	for {
		msg := <-cmd
		switch req := msg.(type) {
		case TravelerEnterRequest:
			if state == Empty {
				// Pole wolne – przyjmujemy podróżnika
				state = TravelerOccupant
				occupantId = req.travelerId
				//occupantSymbol = req.symbol
				req.reply <- true
			} else if state == TravelerOccupant {
				// Już zajęte przez innego podróżnika – ruch niedozwolony
				req.reply <- false
			} else if state == WildOccupant {
				// Pole zajęte przez dzikiego lokatora – spróbuj relokacji
				relocated := false
				// Wyznacz współrzędne sąsiadów (topologia torusa)
				candidates := []Position_Type{
					{X: x, Y: (y + Board_Height - 1) % Board_Height}, // góra
					{X: x, Y: (y + 1) % Board_Height},                // dół
					{X: (x + Board_Width - 1) % Board_Width, Y: y},   // lewo
					{X: (x + 1) % Board_Width, Y: y},                 // prawo
				}
				// Zgodnie z treścią zadania – wykluczyć pole pochodzenia ruchu (v)
				filtered := []Position_Type{}
				for _, pos := range candidates {
					if pos.X == req.from.X && pos.Y == req.from.Y {
						continue
					}
					filtered = append(filtered, pos)
				}
				// Próbujemy kolejno relokować lokatora do sąsiednich pól
				for _, pos := range filtered {
					replyWild := make(chan bool)
					relocReq := WildRelocateRequest{reply: replyWild}
					boardCells[pos.X][pos.Y] <- relocReq
					if success := <-replyWild; success {
						relocated = true
						break
					}
				}
				if relocated {
					// Udało się przenieść dzikiego lokatora – przyjmujemy podróżnika
					state = TravelerOccupant
					occupantId = req.travelerId
					//occupantSymbol = req.symbol
					req.reply <- true
				} else {
					// Lokatora nie udało się przenieść – ruch odrzucamy
					req.reply <- false
				}
			}
		case WildRelocateRequest:
			// Żądanie relokacji dzikiego lokatora: akceptujemy tylko, gdy pole jest wolne
			if state == Empty {
				state = WildOccupant
				occupantId = -1 // nieużywany
				// Dla uproszczenia symbol dzikiego lokatora ustawiamy na '0'
				//occupantSymbol = '0'
				req.reply <- true
			} else {
				req.reply <- false
			}
		case LeaveRequest:
			// Zwolnienie pola (pod warunkiem, że id się zgadza)
			if state == TravelerOccupant && occupantId == req.id {
				state = Empty
				occupantId = -1
				//occupantSymbol = ' '
			}
		}
	}
}

// Globalna tablica kanałów obsługujących pola (stateful goroutines)
var BoardCells [Board_Width][Board_Height]chan CellCommand

// Inicjalizacja planszy: tworzymy kanały i uruchamiamy gorutiny dla każdego pola.
func Init_BoardCells() {
	for i := 0; i < Board_Width; i++ {
		for j := 0; j < Board_Height; j++ {
			BoardCells[i][j] = make(chan CellCommand)
		}
	}
	// Uruchamiamy każdą gorutinę, przekazując referencję do całej tablicy BoardCells
	for i := 0; i < Board_Width; i++ {
		for j := 0; j < Board_Height; j++ {
			go cellServer(i, j, BoardCells[i][j], &BoardCells)
		}
	}
}

// ---------------------------------------------------------------------------
// System zapisu śladów (trace) podróżników
// ---------------------------------------------------------------------------
type Trace_Type struct {
	Time_Stamp time.Duration
	Id         int
	Position   Position_Type
	Symbol     rune
}

type Trace_Array_type [Max_Steps + 1]Trace_Type

type Traces_Sequence_Type struct {
	Last        int
	Trace_Array Trace_Array_type
}

func Print_Trace(Trace Trace_Type) {
	fmt.Printf(
		"%.6f %d %d %d %c\n",
		Trace.Time_Stamp.Seconds(),
		Trace.Id,
		Trace.Position.X,
		Trace.Position.Y,
		Trace.Symbol,
	)
}

func Print_Traces(Traces Traces_Sequence_Type) {
	for i := 0; i <= Traces.Last; i++ {
		Print_Trace(Traces.Trace_Array[i])
	}
}

func Printer(printerChan chan Traces_Sequence_Type, Wait_for_Finish *sync.WaitGroup) {
	for Report_Counter.Load() > 0 {
		report := <-printerChan
		Print_Traces(report)
		Report_Counter.Add(-1)
	}
	defer Wait_for_Finish.Done()
}

// ---------------------------------------------------------------------------
// Typ i logika podróżnika
// ---------------------------------------------------------------------------
type Traveler_Type struct {
	Id       int
	Symbol   rune
	Position Position_Type
}

type Traveler_Task_Type struct {
	Traveler    Traveler_Type
	Time_Stamp  time.Duration
	Nr_of_Steps int
	Traces      Traces_Sequence_Type
}

// Inicjalizacja podróżnika – losujemy startowe pole (za pomocą komunikatu do cell servera)
func (t *Traveler_Task_Type) Init(Id int, Seed int, Symbol rune, wg *sync.WaitGroup) {
	t.Traveler.Id = Id
	t.Traveler.Symbol = Symbol
	t.Traces.Last = -1

	rand.Seed(int64(Seed))
	// Losujemy pozycję, aż uda się "zarezerwować" pole
	for {
		Pos := Position_Type{
			X: int(math.Floor(float64(Board_Width) * rand.Float64())),
			Y: int(math.Floor(float64(Board_Height) * rand.Float64())),
		}
		replyChan := make(chan bool)
		// Żądamy wejścia jako podróżnik. Przy inicjalizacji nie mamy pola pochodzenia – można podać tą samą pozycję.
		req := TravelerEnterRequest{
			travelerId: t.Traveler.Id,
			symbol:     t.Traveler.Symbol,
			from:       Pos,
			reply:      replyChan,
		}
		BoardCells[Pos.X][Pos.Y] <- req
		if accepted := <-replyChan; accepted {
			t.Traveler.Position = Pos
			break
		}
		// Jeśli pole nie jest dostępne, losujemy ponownie
	}
	t.Store_Trace() // zapisujemy startowy ślad
	t.Nr_of_Steps = Min_Steps + int(float64(Max_Steps-Min_Steps)*rand.Float64())
	t.Time_Stamp = time.Since(Start_Time)
	defer wg.Done()
}

// Funkcja symulująca działanie podróżnika
func (t *Traveler_Task_Type) Start(printerChan chan Traces_Sequence_Type, Wait_for_Finish *sync.WaitGroup) {
	// Wykonujemy kolejne kroki symulacji
	for step := 0; step <= t.Nr_of_Steps; step++ {
		// Opóźnienie losowe
		time.Sleep(Min_Delay + time.Duration(rand.Float64()*float64(Max_Delay-Min_Delay)))
		// Losujemy kierunek ruchu
		New_Pos := t.Traveler.Position
		N := int(4.0 * rand.Float64()) // liczba od 0 do 3
		switch N {
		case 0:
			New_Pos.Y = (t.Traveler.Position.Y + Board_Height - 1) % Board_Height
		case 1:
			New_Pos.Y = (t.Traveler.Position.Y + 1) % Board_Height
		case 2:
			New_Pos.X = (t.Traveler.Position.X + Board_Width - 1) % Board_Width
		case 3:
			New_Pos.X = (t.Traveler.Position.X + 1) % Board_Width
		}
		replyChan := make(chan bool)
		// Wysyłamy żądanie wejścia do nowego pola. Parametr from to bieżąca pozycja (pole v)
		req := TravelerEnterRequest{
			travelerId: t.Traveler.Id,
			symbol:     t.Traveler.Symbol,
			from:       t.Traveler.Position,
			reply:      replyChan,
		}
		BoardCells[New_Pos.X][New_Pos.Y] <- req
		// Czekamy na odpowiedź (z timeoutem)
		select {
		case accepted := <-replyChan:
			if accepted {
				// Ruch się udał – wysyłamy komunikat o opuszczeniu poprzedniego pola
				BoardCells[t.Traveler.Position.X][t.Traveler.Position.Y] <- LeaveRequest{id: t.Traveler.Id}
				t.Traveler.Position = New_Pos
				t.Time_Stamp = time.Since(Start_Time)
				t.Store_Trace()
			} else {
				// Brak możliwości ruchu (prawdopodobny deadlock) – zmieniamy symbol na małą literę i kończymy działanie
				t.Traveler.Symbol = rune(strings.ToLower(string(t.Traveler.Symbol))[0])
				t.Time_Stamp = time.Since(Start_Time)
				t.Store_Trace()
				printerChan <- t.Traces
				defer Wait_for_Finish.Done()
				return
			}
		case <-time.After(Max_Delay):
			// Przekroczony timeout – traktujemy jak deadlock
			t.Traveler.Symbol = rune(strings.ToLower(string(t.Traveler.Symbol))[0])
			t.Time_Stamp = time.Since(Start_Time)
			t.Store_Trace()
			printerChan <- t.Traces
			defer Wait_for_Finish.Done()
			return
		}
	}
	// Po zakończeniu kroków, wysyłamy raport do Printera
	printerChan <- t.Traces
	defer Wait_for_Finish.Done()
}

func (t *Traveler_Task_Type) Store_Trace() {
	t.Traces.Last++
	t.Traces.Trace_Array[t.Traces.Last] = Trace_Type{
		Time_Stamp: t.Time_Stamp,
		Id:         t.Traveler.Id,
		Position:   t.Traveler.Position,
		Symbol:     t.Traveler.Symbol,
	}
}

// ---------------------------------------------------------------------------
// Funkcja main – inicjalizacja planszy, podróżników i start symulacji
// ---------------------------------------------------------------------------
func main() {
	// Inicjalizujemy boardCells (stateful goroutines)
	Init_BoardCells()

	var Travel_Tasks [Nr_Of_Travelers]Traveler_Task_Type
	var Symbol rune = 'A'

	printerChan := make(chan Traces_Sequence_Type, Nr_Of_Travelers)

	var Wait_to_Start sync.WaitGroup
	var Wait_for_Finish sync.WaitGroup

	fmt.Printf("-1 %d %d %d\n", Nr_Of_Travelers, Board_Width, Board_Height)

	// Inicjalizacja podróżników (losujemy startowe pozycje)
	for index := range Travel_Tasks {
		Wait_to_Start.Add(1)
		go Travel_Tasks[index].Init(index, Seeds[index], Symbol, &Wait_to_Start)
		Symbol++
	}
	Wait_to_Start.Wait() // Czekamy na zakończenie inicjalizacji

	// Ustalamy licznik raportów – podróżników (oraz potencjalnie wild tenantów) będziemy mieć tyle samo raportów
	Report_Counter.Add(int32(Nr_Of_Travelers))

	Wait_for_Finish.Add(1)
	go Printer(printerChan, &Wait_for_Finish)

	// Start symulacji dla każdego podróżnika
	for index := range Travel_Tasks {
		Wait_for_Finish.Add(1)
		go Travel_Tasks[index].Start(printerChan, &Wait_for_Finish)
	}

	Wait_for_Finish.Wait() // Czekamy na zakończenie Printera i wszystkich zadań podróżników
}
