public class MyThread extends Thread {
  private int argument;

  MyThread(int arg) {
    argument = arg;
  }

  public void run() {
    System.out.println("New thread started! arg=" + argument);
  }
}

public static void main(String[] args) {
  MyThread t = new MyThread(1050);
  t.start();
}
