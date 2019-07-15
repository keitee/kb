//
// Demo package p1.
//

// Table 9-1 applies only to members of classes. A non-nested class has only two
// possible access levels: default and public. 
// 
// When a class is declared as public, it is accessible by any other code. If a
// class has default access, then it can only be accessed by other code within
// its *same package*. When a class is public, it must be the only public class
// declared in the file, and the file must have the same name as the class.

import p1.*;

// package p1;

public class DemoP1 {
  public static void main(String args[]) {

    Protection p = new Protection();

    // same package
    Derived d = new Derived();

    // same package
    SamePackage sp = new SamePackage();
  }
}

