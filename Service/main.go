package main

import (
     "context"
     "encoding/json"
     "github.com/google/uuid"
     "github.com/gorilla/handlers"
     "github.com/gorilla/mux"
     "log"
     "net/http"
     "os"
     "strings"
     "sync"
     "time"
)

var AccessId string
var stop chan int

type Query struct{
     ContextId      string
     TableId        string
     Statement      string
     Connection     string
}

type Result struct{
     Echo           string
     Query          Query
}

func main() {
     AccessId = os.Args[1]
     stop = make(chan int, 1)
     wait := &sync.WaitGroup{}
     
     
     router := mux.NewRouter()
     router.HandleFunc("/",HealthCheckHandler).Methods("GET")
     router.HandleFunc("/query", AccessVerificationHandler(EchoResponseHandler)).Methods("POST")
     router.HandleFunc("/quit", AccessVerificationHandler(QuitRequestHandler)).Methods("GET")
     
     headersOk := handlers.AllowedHeaders([]string{"Content-Type", "Authorization","Content-Length", "Accept"})
     originsOk := handlers.AllowedOrigins([]string{"*"})  // allow all inbound domains
     methodsOk := handlers.AllowedMethods([]string{"GET", "HEAD", "POST"})
     
     // Start Service
     log.Println("Starting Service...")
     
     wait.Add(1)
     server:= &http.Server{Addr:":30200", Handler: handlers.CORS(headersOk, originsOk, methodsOk)(router)}
     
     go func() {
          defer wait.Done()
          if err := server.ListenAndServe() ; err != nil {
               log.Printf("Listen and Serve: %s", err.Error())
          }
     }()
     
     <-stop
     log.Println("Stopping Service...")
     ctx, cancel := context.WithTimeout(context.Background(), 2*time.Second)
     defer cancel()
     if err:= server.Shutdown(ctx); err != nil {
          panic(err)
     }
     
     log.Println("___ END OF LINE ___")
}

func HealthCheckHandler(w http.ResponseWriter, r *http.Request) {
     w.Write([]byte("Health Check OK"))
     w.WriteHeader(http.StatusOK)
     return
     
}

func EchoResponseHandler(w http.ResponseWriter, r *http.Request) {
     // Declare a new Person struct.
     var q Query
     
     // Try to decode the request body into the struct. If there is an error,
     // respond to the client with the error message and a 400 status code.
     err := json.NewDecoder(r.Body).Decode(&q)
     if err != nil {
          http.Error(w, err.Error(), http.StatusBadRequest)
          return
     }
     
     rez := Result{Query: q, Echo: uuid.New().String()}
     
     jbytes, _ := json.Marshal(rez)
     
     w.Write(jbytes)
     w.WriteHeader(http.StatusOK)
}

func QuitRequestHandler(w http.ResponseWriter, r *http.Request) {
     w.WriteHeader(http.StatusOK)
     stop <- 1
}

func AccessVerificationHandler(next http.HandlerFunc) http.HandlerFunc {
     return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request){
          auth := r.Header.Get("Authorization")
          if len(strings.TrimSpace(auth)) <= 0 {
               w.WriteHeader(400)
               w.Write([]byte("Authorization Failed, no principal context provided"))
          }
          
          if auth != AccessId {
               w.WriteHeader(403)
               w.Write([]byte("Authorization Failed"))
          }
          next(w,r)
     })
}






