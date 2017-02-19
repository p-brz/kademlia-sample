# Kademlia Sample

This a simple sample to experiment [kademlia](https://github.com/DavidKeller/kademlia) library features.

The sample uses also the [conan](https://www.conan.io/) dependency manager and the 
[Clara](https://github.com/philsquared/Clara) command line parser library.

The code is licensed under the [unlicense](http://unlicense.org/) terms. So in resume can use it for wherever you want.


## Building

1) First you need to install (detailed instructions at [conan docs](http://docs.conan.io/en/latest/installation.html))

        pip install conan

2) Install the packages dependencies

        conan install
        
3) Then build

        ./waf configure build
        
or

        conan build        
        

## Usage

After building, a binary should be created at `build/src/node`. 

1) You can run it with the `--help` option to see the available options

        ./build/src/node --help    

2) You need to run a first node that will bootstrap the dht:

        ./build/src/node

optionally specify a local port to bind:

        ./build/src/node --port 12345


3) Then, you can run one or more nodes, bootstraping with one of the previous nodes:

**Note**: if you run then on same machine, remember to use different ports.
**Note 2**: if you not set the port of bootstrap node it will use the default value

        ./build/src/node "someip" -p 10001
        ./build/src/node "otherip:10001" -p 10002
        ./build/src/node "otherip:10002"
        ...
        
4) For each one node (except the first one), you can use the `get` and `put` commands:

        ... during execution
        put "some key" "my value"
        get "some key"
        

