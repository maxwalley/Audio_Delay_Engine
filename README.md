# Audio_Delay_Engine

Tests can be built and run using CMake through the following commands:

```
mkdir build
cd build
cmake ..
make
./audio_delay_engine_test
```

## Library Design

The Delay and DelayLine classes are designed to be tightly coupled through their friend relationship. This allows the Delay class to access the prepare and process functions of the DelayLine class while exposing functions such as setAmplitude to a user.

The Delay class uses a custom CAS exchange loop to manage it's delay lines in a real-time safe way. This makes the line list non-realtime mutable while allowing the individual lines to manage their own thread/real-time safety.