package demo

/*

#include <stdio.h>

int sum(int x, int y) {
  return x+y;
}

*/
import "C"

import (
  "fmt"
  "os"
  "reflect"
  "sync"
  "time"
)

var wg sync.WaitGroup

type Pub interface {
  call(string) int
}

type Book struct {
  name string
  id   int
  age  int
}

//Book 实现接口Pub
func (t Book) call(name string) int {
  return len(name)
}

func workRoutine(c chan byte) {
  defer wg.Done()
  defer close(c)

  var i int = 0
  for {
    i++
    if i == 20 {
      return
    }
    c <- byte(i)
    time.Sleep(time.Millisecond * 200)
  }
}

var subscriber map[string][]interface{} = make(map[string][]interface{})

func subscribe(topic string, f interface{}) int {
  item := subscriber[topic]
  if item == nil {
    var t []interface{}
    t = append(t, f)
    subscriber[topic] = t
  } else {
    subscriber[topic] = append(subscriber[topic], f)
  }
  return 0
}

func publish(topic string, content string) {
  item := subscriber[topic]
  if item == nil {
    return
  }
  for _, k := range item {
    //类型断言
    t, _ := k.(func(string, string))
    if t == nil {
      continue
    }
    t(topic, content)
  }
  wg.Done()
}

func sub1(topic string, content string) {
  fmt.Println("i am sub1, recv topic ", topic, " content ", content)
}

func sub2(topic string, content string) {
  fmt.Println("i am sub2, recv topic ", topic, " content ", content)
}

func sub3(topic string, content string) {
  fmt.Println("i am sub3, recv topic ", topic, " content ", content)
}

type ReflectTest struct {
  Name string //字段名首字母大写，public；非大写，private；
  Id   int
}

func demo() {
  //time
  now := time.Now()
  fmt.Printf("hello world! golang! %v\n", now)
  fmt.Printf("Unix() %v\n", now.Unix())
  fmt.Printf("UnixNano %v\n", now.UnixNano())

  //200612345 格式化
  fmt.Println(now.Format("2006-01-02 15:04:05.000"))
  f, err := os.OpenFile("E:\\topic.txt", os.O_RDONLY, 0644)
  if err != nil {
    fmt.Println("open file failed, err: ", err)
    return
  }

  //数组
  var tmp [1024]byte
  n, err := f.Read(tmp[:])
  if err != nil {
    fmt.Println("read failed, err: ", err)
    return
  }
  //延迟调用，在return前调用
  defer f.Close()
  fmt.Printf("read %d\n%s\n", n, string(tmp[:n]))

  rsp := `{"rsp" : "ok"}`
  fmt.Println(rsp)
  //调用C函数
  fmt.Printf("C func called, sum(): %d\n", C.sum(C.int(1), C.int(3)))

  var t Book
  t.name = "xieyongqi"
  t.age = 23
  t.id = 3096
  fmt.Println(t)

  m := make(map[string]int)
  m["xyq"] = 1
  m["wzh"] = 2
  for i := range m {
    fmt.Println(m[i])
  }
  delete(m, "xyq")
  fmt.Println(m)
  //切片，动态数组
  var s []byte
  s = append(s, "helloworld"[:]...)
  fmt.Printf("s len %d cap %d content %v\n", len(s), cap(s), s)

  var p Pub = new(Book)

  fmt.Println(p.call("hello"))
  c1 := make(chan byte, 1024)
  go workRoutine(c1)
  wg.Add(1)
  for {
    select {
    case recv, ok := <-c1:
      if !ok {
        goto exit
      }
      fmt.Println(recv)
    default:
      fmt.Println("No message")
      time.Sleep(time.Second * 1)
    }
  }
exit:
  wg.Wait()
  subscribe("hi", sub1)
  subscribe("hi", sub2)
  subscribe("hi", sub3)
  go publish("hi", "i am xieyongqi, welcome to here.")
  go publish("hi", "i am xieyongqi, welcome to here.")
  wg.Add(2)
  wg.Wait()
  once := sync.Once{}
  once.Do(func() {
    fmt.Println("unknown func, run once")
  })
  twice := sync.Once{}
  twice.Do(func() {
    fmt.Println("unknown func, run twice")
  })

  //反射
  aa := ReflectTest{"xyq", 11}
  tt := reflect.TypeOf(&aa)
  vv := reflect.ValueOf(&aa)

  if tt == nil {
    fmt.Println("tt == nil")
  }
  vv.Elem().FieldByName("Name").SetString("xieyongqi")
  fmt.Println(aa)

  xx := new(ReflectTest)
  xx.Id = 11
  xx.Name = "xyq"
  fmt.Printf("%T %v\n", xx, xx)

  // adr, err := net.ResolveTCPAddr("tcp", ":8080")
  // if err != nil {
  //  fmt.Println("ResolveTCPAddr failed, err ", err)
  //  return
  // }
  // tcp, err := net.ListenTCP("tcp", adr)
  // if err != nil {
  //  fmt.Println("ListenTCP failed, err ", err)
  //  return
  // }

  // con, err := tcp.Accept()
  // if err != nil {
  //  fmt.Println("accept failed, err ", err)
  //  time.Sleep(time.Second * 2)
  //  return
  // }
  // for {
  //  var buf [1024]byte
  //  n, err := con.Read(buf[:])
  //  if err != nil {
  //    fmt.Println("read failed, err ", err)
  //    break
  //  }
  //  fmt.Println("read num ", n, " ", string(buf[:]))
  // }

  fmt.Println("finish work")
}

func Main() {
  demo()
}
