package main

import (
  "fmt"
  hook "github.com/robotn/gohook"
  rbt "github.com/go-vgo/robotgo"
)

func main() {
  add()
}

func KeyTap(key string) {
	rbt.KeyTap(key)
	rbt.MilliSleep(400)
}

func add() {

  hook.Register(hook.KeyDown, []string{"alt","f14"}, func(e hook.Event) {
    code := e.Rawcode
    switch code {
    	case 65409:
		fmt.Println("alt + f13")
		return
	case 65350:
		fmt.Println("alt + f15")
		return
	case 65349:
		fmt.Println("alt + f14")
		return
	case 65351:
		fmt.Println("alt + f16")
		return
	case 65352:
		fmt.Println("alt + f17")
		return
	case 65353:
		fmt.Println("alt + f18")
		return
	case 65458:
		fmt.Println("alt + f20")
		return
	case 65449:
		fmt.Println("alt + f21")
		return
	case 65456:
		fmt.Println("Mechanic Call")
		rbt.MilliSleep(2000)
		KeyTap("up")
		fmt.Println("up")
		rbt.MilliSleep(2000)
		KeyTap("up")
		fmt.Println("up")
		KeyTap("right")
		fmt.Println("right")
		KeyTap("enter")
		fmt.Println("enter")
		return
	case 65457:
		fmt.Println("alt + f19")
		return
	default:
		return
    }
  })

  s := hook.Start()
  <-hook.Process(s)
}

/*
65409, 65350, 65349
65351, 65352, 65353
65458, 65449, 65456
 -   , 65457, - 
*/
