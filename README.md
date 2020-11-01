# Producer/consumer assignment

## Design considerations

The application launches two threads and controls their operation with a simple
boolean flag `stopped`, which in turn is protected by `stop_processing_mutex`
and signaled with `stop_processing` condition.

Producer queues number from 1 to 100 until the stop condition triggers and
Consumer waits data until the stop condition triggers.

The FIFO is a simple circular buffer, using a `std::array` as backend storage.

The class `semaphore` implements a counting semaphore that can run a
transaction, both when releasing a token as well when aquiring.

I opted for this particular implementation to allow the FIFO block the `put`
operation when the buffer is full. I did not want to lose data, but neither
resize the array.

> I chose to run "Transactions" inside the semaphore to use the same threadsafe
> context that increments/decrements the semaphore counter.

Then, after `transaction` is executed the counter is incremented to allow
consumers to read.

## Build and usage instructions

Build with:

```
g++ src/main.cpp -oprodcons
```

Run:

```
./prodcons
```

You can stop the program pressing enter.
