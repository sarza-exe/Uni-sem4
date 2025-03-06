//
// Created by Sara on 6.03.2025.
//
#include <iostream>
using namespace std;

template <typename T>
class FIFO
{
    private:
      struct Node
      {
        T data;
        Node *next;
        Node(const T& value) : data(value), next(nullptr) {}
      };
      Node *front, *rear;

    public:
      FIFO() : front(nullptr), rear(nullptr) {}
      ~FIFO(){
        while(!isEmpty()) dequeue();
      }

      bool isEmpty()
      {
        return front == nullptr;
      }

      void queue(const T& value)
      {
        Node *newNode = new Node(value);
        if (rear == nullptr){
          front = newNode;
        }
        else{
          rear->next = newNode;
        }
        rear = newNode;
      }

      T dequeue()
      {
        if (front == nullptr){
          throw std::out_of_range("The queue is empty");
        }
        T value = front->data;
        Node *temp = front;
        front = front->next;
        if(front == nullptr){
          rear = nullptr;
        }
        delete temp;
        return value;
     }
};

template <typename T>
class LIFO
{
private:
  struct Node
  {
    T data;
    Node *previous;
    Node(const T& value) : data(value), previous(nullptr) {}
  };
  Node *top;

public:
  LIFO() : top(nullptr) {}
  ~LIFO(){
    while(!isEmpty()) pop();
  }

  bool isEmpty()
  {
    return top == nullptr || top->previous == nullptr;
  }

  void push(const T& value)
  {
    Node *newNode = new Node(value);
    Node *temp = nullptr;
    if (top)
    {
      temp = top;
    }
    top = newNode;
    top->previous = temp;
  }

  T pop()
  {
    if (top == nullptr){
      throw std::out_of_range("The queue is empty");
    }
    T value = top->data;
    Node *temp = top;
    top = top->previous;
    delete temp;
    return value;
  }
};

int main()
{
  FIFO<int> fifo;

  cout << "Queue test. Inserting numbers 0-9." << endl;
  for(int i = 0; i < 10; i++)
  {
    fifo.queue(i);
  }

  cout << "Dequeuing elements in the queue." << endl;
  for(int i = 0; i < 11; i++)
  {
    try{
      cout << fifo.dequeue() << endl;
    }
    catch(const std::out_of_range e){
      cout << "The queue is empty!" << endl;
    }

  }


  LIFO<int> lifo;

  cout << "\nStack test. Pushing numbers 0-9." << endl;
  for(int i = 0; i < 10; i++)
  {
    lifo.push(i);
  }

  cout << "Poping elements in the queue." << endl;
  for(int i = 0; i < 11; i++)
  {
    try{
      cout << lifo.pop() << endl;
    }
    catch(const std::out_of_range e){
      cout << "The stack is empty!" << endl;
    }

  }

  return 0;
}