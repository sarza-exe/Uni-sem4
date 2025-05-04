package main

import (
	"fmt"
	"math/rand"
	"time"
	"unicode"
)

const Nr_Of_Travelers int = 15
const Nr_Of_Wild_Travelers int = 10
const Nr_Of_Traps = 10

const Min_Steps int = 10
const Max_Steps int = 100

const Min_Delay time.Duration = 10 * time.Millisecond
const Max_Delay time.Duration = 50 * time.Millisecond

const Board_Width int = 15
const Board_Height int = 15

var Start_Time = time.Now()

var printer = Printer{}

var Board = [Board_Width][Board_Height]Tile{}

type Position struct {
	X, Y int
}

// Movement functions wrap around edges using modulo
func Move_Down(pos *Position)  { pos.Y = (pos.Y + 1) % Board_Height }
func Move_Up(pos *Position)    { pos.Y = (pos.Y + Board_Height - 1) % Board_Height }
func Move_Right(pos *Position) { pos.X = (pos.X + 1) % Board_Width }
func Move_Left(pos *Position)  { pos.X = (pos.X + Board_Width - 1) % Board_Width }

func Move_Direction(pos *Position, direction int) {
	switch direction {
	case 0:
		Move_Up(pos)
	case 1:
		Move_Down(pos)
	case 2:
		Move_Left(pos)
	case 3:
		Move_Right(pos)
	}
}

type Trace struct {
	TimeStamp time.Duration
	Id        int
	Position  Position
	Symbol    rune
}

func Print_Traces(traces []Trace) {
	for _, trace := range traces {
		fmt.Printf("%f %d %d %d %c\n",
			float64(trace.TimeStamp)/float64(time.Second),
			trace.Id,
			trace.Position.X,
			trace.Position.Y,
			trace.Symbol,
		)
	}
}

type Printer struct {
	TraceChannel chan []Trace
	Done         chan bool
}

func (p *Printer) Start() {
	p.TraceChannel = make(chan []Trace, Nr_Of_Travelers)
	p.Done = make(chan bool)

	go func() {
		for i := 0; i < Nr_Of_Travelers+Nr_Of_Wild_Travelers; i++ {
			traces := <-p.TraceChannel
			Print_Traces(traces)
		}

		p.Done <- true

		for i := 0; i < Nr_Of_Traps; i++ {
			traces := <-p.TraceChannel
			Print_Traces(traces)
		}

		p.Done <- true
	}()
}

type GeneralTraveler interface {
	Init(id int, symbol rune)
	Start()
	Store_Trace()
}

type Response int

// iota starts at Success with number 0 and then increments by 1 in const block
const (
	Success Response = iota
	Fail
	Trapped
	Deadlock
)

type Traveler struct {
	Id        int
	Symbol    rune
	Position  Position
	traces    []Trace
	timeStamp time.Duration
	response  Response
}

type Legal struct {
	Traveler // this basically pastes in content of Traveler struct
	steps    int
}

type RelocateRequest struct {
	Position Position
	Status   Response
}
type Wild struct {
	Traveler
	RelocateChannel chan RelocateRequest

	timeAppear    time.Duration
	timeDisappear time.Duration
}

type TrapRequest struct {
	Traveler        GeneralTraveler
	ResponseChannel chan Response
}
type Trap struct {
	Traveler
	TrapChannel chan TrapRequest
	Done        chan bool

	traveler GeneralTraveler
}

type EnterRequest struct {
	Traveler        GeneralTraveler
	ResponseChannel chan Response
}
type Tile struct {
	EnterChannel chan EnterRequest
	LeaveChannel chan bool

	position Position
	traveler GeneralTraveler
	waiting  []EnterRequest
}

func (n *Tile) Init(position Position) {
	n.EnterChannel = make(chan EnterRequest)
	n.LeaveChannel = make(chan bool)

	n.position = position
	n.traveler = nil

	n.Start()
}

