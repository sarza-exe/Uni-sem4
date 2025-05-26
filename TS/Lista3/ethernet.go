package main

import (
	"fmt"
	"math/rand"
	"os"
	. "time"
)

//# if kabel pusty nadaje
//# else czekam na cisze
//# nadaje i kontroluje stan kabla

const EthernetLength = 30
const Interval = Millisecond * 300
const MaxWait = 50
const LogLines = 800 // number of state lines to log

type readOp struct {
	key  int
	resp chan string
}
type writeOp struct {
	key  int
	val  string
	sign string
	resp chan bool
}

var (
	stations    []string
	connections []string
	logFile     *os.File
)

func main() {
	var err error
	// Create or truncate the log file
	logFile, err = os.Create("ether")
	if err != nil {
		panic(err)
	}
	defer logFile.Close()

	reads := make(chan *readOp)
	writes := make(chan *writeOp)

	stations = fillSlice(nil, " ")
	connections = fillSlice(nil, " ")

	// Launch stations
	go station("A", 4, reads, writes)
	Sleep(Nanosecond)
	go station("B", 15, reads, writes)
	Sleep(Nanosecond)
	// You can uncomment more stations as needed

	ethernet(reads, writes)
}

func ethernet(reads chan *readOp, writes chan *writeOp) {
	// Initialize shared medium
	state := fillSlice(nil, "0")

	// small delay for station registration
	Sleep(10 * Millisecond)

	// Write headers
	fmt.Fprintln(logFile, stations)
	fmt.Fprintln(logFile, connections)

	// Ticker to log every interval
	ticker := NewTicker(Interval)
	defer ticker.Stop()

	lines := 0
	// Log initial state
	fmt.Fprintln(logFile, state)
	lines++

	for lines < LogLines {
		select {
		case write := <-writes:
			// Process write
			if processWrite(state, write) {
				// collision or change handled inside
			}
			// Continue without logging here; logged on ticker

		case read := <-reads:
			read.resp <- state[read.key]

		case <-ticker.C:
			// Log current state every interval
			fmt.Fprintln(logFile, state)
			lines++
		}
	}
}

// processWrite updates state based on writeOp
func processWrite(state []string, write *writeOp) bool {
	isRight := true
	switch write.val {
	case "0":
		if state[write.key] == write.sign || state[write.key] == "=" {
			state[write.key] = write.val
		}
	default:
		if state[write.key] == "0" || state[write.key] == write.val {
			state[write.key] = write.val
		} else {
			state[write.key] = "="
			isRight = false
		}
	}
	write.resp <- isRight
	return isRight
}

func station(signal string, place int, reads chan *readOp, writes chan *writeOp) {
	stations[place] = signal
	connections[place] = "|"
	wait := getRandomTime()
	cu := make(chan bool, 1)
	cd := make(chan bool, 1)
	counter := 0

	Sleep(wait)
	for {
		if isFree(place, reads) {
			sendSignal(place, signal, signal, writes)
			Sleep(Interval)

			go propagateUp(place, signal, signal, writes, cu)
			go propagateDown(place, signal, signal, writes, cd)
			Sleep(Interval)

			c1 := <-cu
			c2 := <-cd

			sendSignal(place, "0", signal, writes)
			Sleep(Interval)

			go propagateUp(place, "0", signal, writes, nil)
			go propagateDown(place, "0", signal, writes, nil)

			if !c1 || !c2 {
				counter++
				wait = getTimeToWait(counter)
			} else {
				counter = 0
				wait = getRandomTime()
			}
			Sleep(wait)
		} else {
			Sleep(Interval)
		}
	}
}

func getTimeToWait(counter int) Duration {
	source := rand.NewSource(Now().UnixNano())
	generator := rand.New(source)
	var maxExp int
	switch {
	case counter < 10:
		maxExp = 1 << counter
	case counter < 17:
		maxExp = 1 << 10
	default:
		fmt.Fprintln(logFile, "Ethernet giving up after too many collisions")
		os.Exit(1)
	}
	r := generator.Intn(maxExp)
	return Duration(r) * Interval
}

func getRandomTime() Duration {
	source := rand.NewSource(Now().UnixNano())
	generator := rand.New(source)
	r := generator.Intn(MaxWait)
	return Duration(r) * Interval
}

func isFree(key int, reads chan *readOp) bool {
	req := &readOp{key: key, resp: make(chan string, 1)}
	reads <- req
	return <-req.resp == "0"
}

func sendSignal(key int, val, sign string, writes chan *writeOp) bool {
	req := &writeOp{key: key, val: val, sign: sign, resp: make(chan bool, 1)}
	writes <- req
	return <-req.resp
}

func propagateUp(src int, signal, sign string, writes chan *writeOp, done chan<- bool) {
	ok := true
	for step := 0; step < EthernetLength*2; step++ {
		for j := src + 1; j < EthernetLength; j++ {
			if !sendSignal(j, signal, sign, writes) {
				ok = false
			}
			Sleep(Interval)
		}
	}
	if done != nil {
		done <- ok
	}
}

func propagateDown(src int, signal, sign string, writes chan *writeOp, done chan<- bool) {
	ok := true
	for step := 0; step < EthernetLength*2; step++ {
		for j := src - 1; j >= 0; j-- {
			if !sendSignal(j, signal, sign, writes) {
				ok = false
			}
			Sleep(Interval)
		}
	}
	if done != nil {
		done <- ok
	}
}

func fillSlice(_ []string, s string) []string {
	out := make([]string, EthernetLength)
	for i := range out {
		out[i] = s
	}
	return out
}
