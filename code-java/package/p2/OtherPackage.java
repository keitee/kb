//
// OtherPackage.java
//

package p2;

class OtherPackage {

  public OtherPackage() {

    p1.Protection p = new p1.Protection();

    System.out.println("other package constructor");
    System.out.println("n = " + n);
    System.out.println("n_pri = " + n_pri);
    System.out.println("n_pro = " + n_pro);
    System.out.println("n_pub = " + n_pub);
  }
}

