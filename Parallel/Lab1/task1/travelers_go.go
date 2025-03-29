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
func Printer(printerChan chan Traces_Sequence_Type, Wait_for_Finish *sync.WaitGroup) {
	for i := 0; i < Nr_Of_Travelers; i++ {
		report := <-printerChan
		Print_Traces(report)
	}
	defer Wait_for_Finish.Done()
}

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

func (t *Traveler_Task_Type) Init(Id int, Seed int, Symbol rune, wg *sync.WaitGroup) {
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

	defer wg.Done()
}

func (t *Traveler_Task_Type) Start(printerChan chan Traces_Sequence_Type, Wait_for_Finish *sync.WaitGroup) {
	// Simulate the task doing some work for NrOfSteps
	for step := 0; step <= t.Nr_of_Steps; step++ {
		time.Sleep(Min_Delay + time.Duration(rand.Float64()*float64(Max_Delay-Min_Delay)))
		// Make a move
		t.Make_Step()
		t.Time_Stamp = time.Since(Start_Time)
		t.Store_Trace()

	}
	// When finished, send the report to the Printer.
	printerChan <- t.Traces
	defer Wait_for_Finish.Done() // Mark the task as done. Defer waits for func to complete
}

func (t *Traveler_Task_Type) Make_Step() {
	N := int(4.0 * rand.Float64()) // Generate a random number between 0 and 3

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

	var Wait_to_Start sync.WaitGroup
	var Wait_for_Finish sync.WaitGroup

	fmt.Printf(
		"-1 %d %d %d\n",
		Nr_Of_Travelers,
		Board_Width,
		Board_Height,
	)

	//for index, element := range Travel_Tasks {  element.Init(index, Seeds[index], Symbol, &Wait_to_Start)
	//element makes a copy of Travel_Task
	for index := range Travel_Tasks {
		Wait_to_Start.Add(1)
		go Travel_Tasks[index].Init(index, Seeds[index], Symbol, &Wait_to_Start)
		Symbol++
	}

	Wait_to_Start.Wait() // Wait for Travel tasks to finish Init

	Wait_for_Finish.Add(1)
	go Printer(printerChan, &Wait_for_Finish)

	for _, task := range Travel_Tasks {
		Wait_for_Finish.Add(1)
		go task.Start(printerChan, &Wait_for_Finish)
	}

	//And every program in Golang executes until main function is not terminated
	Wait_for_Finish.Wait()
	// we wait for Printer and all Travel_Tasks to finish
}
