// Class.java
//
// $ javac Class.java
// $ ls
// A.class  B.class  Class.class  Class.java

// a simple example of inheritance

// superclass

class A {
  int i, j;

  void showij() {
    System.out.println("i and j: " + i + " " + j);
  }
}

class B extends A {
  int k;

  void showK() {
    System.out.println("k: " + k);
  }

  void sum() {
    System.out.println("sum: i + j + k: " + (i+j+k));
  }
}


// $ java Class
// contents of a:
// i and j: 10 20
// 
// contents of b:
// i and j: 7 8
// k: 9
// sum: i + j + k: 24

class Class {
  public static void main(String args[]) {

    {
      A a;
      // a.showij();
    }

    A a = new A();
    B b = new B();

    // access a
    a.i = 10;
    a.j = 20;
    System.out.println("contents of a: ");
    a.showij();
    System.out.println();

    b.i = 7;
    b.j = 8;
    b.k = 9;
    System.out.println("contents of b: ");
    b.showij();
    b.showK();
    b.sum();
    System.out.println();
  }
}

