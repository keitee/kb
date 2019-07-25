// Class.java
//
// $ javac Class.java
// $ ls
// A.class  B.class  Class.class  Class.java

// a simple example of inheritance

// superclass

class Hello
{
  void print() {
    System.out.println("Hello class");
  }
}

class A {
  int i, j;
  Hello ho = new Hello() {

    void print_2() {
      System.out.println("Hello ctor");
    }
  };

  A() {
    ho.print();
    System.out.println("A ctor");
  }

  void showij() {
    System.out.println("i and j: " + i + " " + j);
  }
}


// see that ho gets created before running A's ctor
//
// $ java ClassAno
// Hello class
// A ctor

class ClassAno {
  public static void main(String args[]) {

    A a = new A();
  }
}

