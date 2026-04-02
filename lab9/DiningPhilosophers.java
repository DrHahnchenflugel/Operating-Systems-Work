public class DiningPhilosophers {

    private static final int N = 5;

    private static final Semaphore[] fork = new Semaphore[N];
    private static final Semaphore room = new Semaphore(N - 1);

    private static final int[] forkOwner = new int[N]; // -1 means free
    private static int completed = 0;

    static {
        for (int i = 0; i < N; i++) {
            fork[i] = new Semaphore(1);
            forkOwner[i] = -1;
        }
    }

    static class Philosopher extends Thread {
        private final int id; // 0-based internally

        public Philosopher(int id) {
            this.id = id;
        }

        @Override
        public void run() {
            think();

            room.down();

            takeFork(id);
            takeFork((id + 1) % N);

            eat();

            releaseBothForks(id, (id + 1) % N);

            room.up();

            markCompleted();
        }

        private void think() {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }

        private void eat() {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }

        private void takeFork(int forkIndex) {
            synchronized (DiningPhilosophers.class) {
                if (forkOwner[forkIndex] != -1) {
                    System.out.println("Philosopher " + (id + 1)
                            + " is waiting for fork " + (forkIndex + 1));
                }
            }

            fork[forkIndex].down();

            synchronized (DiningPhilosophers.class) {
                forkOwner[forkIndex] = id;
                System.out.println("Fork " + (forkIndex + 1)
                        + " taken by Philosopher " + (id + 1));
            }
        }

        private void releaseBothForks(int firstFork, int secondFork) {
            synchronized (DiningPhilosophers.class) {
                forkOwner[firstFork] = -1;
                forkOwner[secondFork] = -1;

                fork[firstFork].up();
                fork[secondFork].up();

                System.out.println("Philosopher " + (id + 1)
                        + " released fork " + (firstFork + 1)
                        + " and fork " + (secondFork + 1));
            }
        }

        private void markCompleted() {
            synchronized (DiningPhilosophers.class) {
                completed++;
                System.out.println("Philosopher " + (id + 1) + " completed his dinner");
                System.out.println("Till now num of philosophers completed dinner are " + completed);

                if (completed == N) System.out.println("All philosphers have eaten!");
            }
        }
    }

    public static void main(String[] args) {
        Philosopher[] philosophers = new Philosopher[N];

        for (int i = 0; i < N; i++) {
            philosophers[i] = new Philosopher(i);
            philosophers[i].start();
        }

        for (int i = 0; i < N; i++) {
            try {
                philosophers[i].join();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
}