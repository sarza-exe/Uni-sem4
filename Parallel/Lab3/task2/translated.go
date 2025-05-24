package main

import (
	"fmt"
	"math/rand"
	"sync"
	"sync/atomic"
	"time"
)

const Nr_Of_Processes int = 15

const Min_Steps int = 50
const Max_Steps int = 100

const Min_Delay time.Duration = 10000000
const Max_Delay time.Duration = 50000000

type Process_State int

const (
	LocalSection Process_State = iota
	EntryProtocol
	CriticalSection
	ExitProtocol
)

const Board_Width int = Nr_Of_Processes
const Board_Height int = int(ExitProtocol) + 1

var Start_Time = time.Now()

// Types, procedures and functions

type Max_Ticket_Atom struct {
	ticket int32
}

// Set atomically does: ticket = max(ticket, newTicket)
func (m *Max_Ticket_Atom) Set(newTicket int32) {
	for {
		old := atomic.LoadInt32(&m.ticket)
		if newTicket <= old {
			return
		}
		if atomic.CompareAndSwapInt32(&m.ticket, old, newTicket) {
			return
		}
		// else: someone else changed it in the meantime; retry
	}
}

func (m *Max_Ticket_Atom) Get() int32 {
	return atomic.LoadInt32(&m.ticket)
}

var Max_Ticket Max_Ticket_Atom

// Postitions on the board
type Position_Type struct {
	X, Y int
}

// traces of Processs
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
	defer Wait_for_Finish.Done()
	for i := 0; i < Nr_Of_Processes; i++ {
		report := <-printerChan
		Print_Traces(report)
	}

	fmt.Printf("-1 %d %d %d ", Nr_Of_Processes, Board_Width, Board_Height)

	fmt.Printf("LocalSection;EntryProtocol;CriticalSection;ExitProtocol;")
	// EXTRA_LABEL: strip leading space like Ada’s Image would
	fmt.Printf("MAX_TICKET=%d;\n", Max_Ticket.Get())

}

// not atomic but happens before any concurrency
var Choosing = [Nr_Of_Processes]int32{0}
var Number = [Nr_Of_Processes]int32{0}

type Process_Type struct {
	Id       int
	Symbol   rune
	Position Position_Type
}

type Process_Task_Type struct {
	Time_Stamp       time.Duration
	Nr_of_Steps      int
	Traces           Traces_Sequence_Type
	Process          Process_Type
	Local_Max_Ticket int32
}

func (t *Process_Task_Type) Init(Id int, Symbol rune, wg *sync.WaitGroup) {
	t.Process.Id = Id
	t.Process.Symbol = Symbol
	t.Traces.Last = -1
	t.Local_Max_Ticket = 0

	t.Process.Position = Position_Type{X: Id, Y: int(LocalSection)}

	// Number of steps to be made by the Process
	t.Nr_of_Steps = Min_Steps + int(float64(Max_Steps-Min_Steps)*rand.Float64())
	// Time_Stamp of initialization
	t.Time_Stamp = time.Since(Start_Time)
	t.Store_Trace() //store starting position

	defer wg.Done()
}

func (t *Process_Task_Type) Start(printerChan chan Traces_Sequence_Type, Wait_for_Finish *sync.WaitGroup) {
	// Simulate the task doing some work for NrOfSteps
	for step := 0; step < t.Nr_of_Steps/4; step++ {
		// LOCAL_SECTION - start
		time.Sleep(Min_Delay + time.Duration(rand.Float64()*float64(Max_Delay-Min_Delay)))
		// LOCAL_SECTION - end

		t.Change_State(EntryProtocol) // starting ENTRY_PROTOCOL

		atomic.StoreInt32(&Choosing[t.Process.Id], 1)
		atomic.StoreInt32(&Number[t.Process.Id], 1+Curr_Max_Ticket())
		atomic.StoreInt32(&Choosing[t.Process.Id], 0)
		ticket := atomic.LoadInt32(&Number[t.Process.Id])
		if ticket > t.Local_Max_Ticket {
			atomic.StoreInt32(&t.Local_Max_Ticket, ticket)
		}
		for j := 0; j < Nr_Of_Processes; j++ {
			if j != t.Process.Id {
				for atomic.LoadInt32(&Choosing[j]) != 0 {
				}

				for {
					jNumber := atomic.LoadInt32(&Number[j])
					myNumber := atomic.LoadInt32(&Number[t.Process.Id])

					if jNumber == 0 ||
						myNumber < jNumber ||
						(myNumber == jNumber && t.Process.Id < j) {
						break
					}
				}
			}
		}

		t.Change_State(CriticalSection) /// starting CRITICAL_SECTION

		// CRITICAL_SECTION - start
		time.Sleep(Min_Delay + time.Duration(rand.Float64()*float64(Max_Delay-Min_Delay)))
		// CRITICAL_SECTION - end

		t.Change_State(ExitProtocol) // starting EXIT_PROTOCOL
		atomic.StoreInt32(&Number[t.Process.Id], 0)
		time.Sleep(5)                // to avoid to timestamps at the same time
		t.Change_State(LocalSection) // starting LOCAL_SECTION

	}
	Max_Ticket.Set(t.Local_Max_Ticket)
	// When finished, send the report to the Printer.
	printerChan <- t.Traces
	defer Wait_for_Finish.Done() // Mark the task as done. Defer waits for func to complete
}

func Curr_Max_Ticket() int32 {
	var current int32 = 0
	for index := range Number {
		val := atomic.LoadInt32(&Number[index])
		if val > current {
			current = val
		}
	}
	return current
}

func (t *Process_Task_Type) Change_State(State Process_State) {
	t.Time_Stamp = time.Since(Start_Time)
	t.Process.Position.Y = int(State)
	t.Store_Trace()
}

func (t *Process_Task_Type) Store_Trace() {
	t.Traces.Last++
	//fmt.Printf("%c %d\t", t.Process.Symbol, t.Traces.Last)
	t.Traces.Trace_Array[t.Traces.Last] = Trace_Type{
		Time_Stamp: t.Time_Stamp,
		Id:         t.Process.Id,
		Position:   t.Process.Position,
		Symbol:     t.Process.Symbol,
	}
}

func main() {
	var Process_Tasks [Nr_Of_Processes]Process_Task_Type
	var Symbol rune = 'A'

	printerChan := make(chan Traces_Sequence_Type, Nr_Of_Processes)

	var Wait_to_Start sync.WaitGroup
	var Wait_for_Finish sync.WaitGroup

	//for index, element := range Process_Tasks {  element.Init(index, Symbol, &Wait_to_Start)
	//element makes a copy of Travel_Task
	for index := range Process_Tasks {
		Wait_to_Start.Add(1)
		go Process_Tasks[index].Init(index, Symbol, &Wait_to_Start)
		Symbol++
	}

	Wait_to_Start.Wait() // Wait for Travel tasks to finish Init

	Wait_for_Finish.Add(1)
	go Printer(printerChan, &Wait_for_Finish)

	for index := range Process_Tasks {
		Wait_for_Finish.Add(1)
		go Process_Tasks[index].Start(printerChan, &Wait_for_Finish)
	}

	//And every program in Golang executes until main function is not terminated
	Wait_for_Finish.Wait()
	// we wait for Printer and all Process_Tasks to finish
}
