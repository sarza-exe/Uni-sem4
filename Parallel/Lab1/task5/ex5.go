package main

import (
	"fmt"
	"math/rand"
	"strings"
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

// We use struct{} because it's the smallest data type in go and we don't send any data
var Board [Board_Width][Board_Height]chan struct{}

func Init_Board() {
	for i := 0; i < Board_Width; i++ {
		for j := 0; j < Board_Height; j++ {
			// creates chan with buffor of size 1, which means every chan can store max of 1 token (struct{})
			Board[i][j] = make(chan struct{}, 1)
			// free tile is one that contains token struct{}
			Board[i][j] <- struct{}{}
		}
	}
}

func Acquire_Tile(X, Y int) bool {
	// This is Timeout. Select proceeds with the first receive that's ready
	select {
	case <-Board[X][Y]:
		return true
	case <-time.After(Max_Delay):
		return false
	}
}

func Try_Acquire_Tile(X, Y int) bool { // for init
	select {
	case <-Board[X][Y]:
		return true
	default: // If value is not available return false immediately
		return false
	}
}

func Release_Tile(X, Y int) {
	//struct{}{}, the first pair {} defines an empty struct type, and the second pair {} is a literal that creates an instance of that empty struct.
	Board[X][Y] <- struct{}{}
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
	Direction   int
}

func (t *Traveler_Task_Type) Init(Id int, Seed int, Symbol rune, wg *sync.WaitGroup) {
	t.Traveler.Id = Id
	t.Traveler.Symbol = Symbol
	t.Traces.Last = -1

	// Starting postion : (id, id)
	Pos := Position_Type{X: t.Traveler.Id, Y: t.Traveler.Id}
	Try_Acquire_Tile(Pos.X, Pos.Y)
	t.Traveler.Position = Pos

	// Get moving direction
	N := int(2.0 * rand.Float64())
	if N == 0 {
		t.Direction = 1
	} else if t.Traveler.Id%2 == 0 {
		t.Direction = Board_Height - 1
	} else {
		t.Direction = Board_Width - 1
	}

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
		New_Pos := t.Traveler.Position
		if t.Traveler.Id%2 == 0 {
			New_Pos.Y = (t.Traveler.Position.Y + t.Direction) % Board_Height
		} else {
			New_Pos.X = (t.Traveler.Position.X + t.Direction) % Board_Width
		}
		if Acquire_Tile(New_Pos.X, New_Pos.Y) {
			Release_Tile(t.Traveler.Position.X, t.Traveler.Position.Y)
			t.Traveler.Position = New_Pos
			t.Time_Stamp = time.Since(Start_Time)
			t.Store_Trace()
		} else {
			t.Traveler.Symbol = rune(strings.ToLower(string(t.Traveler.Symbol))[0])
			t.Time_Stamp = time.Since(Start_Time)
			t.Store_Trace()
			printerChan <- t.Traces
			defer Wait_for_Finish.Done()
			return
		}

	}
	// When finished, send the report to the Printer.
	printerChan <- t.Traces
	defer Wait_for_Finish.Done() // Mark the task as done. Defer waits for func to complete
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
	Init_Board()

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

	for index := range Travel_Tasks {
		Wait_for_Finish.Add(1)
		go Travel_Tasks[index].Start(printerChan, &Wait_for_Finish)
	}

	//And every program in Golang executes until main function is not terminated
	Wait_for_Finish.Wait()
	// we wait for Printer and all Travel_Tasks to finish
}