func (n *Tile) Start() {
	go func() {
		for true {
			// handle requests like a true server
			select {
			case Request := <-n.EnterChannel:
				// enter
				if n.traveler == nil {
					// if empty - assign traveler
					n.traveler = Request.Traveler
					Request.ResponseChannel <- Success
				} else if _, ok := n.traveler.(*Legal); ok {
					// if legal - add to queue to wait until someone leaves
					n.waiting = append(n.waiting, Request)
				} else if wild, ok := n.traveler.(*Wild); ok {
					// if wild - try to move him
					if _, ok := Request.Traveler.(*Legal); ok {
						var newPosition Position
						var TileResponse Response
						directions := []int{0, 1, 2, 3}
						for dir := range directions {
							newPosition = n.position
							Move_Direction(&newPosition, dir)

							request := EnterRequest{n.traveler, make(chan Response, 1)}
							Board[newPosition.X][newPosition.Y].EnterChannel <- request
							select {
							case TileResponse = <-request.ResponseChannel:
							case <-time.After(100 * time.Millisecond):
								request.ResponseChannel <- Fail
								TileResponse = Fail
							}
							if TileResponse == Success {
								break
							}
						}

						if TileResponse != Fail {
							if TileResponse != Trapped {
								wild.RelocateChannel <- RelocateRequest{newPosition, Success}
							}
							n.traveler = Request.Traveler
							Request.ResponseChannel <- Success
						}
					} else {
						Request.ResponseChannel <- Fail
					}
				} else if trap, ok := n.traveler.(*Trap); ok {
					// if trap move logic into the trap
					trap.TrapChannel <- TrapRequest{Request.Traveler, Request.ResponseChannel}
				} else {
					Request.ResponseChannel <- Fail
				}
			case <-n.LeaveChannel:
				n.traveler = nil
				for _, request := range n.waiting {
					select {
					case <-request.ResponseChannel:
						continue // he's moved on - dont answer
					default:
						request.ResponseChannel <- Success
						n.traveler = request.Traveler
						n.waiting = n.waiting[:0]
					}
					if n.traveler != nil {
						break
					}
				}
			}
		}
	}()
}

func (t *Traveler) Store_Trace() {
	t.traces = append(t.traces, Trace{
		TimeStamp: t.timeStamp,
		Id:        t.Id,
		Position:  t.Position,
		Symbol:    t.Symbol,
	})
}

func (t *Legal) Init(id int, symbol rune) {
	t.Id = id
	t.Symbol = symbol
	t.steps = Min_Steps + rand.Intn(Max_Steps-Min_Steps+1)

	t.response = Fail
	for t.response == Fail {
		t.Position = Position{
			X: rand.Intn(Board_Width),
			Y: rand.Intn(Board_Height),
		}

		request := EnterRequest{t, make(chan Response, 1)}
		Board[t.Position.X][t.Position.Y].EnterChannel <- request
		select {
		case t.response = <-request.ResponseChannel:
		case <-time.After(100 * time.Millisecond):
			request.ResponseChannel <- Fail
			t.response = Fail
		}
	}

	if t.response == Trapped {
		t.Position = Position{ // move legal off board
			X: Board_Width,
			Y: Board_Height,
		}
	}

	t.timeStamp = time.Since(Start_Time)
	t.Store_Trace()
}

func (t *Legal) Start() {
	go func() {
		for i := 0; i < t.steps; i++ {
			if t.response == Trapped || t.response == Deadlock {
				break
			}
			time.Sleep(Min_Delay + time.Duration(rand.Int63n(int64(Max_Delay-Min_Delay))))

			// try to move
			var newPosition Position
			t.response = Fail
			for t.response == Fail {
				newPosition = t.Position
				Move_Direction(&newPosition, rand.Intn(4))
				request := EnterRequest{t, make(chan Response, 1)}
				Board[newPosition.X][newPosition.Y].EnterChannel <- request
				select {
				case t.response = <-request.ResponseChannel:
				case <-time.After(6 * Max_Delay):
					request.ResponseChannel <- Deadlock
					t.response = Deadlock
				}
			}

			// handle response
			switch t.response {
			case Success:
				Board[t.Position.X][t.Position.Y].LeaveChannel <- true
				t.Position = newPosition
			case Trapped:
				Board[t.Position.X][t.Position.Y].LeaveChannel <- true
				t.Position = Position{
					X: Board_Width, // move legal off board
					Y: Board_Height,
				}
			case Deadlock:
				t.Symbol = unicode.ToLower(t.Symbol)
			}

			// store trace
			t.timeStamp = time.Since(Start_Time)
			t.Store_Trace()
		}

		printer.TraceChannel <- t.traces
	}()
}

func (t *Wild) Init(id int, symbol rune) {
	t.RelocateChannel = make(chan RelocateRequest)
	t.Id = id
	t.Symbol = symbol
	t.timeAppear = time.Duration(rand.Int63n(int64(int(Max_Delay) * Max_Steps)))
	t.timeDisappear = t.timeAppear + time.Duration(rand.Int63n(int64(int(Max_Delay)*Max_Steps-int(t.timeAppear))))
}

