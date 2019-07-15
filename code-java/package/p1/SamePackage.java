//
// SamePackage.java
//

// Table 9-1 applies only to members of classes. A non-nested class has only two
// possible access levels: default and public. 
// 
// When a class is declared as public, it is accessible by any other code. If a
// class has default access, then it can only be accessed by other code within
// its *same package*. When a class is public, it must be the only public class
// declared in the file, and the file must have the same name as the class.

package p1;

class SamePackage {

  SamePackage() {

    Protection p = new Protection();

    System.out.println("same package constructor");
    System.out.println("n = " + p.n);

    // ./p1/SamePackage.java:16: error: n_pri has private access in Protection
    // System.out.println("n_pri = " + p.n_pri);

    System.out.println("n_pro = " + p.n_pro);
    System.out.println("n_pub = " + p.n_pub);
  }
}
