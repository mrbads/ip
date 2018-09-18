class Print {
  private boolean lock = true;
  private Display display;

  public synchronized void ab(Display display) {
    while (!lock) {
      try {
        // System.out.println("Wait ab");
        wait();
      } catch(Exception e) {}
    }
    lock = false;
    try {
      display.display("ab");
    } catch(Exception e) {}
    notifyAll();
  }
  public synchronized void cd(Display display) {
    while (lock) {
      try {
        // System.out.println("wait cd");
        wait();
      } catch(Exception e) {}
    }
    lock = true;
    try {
      display.display("cd\n");
    } catch(Exception e) {}
    notifyAll();
  }
}

class MyThread2 extends Thread {
  private int argument;
  private Display display;
  private Print print;

  MyThread2(int arg, Display obj, Print p) {
    argument = arg;
    display = obj;
    print = p;
  }

  public void run() {
    int i;
    if (argument == 1050) {
      for (i=0; i<10; i++) {
        print.ab(display);
      }
    }
    if (argument == 1060) {
      for (i=0; i<10; i++) {
        print.cd(display);
      }
    }
  }

}

class Display {
  public void display(String str) throws InterruptedException {
    for (char p : str.toCharArray()) {
      System.out.print(p);
      Thread.sleep(1);
    }
  }
}

class Main2 {
  public static void main(String[] args) {
    Display display = new Display();
    Print print = new Print();
    MyThread2 t1 = new MyThread2(1050, display, print);
    MyThread2 t2 = new MyThread2(1060, display, print);

    t1.start();
    t2.start();
  }
}