func (t *Wild) Start() {
	go func() {
		time.Sleep(t.timeAppear)

		// try to move in
		t.response = Fail
		for t.response == Fail {
			t.Position = Position{
				X: rand.Intn(Board_Width),
				Y: rand.Intn(Board_Height),
			}

			request := EnterRequest{t, make(chan Response, 1)}
			Board[t.Position.X][t.Position.Y].EnterChannel <- request
			select {
			case t.response = <-request.ResponseChannel:
			case <-time.After(100 * time.Millisecond):
				request.ResponseChannel <- Fail
				t.response = Fail
			}
		}

		t.timeStamp = time.Since(Start_Time)
		t.Store_Trace()

		// main loop
		t.RelocateChannel = make(chan RelocateRequest)
		for true {
			if t.response == Trapped || time.Since(Start_Time) > t.timeDisappear {
				break
			}

			select {
			case Request := <-t.RelocateChannel:
				t.response = Request.Status
				t.Position = Request.Position
				t.timeStamp = time.Since(Start_Time)
				t.Store_Trace()
			case <-time.After(t.timeDisappear - time.Since(Start_Time)):
			}
		}

		// free the board
		if t.response != Trapped {
			Board[t.Position.X][t.Position.Y].LeaveChannel <- true
			t.Position = Position{
				X: Board_Width,
				Y: Board_Height,
			}
			t.timeStamp = time.Since(Start_Time)
			t.Store_Trace()
		}

		printer.TraceChannel <- t.traces
	}()
}

func (t *Trap) Init(id int, symbol rune) {
	t.TrapChannel = make(chan TrapRequest, 4)
	t.Done = make(chan bool)
	t.Id = id
	t.Symbol = '#'
	t.traveler = nil

	// try to move in
	t.response = Fail
	for t.response == Fail {
		t.Position = Position{
			X: rand.Intn(Board_Width),
			Y: rand.Intn(Board_Height),
		}

		request := EnterRequest{t, make(chan Response, 1)}
		Board[t.Position.X][t.Position.Y].EnterChannel <- request
		select {
		case t.response = <-request.ResponseChannel:
		case <-time.After(100 * time.Millisecond):
			request.ResponseChannel <- Fail
			t.response = Fail
		}
	}

	t.timeStamp = time.Since(Start_Time)
	t.Store_Trace()

	t.Start()
}

func (t *Trap) Start() {
	go func() {
		// main loop
		for true {
			if t.response == Deadlock { // t.done sets response to Deadlock to exit loop
				break
			}

			select {
			case Request := <-t.TrapChannel:
				switch v := Request.Traveler.(type) {
				case *Legal:
					t.response = Trapped
					t.Symbol = unicode.ToLower(v.Symbol)
				case *Wild:
					select {
					case v.RelocateChannel <- RelocateRequest{Position{Board_Width, Board_Height}, Trapped}:
						t.response = Trapped
						t.Symbol = '?'
					default:
						t.response = Fail
					}
				default:
					t.response = Fail
				}

				Request.ResponseChannel <- t.response

				// if traveller caught
				if t.response == Trapped {
					t.timeStamp = time.Since(Start_Time)
					t.Store_Trace()

					time.Sleep(2 * Max_Delay)

					t.Symbol = '#'
					t.timeStamp = time.Since(Start_Time)
					t.Store_Trace()
				}
			case <-t.Done:
				t.response = Deadlock // to exit loop
			}
		}

		printer.TraceChannel <- t.traces
	}()
}

func main() {
	var travelers [Nr_Of_Traps + Nr_Of_Travelers + Nr_Of_Wild_Travelers]GeneralTraveler

	fmt.Printf(
		"-1 %d %d %d\n",
		Nr_Of_Traps+Nr_Of_Travelers+Nr_Of_Wild_Travelers,
		Board_Width,
		Board_Height,
	)

	printer.Start()

	for i := 0; i < Board_Width; i++ {
		for j := 0; j < Board_Height; j++ {
			Board[i][j].Init(Position{i, j})
		}
	}

	id := 0
	for i := 0; i < Nr_Of_Traps; i++ {
		travelers[id] = &Trap{}
		travelers[id].Init(id, '#')
		id++
	}

	symbol := 'A'
	for i := 0; i < Nr_Of_Travelers; i++ {
		travelers[id] = &Legal{}
		travelers[id].Init(id, symbol)
		id++
		symbol++
	}

	symbol = '0'
	for i := 0; i < Nr_Of_Wild_Travelers; i++ {
		travelers[id] = &Wild{}
		travelers[id].Init(id, symbol)
		id++
		symbol++
		if symbol > '9' {
			symbol = '0'
		}
	}

	id = Nr_Of_Traps
	for i := 0; i < Nr_Of_Travelers; i++ {
		travelers[id].Start()
		id++
	}

	for i := 0; i < Nr_Of_Wild_Travelers; i++ {
		travelers[id].Start()
		id++
	}

	<-printer.Done

	for i := 0; i < Nr_Of_Traps; i++ {
		trap, _ := travelers[i].(*Trap)
		trap.Done <- true
	}

	<-printer.Done
}
