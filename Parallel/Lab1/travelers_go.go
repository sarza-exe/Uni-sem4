package main

import (
	"fmt"
	"math"
	"math/rand"
	"sync"
	"time"
)

type Seed_Array_Type []int

func MakeSeeds(size int) Seed_Array_Type {
	const SeedRange = 1.0e6
	seeds := make(Seed_Array_Type, size)
	for i := range seeds {
		seeds[i] = int(SeedRange * rand.Float64())
	}
	return seeds
}

const Nr_Of_Travelers int = 15

const Min_Steps int = 10
const Max_Steps int = 100

const Min_Delay time.Duration = 10000000
const Max_Delay time.Duration = 50000000

const Board_Width int = 15
const Board_Height int = 15

var Start_Time = time.Now()

var Seeds Seed_Array_Type = MakeSeeds(Nr_Of_Travelers)

// Types, procedures and functions

// Postitions on the board
type Position_Type struct {
	X int
	Y int
}

// elementary steps
func Move_Down(Position *Position_Type) {
	Position.Y = (Position.Y + 1) % Board_Height
}

func Move_Up(Position *Position_Type) {
	Position.Y = (Position.Y + Board_Height - 1) % Board_Height
}

func Move_Right(Position *Position_Type) {
	Position.X = (Position.X + 1) % Board_Width
}

func Move_Left(Position *Position_Type) {
	Position.X = (Position.X + Board_Width - 1) % Board_Width
}

// traces of travelers
type Trace_Type struct {
	Time_Stamp time.Duration
	Id         int
	Position   Position_Type
	Symbol     rune
}

type Trace_Array_type [Max_Steps + 1]Trace_Type

type Traces_Sequence_Type struct {
	// IT'S NOT -1 Last: Integer := -1;
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

// through Channels you can send and receive values with the channel operator, <-.
// The data flows in the direction of the arrow
func Printer(printerChan chan Traces_Sequence_Type) {
	for i := 0; i < Nr_Of_Travelers; i++ {
		report := <-printerChan
		Print_Traces(report)
	}
}

type Traveler_Type struct {
	Id       int
	Symbol   rune
	Position Position_Type
}

type Traveler_Task_Type struct {
	// i think i don't need a generator
	Traveler    Traveler_Type
	Time_Stamp  time.Duration
	Nr_of_Steps int
	Traces      Traces_Sequence_Type
	Start_Chan  chan struct{}
}

func (t *Traveler_Task_Type) Init(Id int, Seed int, Symbol rune) {
	t.Traveler.Id = Id
	t.Traveler.Symbol = Symbol
	t.Traces.Last = -1 // IS IT NEEEEEEEEDED?????

	rand.Seed(int64(Seed))
	t.Traveler.Position = Position_Type{ // Random initial position:
		X: int(math.Floor(float64(Board_Width) * rand.Float64())),
		Y: int(math.Floor(float64(Board_Height) * rand.Float64()))}

	t.Store_Trace() //store starting position
	// Number of steps to be made by the traveler
	t.Nr_of_Steps = Min_Steps + int(float64(Max_Steps-Min_Steps)*rand.Float64())
	// Time_Stamp of initialization
	t.Time_Stamp = time.Since(Start_Time)

	t.Start_Chan = make(chan struct{}) // Channel to synchronize the start of the task
}

func (t *Traveler_Task_Type) Start(printerChan chan Traces_Sequence_Type, wg *sync.WaitGroup) {
	// Wait for the start signal
	//<-t.Start_Chan
	defer wg.Done()

	fmt.Print("start")

	// Simulate the task doing some work for NrOfSteps
	for step := 0; step <= t.Nr_of_Steps; step++ {
		time.Sleep(Min_Delay + time.Duration(rand.Float64()*float64(Max_Delay-Min_Delay)))
		// Make a move
		t.Make_Step()
		t.Store_Trace()
		t.Time_Stamp = time.Since(Start_Time)
	}
	// When finished, send the report to the Printer.
	printerChan <- t.Traces
	//t.wg.Done() // Mark the task as done
}

func (t *Traveler_Task_Type) Make_Step() {
	// Generate a random number between 0 and 3
	N := int(4.0 * rand.Float64())

	switch N {
	case 0:
		Move_Up(&t.Traveler.Position)
	case 1:
		Move_Down(&t.Traveler.Position)
	case 2:
		Move_Left(&t.Traveler.Position)
	case 3:
		Move_Right(&t.Traveler.Position)
	default:
		fmt.Printf(" ?????????????? %d\n", N)
	}
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

func main() {
	var Travel_Tasks [Nr_Of_Travelers]Traveler_Task_Type
	var Symbol rune = 'A'

	printerChan := make(chan Traces_Sequence_Type, Nr_Of_Travelers)

	var wg sync.WaitGroup

	fmt.Printf(
		"-1 %d %d %d\n",
		Nr_Of_Travelers,
		Board_Width,
		Board_Height,
	)

	for index, element := range Travel_Tasks {
		element.Init(index, Seeds[index], Symbol)
		Symbol++
	}

	go Printer(printerChan)

	for _, task := range Travel_Tasks {
		wg.Add(1)
		go task.Start(printerChan, &wg)
	}
}
