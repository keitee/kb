// A simple package

// ~/git/kb/code-java/package/my$ javac AccountBalance.java
// ~/git/kb/code-java/package/my$ cd ..
// ~/git/kb/code-java/package$ java my.AccountBalance
// K. J. Fielding: $123.23
// Will Tell: $157.02
// --> Tom Jackson: $-12.33

// As explained, AccountBalance is now part of the package MyPack. This means
// that it cannot be executed by itself. That is, you cannot use this command
// line: java AccountBalance AccountBalance must be qualified with its package
// name.

package my;

class Balance {

  String name;
  double bal;

  Balance(String n, double b) {
    name = n;
    bal = b;
  }

  void show() {
    if(bal<0)
      System.out.print("--> ");
    System.out.println(name + ": $" + bal);
  }
}

class AccountBalance {

  public static void main(String args[]) {

    Balance current[] = new Balance[3];

    current[0] = new Balance("K. J. Fielding", 123.23);
    current[1] = new Balance("Will Tell", 157.02);
    current[2] = new Balance("Tom Jackson", -12.33);

    for(int i=0; i<3; i++) current[i].show();
  }

}
