class MyThread extends Thread {
  private int argument;
  Display display;

  MyThread(int arg, Display obj) {
    argument = arg;
    display = obj;
  }

  public void run() {
    if (argument == 1050) {
      for (int i=0; i<10; i++) {
        synchronized (display) {
          try {
            display.display("Hello world\n");
          } catch(Exception e) {

          }
        }
      }
    }
    if (argument == 1060) {
      for (int i=0; i<10; i++) {
        synchronized (display) {
          try {
            display.display("Bonjour monde\n");
          } catch(Exception e) {

          }
        }
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

class Main {
  public static void main(String[] args) {
    Display display = new Display();
    MyThread t1 = new MyThread(1050, display);
    MyThread t2 = new MyThread(1060, display);
    t1.start();
    t2.start();
  }
}
